#ifndef WAVEVIEWERWINDOW_H
#define WAVEVIEWERWINDOW_H

#include <QMainWindow>

class WaveViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
    WaveViewerWindow(QWidget *parent = nullptr);
    ~WaveViewerWindow();
};

#endif // WAVEVIEWERWINDOW_H
