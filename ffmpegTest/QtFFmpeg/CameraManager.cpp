#include "CameraManager.h"
#include "CCVideoWriter.h"
#include <stdio.h>
#include <unistd.h>
#include <QDebug>
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

    // create pthread
    mThreadState = THREAD_RUN;
    pthread_create(&(this->cameraThread), nullptr, RunCameraThread, this);
    pthread_mutex_init(&this->cameraThreadMutex, nullptr);
    isHaveNewImage = false;

    //CCVideoWriter::GetInstance()->SetVideoInSize(width, height);
    //CCVideoWriter::GetInstance()->SetVideoOutSize(width, height);

    //CCVideoWriter::GetInstance()->StartRecordWithFilePath("1.mp4");

    isOpened = true;
    return true;
}

void CameraManager::CloseCamera()
{
    mThreadState = THREAD_QUIT;
    pthread_join(this->cameraThread, nullptr);

    pthread_mutex_destroy(&(this->cameraThreadMutex));


    //CCVideoWriter::GetInstance()->endWriteMp4File();

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
    isHaveNewImage = false;



}

bool CameraManager::GetImageDataBGRA(unsigned char *BGRA)
{
    if(this->m_BGRABuffer == nullptr)
        return false;
    if(!isHaveNewImage)
        return false;
    pthread_mutex_lock(&this->cameraThreadMutex);
    memcpy(BGRA,  this->m_BGRABuffer, (size_t)(this->imageSize));
    pthread_mutex_unlock(&this->cameraThreadMutex);
    return true;
}

bool CameraManager::GetImageDataYUV420P(unsigned char *yuv420p)
{
    if(this->m_YUV420PBuffer == nullptr)
        return false;
    if(!isHaveNewImage)
        return false;
    pthread_mutex_lock(&this->cameraThreadMutex);
    memcpy(yuv420p,  this->m_YUV420PBuffer, (size_t)(this->yuvImageSize));
    pthread_mutex_unlock(&this->cameraThreadMutex);
    return true;
}

void * CameraManager::RunCameraThread(void * param)
{
    CameraManager* CM = (CameraManager*)param;
    while(CM->mThreadState == THREAD_RUN){
        Mat frame;
        *CM->m_capture >> frame;    // 读取图像帧至frame
        if(!frame.empty())	    // 判断是否为空
        {

        }
        Mat frameYUV420P;
        Mat frameBGRA;

        cvtColor(frame, frameYUV420P, CV_BGR2YUV_I420);
        cvtColor(frame, frameBGRA, CV_BGR2BGRA);
        pthread_mutex_lock(&CM->cameraThreadMutex);
        memcpy(CM->m_BGRABuffer, frameBGRA.data, (size_t)(CM->imageSize));
        memcpy(CM->m_YUV420PBuffer, frameYUV420P.data, (size_t)(CM->yuvImageSize));
        CM->isHaveNewImage = true;

        //CCVideoWriter::GetInstance()->addVideoStream(CM->m_BGRABuffer);

        pthread_mutex_unlock(&CM->cameraThreadMutex);


        //usleep(33333);
    }
    pthread_exit(nullptr);
    return nullptr;
}
