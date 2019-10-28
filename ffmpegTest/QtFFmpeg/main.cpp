#include "MainWindow.h"
#include <QApplication>
#include "CCVideoWriter.h"
#include <iostream>
#include <QDebug>
#include "CameraManager.h"
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
#if 0
    CCVideoWriter* mccvideo = CCVideoWriter::GetInstance();

    int width = 640;
    int height = 480;

    mccvideo->SetVideoInSize(width, height);
    mccvideo->SetVideoOutSize(640, 480);

    mccvideo->StartRecordWithFilePath("1.mp4");
    gCameraManager->OpenCamera(width, height);
    qDebug() << "jsf .... 1";
    int counter = 0;
    unsigned char* BGRABuffer = (unsigned char*)calloc(1, width * height * 4);
    while(true)
    {
        qDebug() << "jsf .... 2";
        gCameraManager->GetImageDataBGRA(BGRABuffer);
        qDebug() << "jsf .... 3";
        mccvideo->addVideoStream(BGRABuffer);
        qDebug() << "jsf .... 4";
        counter ++;
        if(counter == 100){
            break;
        }

    }
    gCameraManager->CloseCamera();
    mccvideo->endWriteMp4File();
    free(BGRABuffer);
#endif
    return a.exec();
}
