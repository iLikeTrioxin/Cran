#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CameraTrackerWindow.h"

#include <QPushButton>
#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QCameraInfo>
#include <iostream>
#include "OpencvCamera.h"
#include "KinectCamera.h"
#include <QComboBox>
#include <QtCore>
#include <opencv2/opencv.hpp>

std::vector<std::pair<QString, QString>> camerasAvailable;
std::unordered_map<std::string, std::string> camerasUsed;
#include <QtGui>

void MainWindow::closeTracker(CameraTrackerWindow* window){
    std::cout<<"destroyed\n";
    for(int i = 0; i < cameraTrackerWindows.size(); i++){
        if(cameraTrackerWindows[i].second->window == (QMainWindow*)window){
            std::string cameraName = window->tracker->camera->name;
            std::string cameraDesc = camerasUsed.at(cameraName);
            std::string cameraLabel = cameraName + " - " + cameraDesc;
            cameraTrackerWindows[i].first->quit();
            cameraTrackerWindows[i].first->wait();
            delete cameraTrackerWindows[i].first;
            delete cameraTrackerWindows[i].second;
            cameraTrackerWindows.erase(cameraTrackerWindows.begin() + i);
            multiCameraTracker.removeTracker(i);
            ui->cameraAddSelect->addItem(cameraLabel.c_str());
            camerasAvailable.push_back({cameraName.c_str(), cameraDesc.c_str()});
            camerasUsed.erase(cameraName);
            onCurrentIndexChanged(0);
            break;
        }
    }
}

void MainWindow::addCamera(){
    std::cout<<"added camera\n";

    Cranel::CameraTracker* tracker = new Cranel::CameraTracker(previewCam);
    tracker = multiCameraTracker.addTracker(tracker);

    QThread* thread = new QThread();
    Worker* worker = new Worker(new CameraTrackerWindow(this, tracker), this);

    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::started, worker, &Worker::process);
    cameraTrackerWindows.emplace_back(thread, worker);
    connect((CameraTrackerWindow*)cameraTrackerWindows.back().second->window, &CameraTrackerWindow::destroyedd, this, &MainWindow::closeTracker);
    connect(this, &MainWindow::update, (CameraTrackerWindow*)cameraTrackerWindows.back().second->window, &CameraTrackerWindow::update);
    thread->start();

    camerasUsed[tracker->camera->name] = camerasAvailable[ui->cameraAddSelect->currentIndex()].second.toStdString();
    camerasAvailable.erase(camerasAvailable.begin() + ui->cameraAddSelect->currentIndex());
    ui->cameraAddSelect->removeItem(ui->cameraAddSelect->currentIndex());
}

void MainWindow::onCurrentIndexChanged(int index){
    delete previewCam;

    if(index == -1) return;

    auto camera = camerasAvailable[index];

    if(camera.second.toLower().contains("xbox"))
        previewCam = new Cranel::KinectCamera(camera.first.toStdString());
    else
        previewCam = new Cranel::OpenCVCamera(camera.first.toStdString());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), _timer(this), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QList<QCameraInfo> camerasList = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, camerasList) {
            camerasAvailable.push_back({cameraInfo.deviceName(), cameraInfo.description()});
        ui->cameraAddSelect->addItem(cameraInfo.deviceName() + " - " + cameraInfo.description());
    }
//    onCurrentIndexChanged(0);

    // populate your window with images, labels, etc. here
    connect(ui->cameraAddSelect, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentIndexChanged);
    connect(ui->cameraAddSubmit, &QPushButton::pressed, this, &MainWindow::addCamera);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(on_timeout()));


    _timer.start(10 /*call the timer every 10 ms*/);
}

MainWindow::~MainWindow() {
    for(auto& cameraTrackerWindow : cameraTrackerWindows){
        closeTracker((CameraTrackerWindow*)cameraTrackerWindow.second->window);
        delete cameraTrackerWindow.first;
        delete cameraTrackerWindow.second;
    }
    cameraTrackerWindows.clear();
    delete ui;
    delete previewCam;
}

void MainWindow::on_timeout() {
    emit update();
    if(previewCam) ui->cameraPreview->setPixmap(cvMatToQPixmap(this->previewCam->getFrame()));
}

