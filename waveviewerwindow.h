#ifndef WAVEVIEWERWINDOW_H
#define WAVEVIEWERWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QUdpSocket>

#include "qcustomplot.h"

class WaveViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
    WaveViewerWindow(QWidget *parent = nullptr);
    ~WaveViewerWindow();

    void resizeEvent(QResizeEvent *event);

private slots:
    void processData(const QByteArray &data); // Слот для обработки данных по сетевому протоколу

private:
    double calculateMaximumValue(const QVector<short> &data); // Метод для вычисления максимального значения
    double calculateMedian(const QVector<short> &data); // Метод для вычисления медианы


    QCustomPlot *plot; // График для отображения кривой
    QVector<double> waveData; // Данные кривой
    QLabel *maxValueLabel; // Метка для отображения максимального значения
    QLabel *medianLabel; // Метка для отображения медианы
};

#endif // WAVEVIEWERWINDOW_H
