#include "mainwindow.h"

#include "waveviewerwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *toolsMenu = menuBar()->addMenu("Tools");
    QAction *waveViewerAction = toolsMenu->addAction("Wave Viewer");
    QAction *aboutAction = toolsMenu->addAction("About");

    connect(waveViewerAction, &QAction::triggered, this, &MainWindow::openWaveViewerWindow);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::openAboutDialog);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openWaveViewerWindow()
{
    WaveViewerWindow *waveViewer = new WaveViewerWindow(this);
    waveViewer->show();
}

void MainWindow::openAboutDialog()
{
    AboutDialog *aboutDialog = new AboutDialog(this);
    aboutDialog->show();
}

