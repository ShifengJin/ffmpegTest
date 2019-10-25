#include "CameraManager.h"

CameraManager* CameraManager::gInstance = nullptr;

CameraManager::CameraManager()
{
    isOpened = false;
}

bool CameraManager::OpenCamera(int width, int height)
{
    if(isOpened){
        return true;
    }
    this->width = width;
    this->height = height;
    // BGRA
    this->imageSize = this->width * this->height * 4;

    m_capture = new VideoCapture(0);    // 打开摄像头
    if(!m_capture->isOpened())    // 判断是否打开成功
    {
        cout << "open camera failed. " << endl;
        return false;
    }
    m_capture->set(CV_CAP_PROP_FPS, 30);
    m_capture->set(CV_CAP_PROP_FRAME_WIDTH, this->width);
    m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, this->height);

    m_BGRABuffer = new unsigned char[this->imageSize];
    if(m_BGRABuffer == nullptr){
        cout << "calloc BGRABuffer failed." << endl;
        delete m_capture;
        return false;
    }
    yuvImageSize = this->width * this->height * 3 / 2;
    m_YUV420PBuffer = new unsigned char[yuvImageSize];

    isOpened = true;
    return true;
}

void CameraManager::CloseCamera()
{
    if(m_capture){
        m_capture->release();
        delete m_capture;
        m_capture = nullptr;
    }
    this->width = 0;
    this->height = 0;
    this->imageSize = 0;
    isOpened = false;
    if(m_BGRABuffer){
        delete[] m_BGRABuffer;
        m_BGRABuffer = nullptr;
    }

    if(m_YUV420PBuffer){
        delete m_YUV420PBuffer;
        m_YUV420PBuffer = nullptr;
    }
}

void CameraManager::GetImageDataBGRA(unsigned char *BGRA)
{
    if(BGRA == nullptr){
        return;
    }

    Mat frame;
    *m_capture >> frame;    // 读取图像帧至frame
    if(!frame.empty())	    // 判断是否为空
    {
        //imshow("camera", frame);
        //waitKey(30);
    }
    Mat frameBGRA;
    cvtColor(frame, frameBGRA, CV_BGR2BGRA);
    memcpy(BGRA, frameBGRA.data, (size_t)(this->imageSize));
}

void CameraManager::GetImageDataYUV420P(unsigned char *yuv420p)
{
    if(m_YUV420PBuffer == nullptr){
        return;
    }

    Mat frame;
    *m_capture >> frame;    // 读取图像帧至frame
    if(!frame.empty())	    // 判断是否为空
    {
        //imshow("camera", frame);
        //waitKey(30);
    }
    Mat frameYUV420P;
    cvtColor(frame, frameYUV420P, CV_BGR2YUV_I420);
    memcpy(yuv420p, frameYUV420P.data, (size_t)(this->yuvImageSize));
}
