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

#include <iostream>
#include <fstream>

#include "glwidget.h"
#include "stlviewer.h"

STLViewer::STLViewer(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags) {

  glWidget = new GLWidget;
  setCentralWidget(glWidget);

  entity_ = new Entity;

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createDockWindows();

  readSettings();

  setCurrentFile("");
  setUnifiedTitleAndToolBarOnMac(true);
}

STLViewer::~STLViewer() {
  delete entity_;
}

void STLViewer::closeEvent(QCloseEvent *event) {
  writeSettings();
  entity_->close();
  event->accept();
}

void STLViewer::newFile() {
  entity_->close();
  setCurrentFile(QString("untitled.stl"));
  glWidget->deleteObject();
  xMax->setText("");
  xMin->setText("");
  xDelta->setText("");

  yMax->setText("");
  yMin->setText("");
  yDelta->setText("");

  zMax->setText("");
  zMin->setText("");
  zDelta->setText("");

  num_facets->setText("");
  num_points->setText("");

  volume->setText("");
  surface->setText("");
}

void STLViewer::open() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(), tr("STL Files (*.stl);;Tous les fichiers (*.*)"));
  if (!fileName.isEmpty())
    loadFile(fileName);
}

bool STLViewer::save() {
  if (curFile.isEmpty()) {
    return saveAs();
  } else {
    return saveFile(curFile);
  }
}

bool STLViewer::saveAs() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer un fichier"), QString(), tr("STL Files (*.stl);;Tous les fichiers (*.*)"));
  if (fileName.isEmpty())
    return false;
  return saveFile(fileName);
}

void STLViewer::rotate() {
  if (rotateAct->isChecked()) {
    rotateAct->setChecked(true);
    translateAct->setChecked(false);
    glWidget->setRotationMode(true);
    glWidget->setTranslationMode(false);
  } else {
    rotateAct->setChecked(false);
    glWidget->setRotationMode(false);
  }
}

void STLViewer::translate() {
  if (translateAct->isChecked()) {
    translateAct->setChecked(true);
    rotateAct->setChecked(false);
    glWidget->setTranslationMode(true);
    glWidget->setRotationMode(false);
  } else {
    translateAct->setChecked(false);
    glWidget->setTranslationMode(false);
  }
}

void STLViewer::zoom() {}

void STLViewer::home() {
  glWidget->setDefaultCoordinates();
}

void STLViewer::about() {
  QMessageBox::about(this, tr("A propos"),
          tr("<b>STLViewer</b> simule la visualisation de la tête fémorale "
             "par une caméra endoscopique mobile."));
}

