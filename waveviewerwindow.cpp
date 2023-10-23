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
    maxValueLabel = new QLabel(this);
    medianLabel = new QLabel(this);

    // Размещение виджетов на главном окне
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(plot);
    mainLayout->addWidget(maxValueLabel);
    mainLayout->addWidget(medianLabel);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

}

WaveViewerWindow::~WaveViewerWindow()
{
    // Деструктор окна просмотра волн здесь
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

    // Вычисление и обновление максимального значения и медианы
    double maxValue = calculateMaximumValue(samples);
    double median = calculateMedian(samples);

    maxValueLabel->setText("Максимальное значение: " + QString::number(maxValue));
    medianLabel->setText("Медиана: " + QString::number(median));
}

double WaveViewerWindow::calculateMaximumValue(const QVector<short> &data)
{
    if (data.isEmpty()) {
        return 0.0;
    }

    return *std::max_element(data.begin(), data.end());
}

double WaveViewerWindow::calculateMedian(const QVector<short> &data)
{
    if (data.isEmpty()) {
        return 0.0;
    }

    QVector<short> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());
    int size = sortedData.size();

    if (size % 2 == 0) {
        return (sortedData[size / 2 - 1] + sortedData[size / 2]) / 2.0;
    } else {
        return sortedData[size / 2];
    }
}

void WaveViewerWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    plot->setGeometry(0, 0, width(), height());
}
