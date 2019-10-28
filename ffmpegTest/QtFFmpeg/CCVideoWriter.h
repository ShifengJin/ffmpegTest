#ifndef CCVIDEOWRITER_H
#define CCVIDEOWRITER_H


extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <pthread.h>
#include <iostream>
#include <pthread.h>

class CCVideoWriter
{
public:
    CCVideoWriter();
    virtual ~CCVideoWriter();

    static CCVideoWriter* GetInstance();

private:

    AVFormatContext     *avFormatCtx; // write mp4 head and tail
    AVFormatContext     *oAVFormatCtx;

    AVCodecContext      *videoCodecCtx;
    //AVCodecContext      *audioCodecCtx;

    AVStream            *videoStream;
    //AVStream            *audioStream;

    AVCodec             *videoCodec;
    //AVCodec           *audioCodec;

    AVPacket            videoPkt;

    SwsContext          *videoSwCtx;  //contex switch
    AVFrame             *yuvFrame;    // RGB --> YUV --> H.264
    AVFrame             *bgrFrame;

    SwsContext          *swsCtx;

    std::string              m_filePath;

    int m_videoInWidth; int m_videoInHeight;
    int m_videoOutWidth; int m_videoOutHeight;
    int m_videoOutBitrate;

    //int m_audioInSamplerate; int m_audioInChannels;
    //int m_audioOutSamplerate; int m_audioOutChannels;
    //int m_audioOutBitrate;

    int picture_size;
    uint8_t *picture_buf;

    int counter = 0;

public:
    bool StartRecordWithFilePath(const char* file);
    void StopRecordReleaseAllResources();

    bool WriteVideoFrameWithRGBData(const unsigned char* rgb);
    bool WriteAudioFrameWithPCMData(unsigned char* data, int captureSize);

    void SetVideoInSize(int videoWidth, int videoHeight);
    void SetVideoOutSize(int videoWidth, int videoHeight);

private:
    static void * RunFFmpegThread (void *param);
    enum ThreadState{
        THREAD_RUN,
        THREAD_QUIT,
    };
    pthread_t ffmpegThread;
    ThreadState mThreadState;

public:
    bool addVideoStream(unsigned char* BGRABuffer);
    bool addAudioStream();

    bool endWriteMp4File();

    void releaseAllRecodeResources();
};

#endif // CCVIDEOWRITER_H
