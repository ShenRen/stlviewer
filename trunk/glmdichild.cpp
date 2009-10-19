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
  QString filterBin = tr("STL Files, binary (*.stl)");
  QString filterAscii = tr("STL Files, ASCII (*.stl)");
  QString filterAll = tr("All files (*.*)");
  QString filterSel;
  if (entity_->stats().type == Entity::ASCII)
    filterSel = filterAscii;
  else
    filterSel = filterBin;
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile, filterBin + ";;" + filterAscii + ";;" + filterAll, &filterSel);
  if (fileName.isEmpty())
    return false;
  if (filterSel == filterBin)
    entity_->setFormat(Entity::BINARY);
  else if (filterSel == filterAscii)
    entity_->setFormat(Entity::ASCII);
  return saveFile(fileName);
}

bool GLMdiChild::saveFile(const QString &fileName) {
  QApplication::setOverrideCursor(Qt::WaitCursor);
  entity_->write(fileName.toStdString());
  QApplication::restoreOverrideCursor();
  setCurrentFile(fileName);
  return true;
}

bool GLMdiChild::saveImage() {
  QFileInfo fi(curFile);
  QString imFile = fi.path() + "/" + fi.completeBaseName() + ".png";
  QString filterPng = tr("PNG Files (*.png)");
  QString filterBmp = tr("BMP Files (*.bmp)");
  QString filterAll = tr("All files (*.*)");
  QString filterSel = filterPng;
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), imFile, filterPng + ";;" + filterBmp + ";;" + filterAll, &filterSel);
  if (fileName.isEmpty())
    return false;
  QString format = "png";
  if (filterSel == filterBmp || QFileInfo(fileName).suffix() == ".bmp")
    format = "bmp";  
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QPixmap originalPixmap = QPixmap::grabWindow(this->winId());
  originalPixmap.save(fileName, format.toAscii());
  QApplication::restoreOverrideCursor();
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

void GLMdiChild::mousePressEvent(QMouseEvent *event) {
  emit mouseButtonPressed(event->buttons());
  GLWidget::mousePressEvent(event);
}

void GLMdiChild::mouseReleaseEvent(QMouseEvent *event) {
  emit mouseButtonReleased(event->button());
  GLWidget::mouseReleaseEvent(event);
}