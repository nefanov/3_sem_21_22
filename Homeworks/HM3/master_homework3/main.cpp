#include <programs_starter.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

#define STEPEN 1000000


QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    double* times = executePrograms();

    if(times == NULL)
    {
        perror("Something went wrong with execution");
        return -1;
    }

    FILE* file1 = fopen(SMALL_LOGFILE_NAME, "w");
    fclose(file1);
    file1 = fopen(MEDIUM_LOGFILE_NAME, "w");
    fclose(file1);
    file1 = fopen(BIG_LOGFILE_NAME, "w");
    fclose(file1);

    QBarSet* fifoSet = new QBarSet("FIFO");
    QBarSet* msgqSet = new QBarSet("MSG_Q");
    QBarSet* smSet = new QBarSet("SH_M");

    (*fifoSet) << (qreal)(times[0] * STEPEN) << (qreal)(times[3] * STEPEN) << (qreal)(times[6] * STEPEN);
    (*msgqSet) << (qreal)(times[1] * STEPEN) << (qreal)(times[4] * STEPEN) << (qreal)(times[7] * STEPEN);
    (*smSet) << (qreal)(times[2] * STEPEN) << (qreal)(times[5] * STEPEN) << (qreal)(times[8] * STEPEN);

    QBarSeries* series = new QBarSeries();
    series->append(fifoSet);
    series->append(msgqSet);
    series->append(smSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Time distrubtion for diffrent IPCs");
    //chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Small File" << "Medium File" << "Big File";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, times[7] * STEPEN + 3);
    axisY->setTickCount(20);
    axisY->setTitleText("Seconds * 10^(-6)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(420, 300);
    window.show();

    delete [] times;

    return a.exec();
}

int main2(int argc, char *argv[])
{
    pid_t pid1 = fork();
    pid_t pid2;
    if(pid1)
        pid2 = fork();

    if(pid1 == 0)
    {
        int parStatus = execlp("./msgq_server", "./msgq_server", "./big_size_logfile.txt", (char*)NULL);

        exit(2);
    }
    else if (pid2 == 0) {
        //sleep(0.5);

        int chStatus = execlp("./msgq_user", "./msgq_user", "./big_size_file.txt", (char*)NULL);

        exit(2);
    }
    else {
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }

    char buf[1];
    double nextTime;

        int fd = open("big_size_logfile.txt", O_RDWR);

        if(fd < 0)
        {
            perror("Can't open file");
            return NULL;
        }

       int bytesRead = read(fd, &nextTime, 8);

       if(bytesRead < 8)
       {
           perror("Can't read file");
           return NULL;
       }

        close(fd);

    return 0;
}

