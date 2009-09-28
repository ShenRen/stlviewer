// Copyright (c) 2009 Olivier Crave
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include "glwidget.h"
#include "entity.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent) {
  object = 0;
  xRot = yRot = zRot = 0;
  xPos= yPos= zPos= 0;
  xTrans= yTrans= zTrans= 0;
  zoomDefault_ = zoom_ = 1.0;
  stepSize_ = 0;
  rotationMode = false;
  translationMode = false;

  grey = QColor::fromRgbF(0.6, 0.6, 0.6);
  purple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget() {
  makeCurrent();
  glDeleteLists(object, 1);
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
  return QSize(400, 400);
}

void GLWidget::updateCursor() {
  QCursor cursor = this->cursor();
  cursor.setShape(Qt::ArrowCursor);
  if (rotationMode)
    cursor.setShape(Qt::SizeAllCursor);
  else if (translationMode)
    cursor.setShape(Qt::SizeAllCursor);
  QWidget::setCursor(cursor);
}

void GLWidget::setTranslationMode(bool mode) {
  translationMode = mode;
  updateCursor();
}

void GLWidget::setRotationMode(bool mode) {
  rotationMode = mode;
  updateCursor();
}

void GLWidget::setXRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != xRot) {
    xRot = angle;
    emit xRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setYRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != yRot) {
    yRot = angle;
    emit yRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setZRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != zRot) {
    zRot = angle;
    emit zRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setDefaultCoordinates() {
  xRot = yRot = zRot = 0;
  xTrans = yTrans = zTrans = 0;
  zoom_ = zoomDefault_;
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glGetFloatv(GL_MODELVIEW_MATRIX, panMatrix);
  glPopMatrix();
  updateGL();
}

void GLWidget::setXTranslation(int distance) {
  if (distance != xTrans) {
    xTrans = distance;
    emit xTranslationChanged(distance);
    updateGL();
  }
}

void GLWidget::setYTranslation(int distance) {
  if (distance != yTrans) {
    yTrans = distance;
    emit yTranslationChanged(distance);
    updateGL();
  }
}

void GLWidget::setZoom(float zoom) {
  if (zoom != zoom_) {
    zoom_ = zoom;
    if (zoom <= 0)
      zoom_ = 0.001;
    emit zoomChanged(zoom);
    updateGL();
  }
}

void GLWidget::initializeGL() {
  qglClearColor(purple.dark());
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
  // Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void GLWidget::paintGL() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-300/zoom_, +300/zoom_, +300/zoom_, -300/zoom_, -1500000, 1500000);
  glOrtho(-zoom_, +zoom_, +zoom_, -zoom_, -zoomDefault_, zoomDefault_);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  //gluLookAt(zoom_, zoom_, zoom_, xPos, yPos, zPos, 0, 1, 0);
  glTranslated(-xTrans, -yTrans, -zTrans);
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
  glMultMatrixf(panMatrix);
  //glGetDoublev(GL_MODELVIEW_MATRIX, panMatrix);
  //glLoadMatrixd(panMatrix);
  //glTranslated(-xPos-xTrans, -yPos-yTrans, -zPos-zTrans);
  glTranslated(-xPos, -yPos, -zPos);
  //glTranslated(-xPos, -yPos, -zPos);
  glCallList(object);
  drawAxes();  
}

void GLWidget::resizeGL(int width, int height) {
  int side = qMax(width, height);
  glViewport((width - side) / 2, (height - side) / 2, side, side);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-300/zoom_, +300/zoom_, +300/zoom_, -300/zoom_, -1500000, 1500000);
  glOrtho(-zoom_, +zoom_, +zoom_, -zoom_, -zoomDefault_, zoomDefault_);
  glMatrixMode(GL_MODELVIEW);
}