void STLViewer::createActions() {
  newAct = new QAction(QIcon(":STLViewer/Images/page_white.png"), tr("&Nouveau..."), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Créer un fichier"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":STLViewer/Images/folder.png"), tr("&Ouvrir..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Ouvrir un fichier..."));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":STLViewer/Images/disk.png"), tr("&Enregistrer"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Enregistrer le fichier"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("&Enregistrer sous..."), this);
  saveAsAct->setShortcut(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Enregistrer le fichier sous..."));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  rotateAct = new QAction(QIcon(":STLViewer/Images/arrow_rotate_clockwise.png"), tr("&Rotation"), this);
  rotateAct->setStatusTip(tr("Effectuer une rotation"));
  rotateAct->setCheckable(true);
  connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));
  rotateAct->setChecked(false);
    
  translateAct = new QAction(QIcon(":STLViewer/Images/arrow_out.png"), tr("&Translation"), this);
  translateAct->setStatusTip(tr("Effectuer une translation"));
  translateAct->setCheckable(true);
  connect(translateAct, SIGNAL(triggered()), this, SLOT(translate()));
  translateAct->setChecked(false);

  zoomAct = new QAction(QIcon(":STLViewer/Images/magnifier_zoom_in.png"), tr("&Zoom"), this);
  zoomAct->setStatusTip(tr("Zoomer sur la scène"));
  connect(zoomAct, SIGNAL(triggered()), this, SLOT(zoom()));

  homeAct = new QAction(QIcon(":STLViewer/Images/page_white_magnify.png"), tr("&Zoom par défaut"), this);
  homeAct->setStatusTip(tr("Zoom par défaut"));
  connect(homeAct, SIGNAL(triggered()), this, SLOT(home()));

  exitAct = new QAction(tr("Quitter"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Quitter l'application"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  aboutAct = new QAction(tr("&A propos"), this);
  aboutAct->setStatusTip(tr("A propos de STLViewer"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void STLViewer::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&Fichier"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&Affichage"));
  viewMenu->addAction(rotateAct);
  viewMenu->addAction(translateAct);
  viewMenu->addAction(zoomAct);
  viewMenu->addAction(homeAct);
  viewMenu->addSeparator();

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Aide"));
  helpMenu->addAction(aboutAct);
}

void STLViewer::createToolBars() {
  fileToolBar = addToolBar(tr("Fichier"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  viewToolBar = addToolBar(tr("Affichage"));
  viewToolBar->addAction(rotateAct);
  viewToolBar->addAction(translateAct);
  viewToolBar->addAction(zoomAct);
  viewToolBar->addAction(homeAct);
}

void STLViewer::createStatusBar() {
  statusBar()->showMessage(tr("Prêt"));
}

void STLViewer::readSettings() {
  QSettings settings("Surgiqual", "STLViewer");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
}

void STLViewer::writeSettings() {
  QSettings settings("Surgiqual", "STLViewer");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

void STLViewer::loadFile(const QString &fileName) {
  entity_->close();
  glWidget->deleteObject();
  QApplication::setOverrideCursor(Qt::WaitCursor);
  entity_->open(fileName.toStdString());
  glWidget->makeObjectFromEntity(entity_);
  QApplication::restoreOverrideCursor();

  QString data;

  data.setNum(entity_->stats_.max.x);
  xMax->setText(data);
  data.setNum(entity_->stats_.min.x);
  xMin->setText(data);
  data.setNum(entity_->stats_.max.x-entity_->stats_.min.x);
  xDelta->setText(data);

  data.setNum(entity_->stats_.max.y);
  yMax->setText(data);
  data.setNum(entity_->stats_.min.y);
  yMin->setText(data);
  data.setNum(entity_->stats_.max.y-entity_->stats_.min.y);
  yDelta->setText(data);

  data.setNum(entity_->stats_.max.z);
  zMax->setText(data);
  data.setNum(entity_->stats_.min.z);
  zMin->setText(data);
  data.setNum(entity_->stats_.max.z-entity_->stats_.min.z);
  zDelta->setText(data);

  data.setNum(entity_->stats_.num_facets);
  num_facets->setText(data);
  //data.setNum(entity_->stats_.num_points);
  //num_points->setText(data);

  data.setNum(entity_->stats_.volume);
  volume->setText(data);
  //data.setNum(entity_->stats_.surface);
  //surface->setText(data);

  glWidget->updateGL();
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("Fichier chargé"), 2000);
}

bool STLViewer::saveFile(const QString &fileName) {
  QApplication::setOverrideCursor(Qt::WaitCursor);
  entity_->write(fileName.toStdString());
  QApplication::restoreOverrideCursor();
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("Fichier sauvé"), 2000);
  return true;
}

void STLViewer::setCurrentFile(const QString &fileName) {
  curFile = fileName;
  setWindowModified(false);

  QString shownName;
  if (curFile.isEmpty())
    shownName = "untitled.stl";
  else
    shownName = strippedName(curFile);

  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("STLViewer")));
}

QString STLViewer::strippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).fileName();
}

