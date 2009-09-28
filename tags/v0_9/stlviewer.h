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

#ifndef STLVIEWER_H
#define STLVIEWER_H

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>

#include "entity.h"

class Entity;
class QAction;
class QMenu;
class GLWidget;
class QGroupBox;
class QLabel;

class STLViewer : public QMainWindow {

  Q_OBJECT
 
 public:
  STLViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
  ~STLViewer();

 protected:
  void closeEvent(QCloseEvent *event);

 private slots:
  void newFile();  
  void open();
  bool save();
  bool saveAs();
  void rotate();
  void translate();
  void zoom();
  void home();

  void about();

 private:

  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();
  void createDimensionsGroupBox();
  void createMeshInformationGroupBox();
  void createPropertiesGroupBox();
  void readSettings();
  void writeSettings();
  void loadFile(const QString &fileName);
  bool saveFile(const QString &fileName);
  void setCurrentFile(const QString &fileName);
  QString strippedName(const QString &fullFileName);

  QString curFile;
  Entity *entity_;
  GLWidget *glWidget;
  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *viewToolBar;
  QAction *newAct;
  QAction *openAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *zoomAct;
  QAction *rotateAct;
  QAction *translateAct;
  QAction *homeAct;
  QAction *viewAct;
  QAction *exitAct;
  QAction *aboutAct;

  QLabel *xMax, *xMin, *xDelta;
  QLabel *yMax, *yMin, *yDelta;
  QLabel *zMax, *zMin, *zDelta;
  QLabel *num_facets, *num_points;
  QLabel *volume, *surface;

  QGroupBox *dimensionsGroupBox;
  QGroupBox *meshInformationGroupBox;
  QGroupBox *propertiesGroupBox;
};

#endif // STLVIEWER_H