void GLWidget::drawAxes() {
  glPushMatrix();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glLineWidth(2.0);
  glScalef(zoomDefault_/4, zoomDefault_/4, zoomDefault_/4);
  glBegin(GL_LINES);
  qglColor(QColor::fromRgbF(1, 0, 0));
  //glColor3f(1,0,0); // X axis is red.
  glVertex3f(0,0,0);
  glVertex3f(1,0,0);
  qglColor(QColor::fromRgbF(0, 1, 0));
  //glColor3f(0,1,0); // Y axis is green.
  glVertex3f(0,0,0);
  glVertex3f(0,1,0);
  qglColor(QColor::fromRgbF(0, 0, 1));
  //glColor3f(0,0,1); // Z axis is blue.
  glVertex3f(0,0,0);
  glVertex3f(0,0,1);
  glEnd();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
  QCursor cursor = this->cursor();
  if (rotationMode)
    cursor.setShape(Qt::SizeAllCursor);
  if (translationMode)
    cursor.setShape(Qt::SizeAllCursor);
  if (event->buttons() & Qt::RightButton) {
    cursor.setShape(Qt::SizeAllCursor);
  } else if (event->buttons() & Qt::MidButton) {
    cursor.setShape(Qt::SizeAllCursor);
  }
  QWidget::setCursor(cursor);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  updateCursor();
  if (event->button() & Qt::MidButton || (event->button() & Qt::MidButton && translationMode)) {

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(-xTrans, -yTrans, -zTrans);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    glMultMatrixf(panMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, panMatrix);
    glPopMatrix();

    xRot = yRot = zRot = 0;
    xTrans = yTrans = zTrans = 0;
    updateGL();
  }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
    if (translationMode) {
      setXTranslation(xTrans + dx*zoom_/100);
      setYTranslation(yTrans + dy*zoom_/100);
    } else if (rotationMode) {
      setXRotation(xRot + 8 * dy);
      setZRotation(zRot + 8 * dx);
    }
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(xRot + 8 * dy);
    setZRotation(zRot + 8 * dx);
  } else if (event->buttons() & Qt::MidButton) {
    setXTranslation(xTrans + dx*zoom_/100);
    setYTranslation(yTrans + dy*zoom_/100);
  }
  lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
  int delta = event->delta();
  //setZoom(zoom_ - delta*0.001);
  setZoom(zoom_ - delta*stepSize_);
}

void GLWidget::makeObjectFromEntity(Entity *entity) {
  object = glGenLists(1);
  glNewList(object, GL_COMPILE);

  glBegin(GL_TRIANGLES);

  for (int i = 0; i < entity->stats_.num_facets; ++i) {
    glNormal3d(entity->facets_[i].normal.x, entity->facets_[i].normal.y, entity->facets_[i].normal.z);
    triangle(entity->facets_[i].vertex[0].x, entity->facets_[i].vertex[0].y, entity->facets_[i].vertex[0].z,
        entity->facets_[i].vertex[1].x, entity->facets_[i].vertex[1].y, entity->facets_[i].vertex[1].z,
        entity->facets_[i].vertex[2].x, entity->facets_[i].vertex[2].y, entity->facets_[i].vertex[2].z);
  }

  glEnd();

  glEndList();

  xPos = (entity->stats_.max.x+entity->stats_.min.x)/2;
  yPos = (entity->stats_.max.y+entity->stats_.min.y)/2;
  zPos = (entity->stats_.max.z+entity->stats_.min.z)/2;
  zoomDefault_ = qMax(qMax(qAbs(entity->stats_.max.x-entity->stats_.min.x),
    qAbs(entity->stats_.max.y-entity->stats_.min.y)),
    qAbs(entity->stats_.max.z-entity->stats_.min.z));
  stepSize_ = zoomDefault_/1000;  

  setDefaultCoordinates();
}

void GLWidget::deleteObject() {
  glDeleteLists(object, 1);
  updateGL();
}

void GLWidget::triangle(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2,
                    GLdouble y2, GLdouble z2, GLdouble x3, GLdouble y3, GLdouble z3) {
  qglColor(grey);
  glVertex3d(x1, y1, z1);
  glVertex3d(x2, y2, z2);
  glVertex3d(x3, y3, z3);
}

void GLWidget::normalizeAngle(int *angle) {
  while (*angle < 0)
    *angle += 360 * 16;
  while (*angle > 360 * 16)
    *angle -= 360 * 16;
}
