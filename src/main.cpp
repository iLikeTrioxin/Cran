#include "MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <iostream>

#include <thread>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "CranelCamera.h"
#include "TrackingDevice.h"
#include "CameraTracker.h"
#include "MyBracelet.h"
#include "KinectCamera.h"
#include "OpencvCamera.h"
#include <string>

#include <QThread>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString &locale : uiLanguages) {
        const QString baseName = "Cranel_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
//    std::thread t(test);


    MainWindow* w = new MainWindow(nullptr);
    QThread* thread = new QThread();
    Worker* worker = new Worker(w);
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::started, worker, &Worker::process);
    thread->start();
    a.exec();
    thread->quit();
    thread->wait();



//    wypierdalaj();
//    t.join();
    return 0;
}
