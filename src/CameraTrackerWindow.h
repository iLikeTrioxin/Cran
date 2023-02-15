#pragma once

#include <ui_CameraTrackerWindow.h>
#include "Aliases.h"
#include <QMainWindow>
#include <QWidget>
#include <CameraTracker.h>
#include <QTimer>


QPixmap cvMatToQPixmap(const cv::Mat &mat);

class CameraTrackerWindow : public QMainWindow, public Ui::CameraTrackerWindow {
    Q_OBJECT
public slots:
    void update();

public:
    CameraTrackerWindow(QWidget* parent, Cranel::CameraTracker* tracker);

signals:
    void destroyedd(CameraTrackerWindow* window);

public:
    bool closed = false;
    Cranel::CameraTracker* tracker;

protected:
    void closeEvent(QCloseEvent *event) override;
};