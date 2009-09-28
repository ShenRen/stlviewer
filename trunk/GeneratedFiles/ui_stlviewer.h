/********************************************************************************
** Form generated from reading ui file 'stlviewer.ui'
**
** Created: Mon 28. Sep 15:27:26 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_STLVIEWER_H
#define UI_STLVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_STLViewerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *STLViewerClass)
    {
        if (STLViewerClass->objectName().isEmpty())
            STLViewerClass->setObjectName(QString::fromUtf8("STLViewerClass"));
        STLViewerClass->resize(600, 400);
        menuBar = new QMenuBar(STLViewerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        STLViewerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(STLViewerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        STLViewerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(STLViewerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        STLViewerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(STLViewerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        STLViewerClass->setStatusBar(statusBar);

        retranslateUi(STLViewerClass);

        QMetaObject::connectSlotsByName(STLViewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *STLViewerClass)
    {
        STLViewerClass->setWindowTitle(QApplication::translate("STLViewerClass", "STLViewer", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(STLViewerClass);
    } // retranslateUi

};

namespace Ui {
    class STLViewerClass: public Ui_STLViewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STLVIEWER_H
