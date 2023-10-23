#ifndef WAVEVIEWERWINDOW_H
#define WAVEVIEWERWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
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
    void readPendingDatagrams(); // Слот для чтения данных по сетевому протоколу

private:
    double calculateMaximumValue(const QVector<short> &data); // Метод для вычисления максимального значения
    double calculateMedian(const QVector<short> &data); // Метод для вычисления медианы
    void initMaxValueAndMedianLabels();


    QCustomPlot *plot; // График для отображения кривой
    QUdpSocket *udpSocket; //
    QVector<double> waveData; // Данные кривой
    QCPItemTracer *maxValue; // Метка для отображения максимального значения
    QCPItemLine *median; // Метка для отображения медианы
};

#endif // WAVEVIEWERWINDOW_H
