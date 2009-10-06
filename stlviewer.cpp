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

#include "stlviewer.h"
#include "glmdichild.h"
#include "dimensionsgroupbox.h"
#include "meshinformationgroupbox.h"
#include "propertiesgroupbox.h"

STLViewer::STLViewer(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags) {

  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);

  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
       this, SLOT(updateMenus()));
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget *)),
       this, SLOT(setActiveSubWindow(QWidget *)));

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createDockWindows();
  updateMenus();

  readSettings();

  setWindowTitle(tr("STLViewer"));
  setUnifiedTitleAndToolBarOnMac(true);
}

STLViewer::~STLViewer() {}

void STLViewer::closeEvent(QCloseEvent *event) {
  mdiArea->closeAllSubWindows();
  if (activeGLMdiChild()) {
    event->ignore();
  } else {
    writeSettings();
    event->accept();
  }
}

void STLViewer::newFile() {
  GLMdiChild *child = createGLMdiChild();
  child->newFile();
  child->show();

  dimensionsGroupBox->reset();
  meshInformationGroupBox->reset();
  propertiesGroupBox->reset();
}

void STLViewer::open() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a file"), QString(), tr("STL Files (*.stl);;All Files (*.*)"));
  if (!fileName.isEmpty()) {
    QMdiSubWindow *existing = findGLMdiChild(fileName);
    if (existing) {
      mdiArea->setActiveSubWindow(existing);
      return;
    }
    GLMdiChild *child = createGLMdiChild();
    if (child->loadFile(fileName)) {
      statusBar()->showMessage(tr("File loaded"), 2000);
      child->show();
    } else {
      child->close();
    }
  }
}

void STLViewer::save() {
  if (activeGLMdiChild() && activeGLMdiChild()->save())
    statusBar()->showMessage(tr("File saved"), 2000);
}

void STLViewer::saveAs() {
  if (activeGLMdiChild() && activeGLMdiChild()->saveAs())
    statusBar()->showMessage(tr("File saved"), 2000);
}

void STLViewer::rotate() {
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());
  if (rotateAct->isChecked()) {
    rotateAct->setChecked(true);
    translateAct->setChecked(false);
    for (int i = 0; i < windows.size(); ++i) {
      GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());
      child->setRotationMode(true);
      child->setTranslationMode(false);
    }
  } else {
    rotateAct->setChecked(false);
    for (int i = 0; i < windows.size(); ++i) {
      GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());
      child->setRotationMode(false);
    }
  }
}

void STLViewer::translate() {
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());
  if (translateAct->isChecked()) {
    translateAct->setChecked(true);
    rotateAct->setChecked(false);
    for (int i = 0; i < windows.size(); ++i) {
      GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());
      child->setTranslationMode(true);
      child->setRotationMode(false);
    }
  } else {
    translateAct->setChecked(false);
    for (int i = 0; i < windows.size(); ++i) {
      GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());
      child->setTranslationMode(false);
    }
  }
}

void STLViewer::zoom() {}

void STLViewer::home() {
  activeGLMdiChild()->setDefaultCoordinates();
}

void STLViewer::about() {
  QMessageBox::about(this, tr("About STLViewer"),
          tr("<b>STLViewer</b> simule la visualisation de la tête fémorale "
             "par une caméra endoscopique mobile."));
}

void STLViewer::updateMenus() {
  bool hasGLMdiChild = (activeGLMdiChild() != 0);
  saveAct->setEnabled(hasGLMdiChild);
  saveAsAct->setEnabled(hasGLMdiChild);
  closeAct->setEnabled(hasGLMdiChild);
  closeAllAct->setEnabled(hasGLMdiChild);
  tileAct->setEnabled(hasGLMdiChild);
  cascadeAct->setEnabled(hasGLMdiChild);
  nextAct->setEnabled(hasGLMdiChild);
  previousAct->setEnabled(hasGLMdiChild);
  separatorAct->setVisible(hasGLMdiChild);
  if (hasGLMdiChild) {
    dimensionsGroupBox->setValues(activeGLMdiChild()->getStats());
    meshInformationGroupBox->setValues(activeGLMdiChild()->getStats());
    propertiesGroupBox->setValues(activeGLMdiChild()->getStats());
  } else {
    dimensionsGroupBox->reset();
    meshInformationGroupBox->reset();
    propertiesGroupBox->reset();
  }
}

