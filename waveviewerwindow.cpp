#include "waveviewerwindow.h"

WaveViewerWindow::WaveViewerWindow(QWidget *parent)
    : QMainWindow(parent),
      plot(nullptr),
      udpSocket(nullptr),
      maxValue(nullptr),
      median(nullptr),
      maxValueLabel(nullptr),
      medianLabel(nullptr)
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

    // Инициализация меток для отображения максимального значения и медианы
    initMaxValueAndMedianLabels();
}

WaveViewerWindow::~WaveViewerWindow()
{
    if (udpSocket)
        delete udpSocket;

    if (plot)
        delete plot;
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

    // Очистим данные графика перед установкой новых данных
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
    maxValue->setGraphKey(samplesAsDouble.indexOf(m_maxValue)); // Ищем координату максимального значения
    median->start->setCoords(0, m_median); // Обновляем координаты медианы
    median->end->setCoords(samplesAsDouble.size() - 1, m_median);

    // Обновляем текстовые метки
    updateMaxValueLabel(samplesAsDouble.indexOf(m_maxValue), m_maxValue);
    updateMedianLabel(m_median);

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

    // Инициализация метки для максимального значения
    maxValueLabel = new QCPItemText(plot);
    maxValueLabel->setPositionAlignment(Qt::AlignBottom| Qt::AlignRight);
    maxValueLabel->position->setType(QCPItemPosition::ptPlotCoords);
    maxValueLabel->position->setCoords(0, 0); // Координаты максимального значения
    maxValueLabel->setPen(QPen(Qt::black)); // Установите цвет текста

    // Инициализация медианы
    median = new QCPItemLine(plot);
    median->start->setType(QCPItemPosition::ptPlotCoords);
    median->end->setType(QCPItemPosition::ptPlotCoords);
    QPen medianPen;
    medianPen.setColor(Qt::blue);
    medianPen.setStyle(Qt::DashDotLine);
    medianPen.setWidth(2);
    median->setPen(medianPen);

    // Инициализация метки для медианы
    medianLabel = new QCPItemText(plot);
    medianLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    medianLabel->position->setType(QCPItemPosition::ptPlotCoords);
    medianLabel->position->setCoords(0, 0); // Координаты медианы
    medianLabel->setPen(QPen(Qt::black)); // Установите цвет текста
}

void WaveViewerWindow::updateMaxValueLabel(int x, double value)
{
    QString label = QString("Max Value\nx: %1\ny: %2").arg(x).arg(value);
    double maxYValue = plot->yAxis->range().upper;
    double labelYPosition = maxYValue * 1;
    maxValueLabel->position->setCoords(x, labelYPosition);
    maxValueLabel->setText(label);
}

void WaveViewerWindow::updateMedianLabel(double value)
{
    QString label = QString("Median: %1").arg(value);
    double labelYPosition = value + 10;
    double medianX = plot->xAxis->range().center();
    medianLabel->position->setCoords(medianX, labelYPosition);
    medianLabel->setText(label);
}

