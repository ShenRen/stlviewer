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

#include "glmdichild.h"
#include "entity.h"

GLMdiChild::GLMdiChild(QWidget *parent)
    : GLWidget(parent) {
  entity_ = new Entity;

  setAttribute(Qt::WA_DeleteOnClose);
  isUntitled = true;
}

GLMdiChild::~GLMdiChild() {
  delete entity_;
}

void GLMdiChild::newFile() {
  static int sequenceNumber = 1;

  isUntitled = true;
  curFile = tr("untitled%1.stl").arg(sequenceNumber++);
  setWindowTitle(curFile);
}

bool GLMdiChild::loadFile(const QString &fileName) {

  QApplication::setOverrideCursor(Qt::WaitCursor);
  entity_->open(fileName.toStdString());
  makeObjectFromEntity(entity_);
  QApplication::restoreOverrideCursor();

  updateGL();
  setCurrentFile(fileName);

  //QString data;

  //data.setNum(entity_->stats_.max.x);
  //xMax->setText(data);
  //data.setNum(entity_->stats_.min.x);
  //xMin->setText(data);
  //data.setNum(entity_->stats_.max.x-entity_->stats_.min.x);
  //xDelta->setText(data);

  //data.setNum(entity_->stats_.max.y);
  //yMax->setText(data);
  //data.setNum(entity_->stats_.min.y);
  //yMin->setText(data);
  //data.setNum(entity_->stats_.max.y-entity_->stats_.min.y);
  //yDelta->setText(data);

  //data.setNum(entity_->stats_.max.z);
  //zMax->setText(data);
  //data.setNum(entity_->stats_.min.z);
  //zMin->setText(data);
  //data.setNum(entity_->stats_.max.z-entity_->stats_.min.z);
  //zDelta->setText(data);

  //data.setNum(entity_->stats_.num_facets);
  //num_facets->setText(data);
  ////data.setNum(entity_->stats_.num_points);
  ////num_points->setText(data);

  //data.setNum(entity_->stats_.volume);
  //volume->setText(data);
  ////data.setNum(entity_->stats_.surface);
  ////surface->setText(data);

  return true;
}

bool GLMdiChild::save() {
  if (isUntitled) {
    return saveAs();
  } else {
    return saveFile(curFile);
  }
}

bool GLMdiChild::saveAs() {
  //QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile, tr("STL Files (*.stl);;All Files (*.*)"));
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
  if (fileName.isEmpty())
    return false;
  return saveFile(fileName);
}

bool GLMdiChild::saveFile(const QString &fileName) {
  QApplication::setOverrideCursor(Qt::WaitCursor);
  entity_->write(fileName.toStdString());
  QApplication::restoreOverrideCursor();
  setCurrentFile(fileName);
  return true;
}

QString GLMdiChild::userFriendlyCurrentFile() {
  return strippedName(curFile);
}

void GLMdiChild::closeEvent(QCloseEvent *event) {
  entity_->close();
  if (maybeSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void GLMdiChild::setCurrentFile(const QString &fileName) {
  curFile = QFileInfo(fileName).canonicalFilePath();
  isUntitled = false;
  setWindowModified(false);
  //setWindowTitle(tr("%1[*] - %2").arg(userFriendlyCurrentFile()).arg(tr("STLViewer")));
  setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString GLMdiChild::strippedName(const QString &fullFileName) {
   return QFileInfo(fullFileName).fileName();
}

bool GLMdiChild::maybeSave() {
  return true;
}