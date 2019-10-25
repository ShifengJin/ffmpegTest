#ifndef SHOWVIDEOWIDGET_H
#define SHOWVIDEOWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QWidget>

class ShowVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ShowVideoWidget(QWidget *para);
    ~ShowVideoWidget();
    void SetVideoSize(int width, int height);
    void SetYUVData(unsigned char* yuvData, int width, int height);

    void SetViewStates(bool flag);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    QOpenGLShaderProgram  *program;

    GLuint textureUniformY, textureUniformU, textureUniformV;
    GLuint idY, idU, idV;

    QOpenGLTexture *pTextureY;
    QOpenGLTexture *pTextureU;
    QOpenGLTexture *pTextureV;

    QOpenGLShader  *pVShader;
    QOpenGLShader  *pFShader;

    int mVideoW, mVideoH;

    unsigned char* pBufYuv420p;

    bool viewFlag = false;

};

#endif // SHOWVIDEOWIDGET_H
