#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void onOpenCameraButtonClicked();
    void onCloseCameraButtonClicked();
    void onStartButtonClicked();
    void onStopButtonClicked();

};

#endif // MAINWINDOW_H
