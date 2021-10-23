#define _GNU_SOURCE

#include <QApplication>
#include <QLabel>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <sched.h>   //cpu_set_t , CPU_SET
#include <pthread.h> //pthread_t
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define THREAD_NUMBER_LIMIT 500
#define THREAD_NUM_INRERVAL 1
#define START_NUM_THREADS 1

QT_USE_NAMESPACE

long const numOfPoints = 1000000;

double const squareArea = 8;        //our square area, where graph is located
double const trueArea = 8 / 3;

typedef struct threadParametrs{
    long* aboveOrUnderCurve;
    int threadNumber;
    int numOfThreads;
    short (*curveFunc)(double, double);
}thPar;

short ParaboleCurve(double x, double y)             //parabole from 0 to 2
{
    return x * x < y;
}

void* threadfunc2(void* arg)
{
    thPar* myParametrs = (thPar*)(arg);

    long numOfCores = sysconf(_SC_NPROCESSORS_ONLN);
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET((myParametrs->threadNumber % numOfCores) + 1, &cpuSet);
    sched_setaffinity(0, sizeof(cpuSet), &cpuSet);

    long curThreadNumOfPoints = numOfPoints / myParametrs->numOfThreads;
    unsigned int seed = time(NULL) + (unsigned int)(pthread_self());

    for (long i = 0; i < curThreadNumOfPoints; ++i) {

        double xZel = rand_r(&seed) % 2;
        double xDrob = rand_r(&seed) % 1000;
        xDrob /= 1000;

        double yZel = rand_r(&seed) % 2;
        double yDrob = rand_r(&seed) % 1000;
        yDrob /= 1000;

        if(myParametrs->curveFunc(xZel + xDrob, yZel + yDrob))              //above curve
        {
            myParametrs->aboveOrUnderCurve[myParametrs->threadNumber]++;
        }
        else {                                                              //under curve
            myParametrs->aboveOrUnderCurve[myParametrs->threadNumber]--;
        }
    }

    return NULL;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    double averageDiffrence = 0;                        //using to calculate avarage error of monte-carlo method
    long numOfThreads = START_NUM_THREADS;              //initial number of threads
    double timeOfExecutions[((THREAD_NUMBER_LIMIT - numOfThreads) / THREAD_NUM_INRERVAL) + 1];
    long iterations = 0;

    for ( ; numOfThreads < THREAD_NUMBER_LIMIT; iterations++, numOfThreads += THREAD_NUM_INRERVAL) {

        long* aboveOrUnderArray = (long*)(calloc(numOfThreads, sizeof (long)));
        thPar* argsArray = (thPar*)(calloc(numOfThreads, sizeof (thPar)));
        pthread_t* threadsArray = (pthread_t*)(calloc(numOfThreads, sizeof (pthread_t)));

        clock_t time1 = clock();

        for (long i = 0; i < numOfThreads; ++i) {
            argsArray[i].aboveOrUnderCurve = aboveOrUnderArray;
            argsArray[i].numOfThreads = numOfThreads;
            argsArray[i].threadNumber = i;
            argsArray[i].curveFunc = ParaboleCurve;

            int stat = pthread_create(&threadsArray[i], NULL, threadfunc2, (void*)(argsArray+i));

            if(stat < 0)
            {
                perror("Thread creation failed");
                return -1;
            }
        }



        long sum = 0;
        for (long i = 0; i < numOfThreads; ++i) {
            pthread_join(threadsArray[i], NULL);

            sum += aboveOrUnderArray[i];
        }

        clock_t time2 = clock();

        timeOfExecutions[iterations] = (double)(((double)(time2 - time1) / CLOCKS_PER_SEC));

        double aboveCurve = (numOfPoints + sum) / 2;
        double underCurve = numOfPoints - aboveCurve;

        double grathsArea = (underCurve / numOfPoints) * squareArea;
        averageDiffrence += abs((trueArea - grathsArea));


        free(aboveOrUnderArray);
        free(argsArray);
        free(threadsArray);
    }

    averageDiffrence /= iterations;

    // creating graph here

    QLineSeries *series = new QLineSeries();

    for (int i = 0; i < iterations; ++i) {
        series->append(START_NUM_THREADS + THREAD_NUM_INRERVAL*i, timeOfExecutions[i] * 1000);
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);

    chart->createDefaultAxes();
    chart->axisX()->setTitleText("Num of threads");
    chart->axisX()->setGridLineVisible(true);
    chart->axisY()->setTitleText("Time, mSec");
    chart->axisY()->setGridLineVisible(true);


    chart->setTitle("Threads graph");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(400, 300);
    window.show();

    QLabel lab("Average Error = " + QString::number(averageDiffrence));
    lab.resize(200, 75);
    lab.show();

    return a.exec();
}

