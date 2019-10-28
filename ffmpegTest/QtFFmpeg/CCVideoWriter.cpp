#include "CCVideoWriter.h"
#include "CameraManager.h"
#include <QDebug>
#include <stdio.h>
#include <unistd.h>
static CCVideoWriter* instance = nullptr;

static pthread_mutex_t instanceMutex;

CCVideoWriter* CCVideoWriter::GetInstance(){

    pthread_mutex_init(&instanceMutex, nullptr);

    pthread_mutex_lock(&instanceMutex);

    if(instance == nullptr){
        instance = new CCVideoWriter();
    }

    pthread_mutex_unlock(&instanceMutex);

    return instance;
}

CCVideoWriter::CCVideoWriter()
{
    avFormatCtx = nullptr; // write mp4 head and tail

    videoCodecCtx = nullptr;
    //audioCodecCtx = NULL;

    videoStream = nullptr;
    //andioStream = NULL;

    videoSwCtx = nullptr;  //contex switch
    yuvFrame = nullptr;    // RGB --> YUV --> H.264
    swsCtx = nullptr;
    m_filePath = "";

    counter = 0;
    av_register_all();
    avcodec_register_all();
}

CCVideoWriter::~CCVideoWriter()
{
}

bool CCVideoWriter::StartRecordWithFilePath(const char* file){

    m_filePath = file;
#if 0
    avFormatCtx = avformat_alloc_context();
    if(avFormatCtx == NULL){
        std::cout << "avformat_alloc_context failed" << std::endl;
        return false;
    }

    // output format
    pOutputFmt = av_guess_format(NULL, file, NULL);
    if(pOutputFmt == NULL){
        std::cout << "av_guess_format failed" << std::endl;
        return false;
    }

    avFormatCtx->oformat = pOutputFmt;

    if(avio_open2(&(avFormatCtx->pb), file, AVIO_FLAG_READ_WRITE, NULL, NULL) < 0){
        std::cout << "avio_open2 failed" << std::endl;
        return false;
    }

    videoCodec = avcodec_find_encoder(pOutputFmt->video_codec);
    if(videoCodec == NULL){
        std::cout << "avcodec_find_encoder failed" << std::endl;
        return false;
    }
#else
    videoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(videoCodec == nullptr){
        qDebug() << "avcodec_find_encoder error";
    }
#endif

    qDebug() << __LINE__;
    videoCodecCtx = avcodec_alloc_context3(videoCodec);
    if(videoCodecCtx == nullptr){
        std::cout << "avcodec_alloc_context3 failed" << std::endl;
        return false;
    }
qDebug() << __LINE__;
    videoCodecCtx->bit_rate = 400000000;
    //videoCodecCtx->width = m_videoInWidth;
    //videoCodecCtx->height = m_videoInHeight;
    videoCodecCtx->width = m_videoOutWidth;
    videoCodecCtx->height = m_videoOutHeight;
    videoCodecCtx->time_base.num = 1;
    videoCodecCtx->time_base.den = 25;
    videoCodecCtx->framerate.num = 25;
    videoCodecCtx->framerate.den = 1;

    videoCodecCtx->gop_size = 50;  // I Frame space
    videoCodecCtx->max_b_frames = 3;
    videoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;//AV_PIX_FMT_BGR24;
    //videoCodecCtx->pix_fmt = AV_PIX_FMT_BGRA;//AV_PIX_FMT_BGR24;
qDebug() << __LINE__;
    //videoCodecCtx = videoStream->codec;
    videoCodecCtx->codec_id = AV_CODEC_ID_H264;

    //videoCodecCtx->thread_count = 8;

    //videoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    //videoCodecCtx->b_frame_strategy = true;


    videoCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    //videoCodecCtx->qmin = 10;
    //videoCodecCtx->qmax = 30;

#if 0
    AVDictionary *param = NULL;
    if(videoCodecCtx->codec_id == AV_CODEC_ID_H264){
        // preset 主要調節編碼速度和質量的平衡
        // ultrafast superfast veryfast faster faster fast medium slow slower veryslow
        av_dict_set(&param, "preset", "veryfast", 0);
        // tune 主要配合視頻類型和視覺優化的參數
        // film: 電影 真人類型
        // animation: 動畫
        // grain: 需要保留大量的grain時用
        // stillimage: 靜態圖像編碼時使用
        // psnr: 爲提高psnr做了優化的參數
        // ssim: 爲提高ssim做了優化的參數
        // fastdecode: 可以快速解碼的參數
        // zerolatency: 靈延遲，用在需要非常低的延遲的情況下
        av_dict_set(&param, "tune", "zerolatency", 0);

        // 畫質 baseline extended main high
        // baseline profile: 基本畫質 支持 I/P幀 支持誤交錯 (Progressive) 和 CAVLC
        // extended profile: 進階畫質 支持 I/P/B/SP/SI 幀 只支持無交錯(Progressive) 和 CAVLC 用的少
        // main profile: 主流畫質 I/P/B 支持無交錯和交錯也支持CAVLC
        // high profile: 高級畫質 在main profile 的基礎上增加了8x8內部預測，自定義量化,無損視頻編碼和更多的YUV格式:
        //                H.264 baseline profile     extended profile 和 main profile 都是針對 8位樣本數據.
        //                4:2:0（YUV）格式的視頻序列,在相同配置情況下 High profile 可以比main profile 降低10%的碼率
        //                根據應用領域的不同,baseline profile 多應用與實時通信領域
        //                                main profile 多應用與流媒體領域
        //                                high profile 多應用與廣電和存儲領域
        // av_dict_set(&param, "profile", "baseline", 0);

    }
#endif
qDebug() << __LINE__;
    // 打開編碼器
    //if(avcodec_open2(videoCodecCtx, videoCodec, &param)){
    if(avcodec_open2(videoCodecCtx, videoCodec, nullptr)){
        std::cout << "avcodec_open2 failed" << std::endl;
        return false;
    }
qDebug() << __LINE__;
    avformat_alloc_output_context2(&oAVFormatCtx, nullptr, nullptr, m_filePath.c_str());
    videoStream = avformat_new_stream(oAVFormatCtx, nullptr);
    if(videoStream == nullptr){
        std::cout << "avformat_new_stream failed" << std::endl;
        return false;
    }
    qDebug() << __LINE__;
    videoStream->id = 0;
    videoStream->codecpar->codec_tag = 0;
    videoStream->time_base.num = 1;
    videoStream->time_base.den = 25;
    avcodec_parameters_from_context(videoStream->codecpar, videoCodecCtx);

    av_dump_format(oAVFormatCtx, 0, file, 1);

    swsCtx = sws_getCachedContext(swsCtx, m_videoInWidth, m_videoInHeight, AV_PIX_FMT_BGRA,
                                  m_videoOutWidth, m_videoOutHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
qDebug() << __LINE__;
    picture_buf = (unsigned char*)calloc(1, sizeof(unsigned char) * m_videoInHeight * m_videoInWidth * 4);
    yuvFrame = av_frame_alloc();
    if(yuvFrame == nullptr){
        std::cout << "av_frame_alloc failed" << std::endl;
        return false;
    }
    yuvFrame->format = AV_PIX_FMT_YUV420P;
    yuvFrame->width = m_videoOutWidth;
    yuvFrame->height = m_videoOutHeight;
    int ret = av_frame_get_buffer(yuvFrame, 32);
    if(ret < 0){
        std::cout << "av_frame_get_buffer failed" << std::endl;
        return false;
    }
    qDebug() << __LINE__;
    ret = avio_open(&(oAVFormatCtx->pb), m_filePath.c_str(), AVIO_FLAG_WRITE);
    if(ret < 0){
        std::cout << "avio_open failed" << std::endl;
        return false;
    }
qDebug() << __LINE__;
    ret = avformat_write_header(oAVFormatCtx, nullptr);
    if(ret < 0){
        std::cout << "avformat_write_header failed" << std::endl;
        return false;
    }
    //av_new_packet(&videoPkt, picture_size);
qDebug() << __LINE__;
    mThreadState = THREAD_RUN;
    pthread_create(&(this->ffmpegThread), nullptr, RunFFmpegThread, this);
qDebug() << __LINE__;
    return true;
}
void CCVideoWriter::StopRecordReleaseAllResources(){

}

bool CCVideoWriter::WriteVideoFrameWithRGBData(const unsigned char* rgb){

}
bool CCVideoWriter::WriteAudioFrameWithPCMData(unsigned char* data, int captureSize){

}

void CCVideoWriter::SetVideoInSize(int videoWidth, int videoHeight)
{
    m_videoInWidth = videoWidth;
    m_videoInHeight = videoHeight;
}

void CCVideoWriter::SetVideoOutSize(int videoWidth, int videoHeight)
{
    m_videoOutWidth = videoWidth;
    m_videoOutHeight = videoHeight;
}

void *CCVideoWriter::RunFFmpegThread(void *param)
{
    CCVideoWriter* VW = (CCVideoWriter*)param;
    while(VW->mThreadState == THREAD_RUN){
        if(!gCameraManager->GetImageDataBGRA(VW->picture_buf)){
            //usleep(33333);
            continue;
        }
        qDebug() << __LINE__;
        VW->addVideoStream(VW->picture_buf);
        qDebug() << __LINE__;
    }
    pthread_exit(nullptr);
    return nullptr;
}

bool CCVideoWriter::addVideoStream(unsigned char* BGRABuffer){

    uint8_t *inData[AV_NUM_DATA_POINTERS] = {nullptr};
    inData[0] = BGRABuffer;
    int inlinesize[AV_NUM_DATA_POINTERS] = {0};
    inlinesize[0] = m_videoInWidth * 4;

    int h = sws_scale(swsCtx, inData, inlinesize, 0, m_videoInHeight, yuvFrame->data, yuvFrame->linesize);
qDebug() << __LINE__;
    yuvFrame->pts = counter * 360;

    counter ++;
qDebug() << __LINE__;
    int ret = avcodec_send_frame(videoCodecCtx,  yuvFrame);
    if(ret != 0)
        return true;
qDebug() << __LINE__;
    AVPacket pkt;
    av_init_packet(&pkt);
    ret = avcodec_receive_packet(videoCodecCtx, &pkt);
    if(ret != 0)
        return true;
    av_interleaved_write_frame(oAVFormatCtx, &pkt);
qDebug() << __LINE__;
    return true;
}

bool CCVideoWriter::addAudioStream(){

}

bool CCVideoWriter::endWriteMp4File(){

    mThreadState = THREAD_QUIT;
    pthread_join(this->ffmpegThread, nullptr);
    av_write_trailer(oAVFormatCtx);
    avio_close(oAVFormatCtx->pb);
    avformat_free_context(oAVFormatCtx);
    avcodec_close(videoCodecCtx);
    avcodec_free_context(&videoCodecCtx);
    videoCodecCtx = nullptr;
   videoCodec = nullptr;
    sws_freeContext(swsCtx);
    swsCtx = nullptr;
    if(videoStream){
        av_free(yuvFrame);
        videoStream = nullptr;
        yuvFrame = nullptr;
    }
    if(picture_buf){
        free(picture_buf);
        picture_buf = nullptr;
    }
    counter = 0;
    return true;
}

void CCVideoWriter::releaseAllRecodeResources(){

}
