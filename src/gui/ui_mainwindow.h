/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Sun Dec 13 23:37:33 2009
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionSavePreset;
    QAction *actionLoadPreset;
    QAction *actionMIDILearn;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSynth;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(600, 400);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionSavePreset = new QAction(MainWindow);
        actionSavePreset->setObjectName(QString::fromUtf8("actionSavePreset"));
        actionLoadPreset = new QAction(MainWindow);
        actionLoadPreset->setObjectName(QString::fromUtf8("actionLoadPreset"));
        actionMIDILearn = new QAction(MainWindow);
        actionMIDILearn->setObjectName(QString::fromUtf8("actionMIDILearn"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuSynth = new QMenu(menuBar);
        menuSynth->setObjectName(QString::fromUtf8("menuSynth"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSynth->menuAction());
        menuFile->addAction(actionQuit);
        menuSynth->addAction(actionSavePreset);
        menuSynth->addAction(actionLoadPreset);
        menuSynth->addSeparator();
        menuSynth->addAction(actionMIDILearn);

        retranslateUi(MainWindow);
        QObject::connect(actionQuit, SIGNAL(triggered()), MainWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Q", 0, QApplication::UnicodeUTF8));
        actionSavePreset->setText(QApplication::translate("MainWindow", "Save preset...", 0, QApplication::UnicodeUTF8));
        actionLoadPreset->setText(QApplication::translate("MainWindow", "Load preset...", 0, QApplication::UnicodeUTF8));
        actionMIDILearn->setText(QApplication::translate("MainWindow", "MIDI Learn", 0, QApplication::UnicodeUTF8));
        actionMIDILearn->setShortcut(QApplication::translate("MainWindow", "L", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuSynth->setTitle(QApplication::translate("MainWindow", "Synth", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
