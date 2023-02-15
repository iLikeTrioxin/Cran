#pragma once

#include "Aliases.h"
#include "MultiCameraTracker.h"

#include <QMainWindow>
#include <QTimer>
#include <opencv2/core/mat.hpp>

#include <utility>
#include "CameraTrackerWindow.h"
DECLARE_QT_CLASS(MainWindow)
class Worker : public QObject {
Q_OBJECT
public:
    QMainWindow *window;

    Worker(QMainWindow *window, QObject *parent = nullptr) : QObject(parent), window(window) {}

public slots:
    void process() {
        window->show();

        QEventLoop loop;
        loop.exec();
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    QTimer _timer;
public:
    std::vector<std::pair<QThread*, Worker*>> cameraTrackerWindows;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addCamera();

private:
    Ui::MainWindow *ui = nullptr;

    Cranel::CranelCamera* previewCam = nullptr;
    Cranel::MultiCameraTracker multiCameraTracker;

signals:
    void update();

public slots:
    void onCurrentIndexChanged(int index);
    void closeTracker(CameraTrackerWindow* window);

private slots:
    void on_timeout();
};
