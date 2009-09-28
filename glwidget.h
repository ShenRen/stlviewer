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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>

class Entity;

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget();
  void makeObjectFromEntity(Entity*);
  void deleteObject();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  void setDefaultCoordinates();
  void setTranslationMode(bool);
  void setRotationMode(bool);

 public slots:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void setXTranslation(int distance);
  void setYTranslation(int distance);
  void setZoom(float zoom);

 signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);
  void xTranslationChanged(int distance);
  void yTranslationChanged(int distance);
  void zoomChanged(float zoom);

 protected:
  void paintGL();
  void initializeGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

 private:
  void triangle(GLdouble, GLdouble, GLdouble, GLdouble,
             GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
  void extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
  void normalizeAngle(int *angle);
  void drawAxes();
  void updateCursor();

  GLfloat panMatrix[16];
  GLuint object;
  bool rotationMode;
  bool translationMode;
  int xRot;
  int yRot;
  int zRot;
  int xPos;
  int yPos;
  int zPos;
  int xTrans;
  int yTrans;
  int zTrans;
  float zoom_;
  float zoomDefault_;
  float stepSize_;
  QPoint lastPos;
  QColor grey;
  QColor purple;
};

#endif  // GLWIDGET_H
