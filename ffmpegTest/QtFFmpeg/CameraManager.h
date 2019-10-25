#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H


#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
using namespace cv;
using namespace std;

class CameraManager
{
public:
    CameraManager();

    static CameraManager* GetInstance(){
        if(gInstance == nullptr){
            gInstance = new CameraManager();
        }
        return gInstance;
    }

    bool OpenCamera(int width, int height);
    void CloseCamera();

    void GetImageDataBGRA(unsigned char* BGRA);
    void GetImageDataYUV420P(unsigned char* yuv420p);
    int GetCameraWidth(){return width;}
    int GetCameraHeight(){return height;}
private:

    int width;
    int height;
    int imageSize;
    int yuvImageSize;
    VideoCapture         * m_capture;

    unsigned char        * m_BGRABuffer;
    unsigned char        * m_YUV420PBuffer;

    static CameraManager * gInstance;

    bool isOpened;

};

#define gCameraManager CameraManager::GetInstance()

#endif // CAMERAMANAGER_H