void STLViewer::updateWindowMenu() {
  windowMenu->clear();
  windowMenu->addAction(closeAct);
  windowMenu->addAction(closeAllAct);
  windowMenu->addSeparator();
  windowMenu->addAction(tileAct);
  windowMenu->addAction(cascadeAct);
  windowMenu->addSeparator();
  windowMenu->addAction(nextAct);
  windowMenu->addAction(previousAct);
  windowMenu->addAction(separatorAct);

  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());

  for (int i = 0; i < windows.size(); ++i) {
    GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());

    QString text;
    if (i < 9) {
      text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
    } else {
      text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
    }
    QAction *action  = windowMenu->addAction(text);
    action->setCheckable(true);
    action ->setChecked(child == activeGLMdiChild());
    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(action, windows.at(i));
  }
}

GLMdiChild *STLViewer::createGLMdiChild() {
  GLMdiChild *child = new GLMdiChild;
  mdiArea->addSubWindow(child);

  return child;
}

void STLViewer::createActions() {

  newAct = new QAction(QIcon(":STLViewer/Images/page_white.png"), tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":STLViewer/Images/folder.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":STLViewer/Images/disk.png"), tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("Save &As..."), this);
  saveAsAct->setShortcut(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  closeAct = new QAction(tr("Cl&ose"), this);
  closeAct->setShortcut(tr("Ctrl+F4"));
  closeAct->setStatusTip(tr("Close the active window"));
  connect(closeAct, SIGNAL(triggered()),
    mdiArea, SLOT(closeActiveSubWindow()));

  closeAllAct = new QAction(tr("Close &All"), this);
  closeAllAct->setStatusTip(tr("Close all the windows"));
  connect(closeAllAct, SIGNAL(triggered()),
    mdiArea, SLOT(closeAllSubWindows()));

  tileAct = new QAction(tr("&Tile"), this);
  tileAct->setStatusTip(tr("Tile the windows"));
  connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

  cascadeAct = new QAction(tr("&Cascade"), this);
  cascadeAct->setStatusTip(tr("Cascade the windows"));
  connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

  nextAct = new QAction(tr("Ne&xt"), this);
  nextAct->setShortcuts(QKeySequence::NextChild);
  nextAct->setStatusTip(tr("Move the focus to the next window"));
  connect(nextAct, SIGNAL(triggered()),
      mdiArea, SLOT(activateNextSubWindow()));

  previousAct = new QAction(tr("Pre&vious"), this);
  previousAct->setShortcuts(QKeySequence::PreviousChild);
  previousAct->setStatusTip(tr("Move the focus to the previous "
      "window"));
  connect(previousAct, SIGNAL(triggered()),
      mdiArea, SLOT(activatePreviousSubWindow()));

  separatorAct = new QAction(this);
  separatorAct->setSeparator(true);

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

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("About STLViewer"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void STLViewer::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(rotateAct);
  viewMenu->addAction(translateAct);
  viewMenu->addAction(zoomAct);
  viewMenu->addAction(homeAct);
  viewMenu->addSeparator();

  windowMenu = menuBar()->addMenu(tr("&Window"));
  updateWindowMenu();
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void STLViewer::createToolBars() {
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  viewToolBar = addToolBar(tr("View"));
  viewToolBar->addAction(rotateAct);
  viewToolBar->addAction(translateAct);
  viewToolBar->addAction(zoomAct);
  viewToolBar->addAction(homeAct);
}

void STLViewer::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}

void STLViewer::readSettings() {
  QSettings settings("Cravesoft", "STLViewer");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
}

void STLViewer::writeSettings() {
  QSettings settings("Cravesoft", "STLViewer");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

GLMdiChild *STLViewer::activeGLMdiChild() {
  if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    return qobject_cast<GLMdiChild *>(activeSubWindow->widget());
  return 0;
}

QMdiSubWindow *STLViewer::findGLMdiChild(const QString &fileName) {
  QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

  foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
    GLMdiChild *glMdiChild = qobject_cast<GLMdiChild *>(window->widget());
    if (glMdiChild->currentFile() == canonicalFilePath)
      return window;
  }
  return 0;
}

void STLViewer::setActiveSubWindow(QWidget *window) {
  if (!window)
    return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void STLViewer::createDockWindows() {
  QDockWidget *dock = new QDockWidget(tr("Informations"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  dimensionsGroupBox = new DimensionsGroupBox(this);
  meshInformationGroupBox = new MeshInformationGroupBox(this);
  propertiesGroupBox = new PropertiesGroupBox(this);

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

