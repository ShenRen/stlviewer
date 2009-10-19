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
  // Read persistent application settings
  readSettings();
  // Deactivate the left mouse button when manipulating objects
  leftMouseButtonMode = GLWidget::INACTIVE;
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
  // Reset all informations
  dimensionsGroupBox->reset();
  meshInformationGroupBox->reset();
  propertiesGroupBox->reset();
}

void STLViewer::open() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a file"),
    curDir, tr("STL Files (*.stl);;All Files (*.*)"));
  if (!fileName.isEmpty()) {
    curDir = QFileInfo(fileName).filePath();
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

void STLViewer::saveImage() {
  if (activeGLMdiChild() && activeGLMdiChild()->saveImage())
    statusBar()->showMessage(tr("Image saved"), 2000);
}

void STLViewer::rotate() {
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());
  if (rotateAct->isChecked()) {
    rotateAct->setChecked(true);
    panningAct->setChecked(false);
    leftMouseButtonMode = GLWidget::ROTATE;
  } else {
    rotateAct->setChecked(false);
    leftMouseButtonMode = GLWidget::INACTIVE;
  }
  emit leftMouseButtonModeChanged(leftMouseButtonMode);
}

void STLViewer::panning() {
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());
  if (panningAct->isChecked()) {
    panningAct->setChecked(true);
    rotateAct->setChecked(false);
    leftMouseButtonMode = GLWidget::PANNING;
  } else {
    panningAct->setChecked(false);
    leftMouseButtonMode = GLWidget::INACTIVE;
  }
  emit leftMouseButtonModeChanged(leftMouseButtonMode);
}

void STLViewer::zoom() {}

void STLViewer::unzoom() {
  activeGLMdiChild()->unzoom();
}

void STLViewer::backView() {
  activeGLMdiChild()->setBackView();
}

void STLViewer::frontView() {
  activeGLMdiChild()->setFrontView();
}

void STLViewer::leftView() {
  activeGLMdiChild()->setLeftView();
}

void STLViewer::rightView() {
  activeGLMdiChild()->setRightView();
}

void STLViewer::topView() {
  activeGLMdiChild()->setTopView();
}

void STLViewer::bottomView() {
  activeGLMdiChild()->setBottomView();
}

void STLViewer::topFrontLeftView() {
  activeGLMdiChild()->setTopFrontLeftView();
}

void STLViewer::about() {
  QMessageBox::about(this, tr("About STLViewer"),
    tr("<a href=\"http://www.cravesoft.com\">www.cravesoft.com</b>"));
}

