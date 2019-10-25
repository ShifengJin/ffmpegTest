#include "Interface.h"
#include "CameraManager.h"
#include "CCVideoWriter.h"

Interface* Interface::instance = nullptr;

Interface::Interface()
{

}

Interface *Interface::Get()
{
    if(instance == nullptr){
        instance = new Interface();
    }
    return instance;
}

void Interface::OpenCamera()
{
    gCameraManager->OpenCamera(1280, 960);
}

void Interface::CloseCamera()
{
    gCameraManager->CloseCamera();
}

int Interface::GetCameraW()
{
    return gCameraManager->GetCameraWidth();
}

int Interface::GetCameraH()
{
    return gCameraManager->GetCameraHeight();
}

void Interface::Start(int xoffset, int yoffset, int width, int height, std::string &path)
{
    CCVideoWriter::GetInstance()->SetVideoInSize(width, height);
    CCVideoWriter::GetInstance()->SetVideoOutSize(width, height);

    CCVideoWriter::GetInstance()->StartRecordWithFilePath(path.c_str());
}

void Interface::Stop()
{
    CCVideoWriter::GetInstance()->StopRecordReleaseAllResources();
}
