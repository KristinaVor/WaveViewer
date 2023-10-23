#include "waveviewerwindow.h"

WaveViewerWindow::WaveViewerWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(800, 600);
    this->move(QGuiApplication::primaryScreen()->geometry().center() - this->rect().center());

    // Инициализация QCustomPlot
    plot = new QCustomPlot(this);
    plot->addGraph();
    plot->xAxis->setLabel("Порядковый номер");
    plot->yAxis->setLabel("Значение сэмпла");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plot->resize(700, 500);

    // Инициализация меток для отображения максимального значения и медианы
    initMaxValueAndMedianLabels();

    // Размещение виджетов на главном окне
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(plot);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Инициализация UDP сокета
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 10002); // Устанавливаем IP-адрес и порт для приема данных
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

WaveViewerWindow::~WaveViewerWindow()
{
    // Деструктор окна просмотра волн здесь
}

void WaveViewerWindow::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        // Обрабатываем полученные данные
        processData(datagram);
    }
}

void WaveViewerWindow::processData(const QByteArray &data)
{
    QVector<short> samples;

    // Преобразование данных из QByteArray в QVector<short>
    int sampleCount = data.size() / sizeof(short);
    samples.resize(sampleCount);
    memcpy(samples.data(), data.constData(), data.size());

    // Преобразование samples в QVector<double>
    QVector<double> samplesAsDouble;
    samplesAsDouble.reserve(samples.size());
    for (short value : samples)
    {
        samplesAsDouble.append(static_cast<double>(value));
    }

    // Обновление графика
    waveData.clear();
    waveData.append(samplesAsDouble);

    // Очистите данные графика перед установкой новых данных
    plot->graph(0)->data()->clear();
    for (int i = 0; i < samplesAsDouble.size(); ++i)
    {
        plot->graph(0)->addData(i, samplesAsDouble.at(i));
    }
    plot->rescaleAxes();
    plot->replot();

    double m_maxValue = calculateMaximumValue(samples);
    double m_median = calculateMedian(samples);

    // Обновляем максимальное значение и медиану
    maxValue->setGraphKey(m_maxValue);  // Обновляем координаты максимального значения
    median->start->setCoords(0, m_median);// Обновляем координаты медианы
    median->end->setCoords(samples.size() - 1, m_median);

    plot->replot();
}


double WaveViewerWindow::calculateMaximumValue(const QVector<short> &data)
{
    if (data.isEmpty())
        return 0.0;

    return *std::max_element(data.begin(), data.end());
}

double WaveViewerWindow::calculateMedian(const QVector<short> &data)
{
    if (data.isEmpty())
        return 0.0;

    QVector<short> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());
    int size = sortedData.size();

    if (size % 2 == 0)
        return (sortedData[size / 2 - 1] + sortedData[size / 2]) / 2.0;
    else
        return sortedData[size / 2];
}

void WaveViewerWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    plot->setGeometry(0, 0, width(), height());
}

void WaveViewerWindow::initMaxValueAndMedianLabels()
{
    // Инициализация максимального значения
    maxValue = new QCPItemTracer(plot);
    maxValue->setGraph(plot->graph(0));
    maxValue->setGraphKey(0);
    maxValue->setInterpolating(true);
    maxValue->setStyle(QCPItemTracer::tsCircle);
    maxValue->setSize(10);
    maxValue->setPen(QPen(Qt::red));

    // Инициализация медианы
    median = new QCPItemLine(plot);
    median->start->setType(QCPItemPosition::ptPlotCoords);
    median->end->setType(QCPItemPosition::ptPlotCoords);
    QPen medianPen;
    medianPen.setColor(Qt::blue);
    medianPen.setStyle(Qt::DashDotLine); // Используем стиль прерывисто-точечной линии
    medianPen.setWidth(2); // Устанавливаем ширину линии
    median->setPen(medianPen);
}