void STLViewer::updateMenus() {
  bool hasGLMdiChild = (activeGLMdiChild() != 0);
  saveAct->setEnabled(hasGLMdiChild);
  saveAsAct->setEnabled(hasGLMdiChild);
  saveImageAct->setEnabled(hasGLMdiChild);
  closeAct->setEnabled(hasGLMdiChild);
  closeAllAct->setEnabled(hasGLMdiChild);
  zoomAct->setEnabled(hasGLMdiChild);
  rotateAct->setEnabled(hasGLMdiChild);
  panningAct->setEnabled(hasGLMdiChild);
  unzoomAct->setEnabled(hasGLMdiChild);
  backViewAct->setEnabled(hasGLMdiChild);
  frontViewAct->setEnabled(hasGLMdiChild);
  leftViewAct->setEnabled(hasGLMdiChild);
  rightViewAct->setEnabled(hasGLMdiChild);
  topViewAct->setEnabled(hasGLMdiChild);
  bottomViewAct->setEnabled(hasGLMdiChild);
  topFrontLeftViewAct->setEnabled(hasGLMdiChild);
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

void STLViewer::setMousePressed(Qt::MouseButtons button) {
  if (button & Qt::RightButton) {
    rotateAct->setChecked(true);
  } else if (button & Qt::MidButton) {
    panningAct->setChecked(true);
  }
}

void STLViewer::setMouseReleased(Qt::MouseButtons button) {
  if (button & Qt::RightButton) {
    if (leftMouseButtonMode != GLWidget::ROTATE)
      rotateAct->setChecked(false);
  } else if (button & Qt::MidButton) {
    if (leftMouseButtonMode != GLWidget::PANNING)
      panningAct->setChecked(false);
  }
}

GLMdiChild *STLViewer::createGLMdiChild() {
  GLMdiChild *child = new GLMdiChild;
  mdiArea->addSubWindow(child);
  child->setLeftMouseButtonMode(leftMouseButtonMode);
  connect(child, SIGNAL(mouseButtonPressed(Qt::MouseButtons)), this,
    SLOT(setMousePressed(Qt::MouseButtons)));
  connect(child, SIGNAL(mouseButtonReleased(Qt::MouseButtons)), this,
    SLOT(setMouseReleased(Qt::MouseButtons)));
  connect(this,
    SIGNAL(leftMouseButtonModeChanged(GLWidget::LeftMouseButtonMode)), child,
    SLOT(setLeftMouseButtonMode(GLWidget::LeftMouseButtonMode)));
  connect(child, SIGNAL(destroyed()), this, SLOT(destroyGLMdiChild()));
  return child;
}

void STLViewer::setActiveSubWindow(QWidget *window) {
  if (!window)
    return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void STLViewer::destroyGLMdiChild() {
  if (activeGLMdiChild() == 0) {
    panningAct->setChecked(false);
    rotateAct->setChecked(false);
    leftMouseButtonMode = GLWidget::INACTIVE;
    emit leftMouseButtonModeChanged(leftMouseButtonMode);
  }
}

void STLViewer::createActions() {
  newAct = new QAction(QIcon(":STLViewer/Images/page_white.png"), tr("&New"),
    this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":STLViewer/Images/folder.png"), tr("&Open..."),
    this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":STLViewer/Images/disk.png"), tr("&Save"),
    this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("Save &As..."), this);
  saveAsAct->setShortcut(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  saveImageAct = new QAction(tr("Save Image..."), this);
  saveImageAct->setShortcut(tr("Ctrl+I"));
  saveImageAct->setStatusTip(tr("Save the current view to disk"));
  connect(saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));

  closeAct = new QAction(tr("Cl&ose"), this);
  closeAct->setShortcut(tr("Ctrl+W"));
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

  rotateAct = new QAction(QIcon(":STLViewer/Images/arrow_rotate_clockwise.png"),
    tr("&Rotate"), this);
  rotateAct->setShortcut(tr("R"));
  rotateAct->setStatusTip(tr("Rotate the object"));
  rotateAct->setCheckable(true);
  connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));
  rotateAct->setChecked(false);
    
  panningAct = new QAction(QIcon(":STLViewer/Images/arrow_out.png"), tr("&Pan"),
    this);
  panningAct->setShortcut(tr("P"));
  panningAct->setStatusTip(tr("Drag the object around"));
  panningAct->setCheckable(true);
  connect(panningAct, SIGNAL(triggered()), this, SLOT(panning()));
  panningAct->setChecked(false);

  zoomAct = new QAction(QIcon(":STLViewer/Images/magnifier_zoom_in.png"),
    tr("&Zoom In"), this);
  zoomAct->setShortcut(tr("Z"));
  zoomAct->setStatusTip(tr("Zoom in"));
  connect(zoomAct, SIGNAL(triggered()), this, SLOT(zoom()));

  unzoomAct =
    new QAction(QIcon(":STLViewer/Images/page_white_magnify.png"),
      tr("&Unzoom"), this);
  unzoomAct->setShortcut(tr("U"));
  unzoomAct->setStatusTip(tr("Unzoom"));
  connect(unzoomAct, SIGNAL(triggered()), this, SLOT(unzoom()));

  backViewAct =
    new QAction(QIcon(":STLViewer/Images/back_view.png"),
      tr("&Back View"), this);
  backViewAct->setStatusTip(tr("Back view"));
  connect(backViewAct, SIGNAL(triggered()), this, SLOT(backView()));

  frontViewAct =
    new QAction(QIcon(":STLViewer/Images/front_view.png"),
      tr("&Front View"), this);
  frontViewAct->setStatusTip(tr("Front view"));
  connect(frontViewAct, SIGNAL(triggered()), this, SLOT(frontView()));

  leftViewAct =
    new QAction(QIcon(":STLViewer/Images/left_view.png"),
      tr("&Left View"), this);
  leftViewAct->setStatusTip(tr("Left view"));
  connect(leftViewAct, SIGNAL(triggered()), this, SLOT(leftView()));

  rightViewAct =
    new QAction(QIcon(":STLViewer/Images/right_view.png"),
      tr("&Right View"), this);
  rightViewAct->setStatusTip(tr("Right view"));
  connect(rightViewAct, SIGNAL(triggered()), this, SLOT(rightView()));

  topViewAct =
    new QAction(QIcon(":STLViewer/Images/top_view.png"),
      tr("&Top View"), this);
  topViewAct->setStatusTip(tr("Top view"));
  connect(topViewAct, SIGNAL(triggered()), this, SLOT(topView()));

  bottomViewAct =
    new QAction(QIcon(":STLViewer/Images/bottom_view.png"),
      tr("&Bottom View"), this);
  bottomViewAct->setStatusTip(tr("Bottom view"));
  connect(bottomViewAct, SIGNAL(triggered()), this, SLOT(bottomView()));

  topFrontLeftViewAct =
    new QAction(QIcon(":STLViewer/Images/top_front_left_view.png"),
      tr("&Top Front Left View"), this);
  topFrontLeftViewAct->setStatusTip(tr("Top Front Left view"));
  connect(topFrontLeftViewAct, SIGNAL(triggered()), this, SLOT(topFrontLeftView()));

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
  fileMenu->addAction(saveImageAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(rotateAct);
  viewMenu->addAction(panningAct);
  viewMenu->addAction(zoomAct);
  viewMenu->addAction(unzoomAct);

  defaultViewsMenu = viewMenu->addMenu(tr("&Default Views"));
  defaultViewsMenu->addAction(backViewAct);
  defaultViewsMenu->addAction(frontViewAct);
  defaultViewsMenu->addAction(leftViewAct);
  defaultViewsMenu->addAction(rightViewAct);
  defaultViewsMenu->addAction(topViewAct);
  defaultViewsMenu->addAction(bottomViewAct);
  defaultViewsMenu->addAction(topFrontLeftViewAct);

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
  viewToolBar->addAction(panningAct);
  viewToolBar->addAction(zoomAct);
  viewToolBar->addAction(unzoomAct);
  viewToolBar->addAction(backViewAct);
  viewToolBar->addAction(frontViewAct);
  viewToolBar->addAction(leftViewAct);
  viewToolBar->addAction(rightViewAct);
  viewToolBar->addAction(topViewAct);
  viewToolBar->addAction(bottomViewAct);
  viewToolBar->addAction(topFrontLeftViewAct);
}

