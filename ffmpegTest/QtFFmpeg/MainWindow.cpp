#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Interface.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->connect(ui->OpenCamera_PB, SIGNAL(clicked()), this, SLOT(onOpenCameraButtonClicked()));
    this->connect(ui->CloseCamera_PB, SIGNAL(clicked()), this, SLOT(onCloseCameraButtonClicked()));
    this->connect(ui->Start_PB, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    this->connect(ui->Stop_PB, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

    ui->Url_LE->setText("0");
    ui->xoffset_LE->setText("0");
    ui->yoffset_LE->setText("0");
    ui->width_LE->setText("-1");
    ui->height_LE->setText("-1");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenCameraButtonClicked()
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__;
    Interface::Get()->OpenCamera();

    ui->ShowWidget->SetVideoSize(Interface::Get()->GetCameraW(), Interface::Get()->GetCameraH());
    ui->ShowWidget->SetViewStates(true);
}

void MainWindow::onCloseCameraButtonClicked()
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__;
    Interface::Get()->CloseCamera();
    ui->ShowWidget->SetViewStates(false);
}

void MainWindow::onStartButtonClicked()
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__;
    int xoffset = ui->xoffset_LE->text().toInt();
    int yoffset = ui->yoffset_LE->text().toInt();
    int width = ui->width_LE->text().toInt();
    int height = ui->height_LE->text().toInt();

    if(width == -1 || height == -1){
        width = 640;
        height = 480;
    }

    std::string outpath("1.mp4");
    Interface::Get()->Start(xoffset, yoffset, width, height, outpath);
}

void MainWindow::onStopButtonClicked()
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__;
    Interface::Get()->Stop();
}
