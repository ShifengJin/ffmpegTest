#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
class Interface
{
public:
    Interface();

    static Interface* Get();

    void OpenCamera();
    void CloseCamera();
    int GetCameraW();
    int GetCameraH();

    void Start(int xoffset, int yoffset, int width, int height, std::string &path);
    void Stop();

private:
    static Interface* instance;
};

#endif // INTERFACE_H