void STLViewer::createDockWindows() {
  QDockWidget *dock = new QDockWidget(tr("Informations"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  createDimensionsGroupBox();
  createMeshInformationGroupBox();
  createPropertiesGroupBox();

  QWidget *wi = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(dimensionsGroupBox);
  layout->addWidget(meshInformationGroupBox);
  layout->addWidget(propertiesGroupBox);

  wi->setLayout(layout);
  wi->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
  dock->setWidget(wi);

  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());
}

void STLViewer::createDimensionsGroupBox() {
  dimensionsGroupBox = new QGroupBox(tr("Dimensions"), this);

  QGridLayout *layout = new QGridLayout;

  QLabel *label = new QLabel("Min");
  label->setAlignment(Qt::AlignHCenter);
  layout->addWidget(label, 0, 1);
  label = new QLabel("Max");
  label->setAlignment(Qt::AlignHCenter);
  layout->addWidget(label, 0, 2);
  label = new QLabel("Delta");
  label->setAlignment(Qt::AlignHCenter);
  layout->addWidget(label, 0, 3);
  layout->addWidget(new QLabel("X"), 1, 0);
  xMin = new QLabel("");
  xMin->setAlignment(Qt::AlignRight);
  layout->addWidget(xMin, 1, 1);
  xMax = new QLabel("");
  xMax->setAlignment(Qt::AlignRight);
  layout->addWidget(xMax, 1, 2);
  xDelta = new QLabel("");
  xDelta->setAlignment(Qt::AlignRight);
  layout->addWidget(xDelta, 1, 3);
  layout->addWidget(new QLabel("Y"), 2, 0);
  yMin = new QLabel("");
  yMin->setAlignment(Qt::AlignRight);
  layout->addWidget(yMin, 2, 1);
  yMax = new QLabel("");
  yMax->setAlignment(Qt::AlignRight);
  layout->addWidget(yMax, 2, 2);
  yDelta = new QLabel("");
  yDelta->setAlignment(Qt::AlignRight);
  layout->addWidget(yDelta, 2, 3);
  layout->addWidget(new QLabel("Z"), 3, 0);
  zMin = new QLabel("");
  zMin->setAlignment(Qt::AlignRight);
  layout->addWidget(zMin, 3, 1);
  zMax = new QLabel("");
  zMax->setAlignment(Qt::AlignRight);
  layout->addWidget(zMax, 3, 2);
  zDelta = new QLabel("");
  zDelta->setAlignment(Qt::AlignRight);
  layout->addWidget(zDelta, 3, 3);

  layout->addWidget(new QLabel("mm"), 1, 4);
  layout->addWidget(new QLabel("mm"), 2, 4);
  layout->addWidget(new QLabel("mm"), 3, 4);

  layout->setColumnMinimumWidth(0, 20);
  layout->setColumnMinimumWidth(1, 50);
  layout->setColumnMinimumWidth(2, 50);
  layout->setColumnMinimumWidth(3, 50);

  dimensionsGroupBox->setLayout(layout);
}

void STLViewer::createMeshInformationGroupBox() {
  meshInformationGroupBox = new QGroupBox(tr("Infos maillage"), this);
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(new QLabel("# Facettes:"), 0, 0);
  num_facets = new QLabel("");
  num_facets->setAlignment(Qt::AlignLeft);
  layout->addWidget(num_facets, 0, 1);
  layout->addWidget(new QLabel("# Points:"), 1, 0);
  num_points = new QLabel("");
  num_points->setAlignment(Qt::AlignLeft);
  layout->addWidget(num_points, 1, 1);

  meshInformationGroupBox->setLayout(layout);
}

void STLViewer::createPropertiesGroupBox() {
  propertiesGroupBox = new QGroupBox(tr("Propriétés"), this);
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(new QLabel("Volume:"), 0, 0);
  volume = new QLabel("");
  volume->setAlignment(Qt::AlignRight);
  layout->addWidget(volume, 0, 1);
  layout->addWidget(new QLabel("Surface:"), 1, 0);
  surface = new QLabel("");
  surface->setAlignment(Qt::AlignRight);
  layout->addWidget(surface, 1, 1);
  layout->addWidget(new QLabel("mm^3"), 0, 2);
  layout->addWidget(new QLabel("mm^3"), 1, 2);

  propertiesGroupBox->setLayout(layout);
}