void STLViewer::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}

void STLViewer::createDockWindows() {
  // Create a DockWidget named "Informations"
  QDockWidget *dock = new QDockWidget(tr("Informations"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  // Create one GroupBox for each type of data
  dimensionsGroupBox = new DimensionsGroupBox(this);
  meshInformationGroupBox = new MeshInformationGroupBox(this);
  propertiesGroupBox = new PropertiesGroupBox(this);
  // Create a layout inside a widget to display all GroupBoxes in one layout
  QWidget *wi = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(dimensionsGroupBox);
  layout->addWidget(meshInformationGroupBox);
  layout->addWidget(propertiesGroupBox);
  wi->setLayout(layout);
  wi->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                QSizePolicy::Fixed));
  // Embed the widget that contains all GroupBoxes into the DockWidget
  dock->setWidget(wi);
  // Add the DockWidget at the right side of the main layout
  addDockWidget(Qt::RightDockWidgetArea, dock);
  // Add a button in the view menu to show/hide the DockWidget
  viewMenu->addAction(dock->toggleViewAction());
}

void STLViewer::readSettings() {
  QSettings settings("Cravesoft", "STLViewer");
  curDir = settings.value("dir", QString()).toString();
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
}

void STLViewer::writeSettings() {
  QSettings settings("Cravesoft", "STLViewer");
  settings.setValue("dir", curDir);
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

