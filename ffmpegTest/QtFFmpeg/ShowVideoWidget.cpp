#include "ShowVideoWidget.h"
#include "CameraManager.h"


ShowVideoWidget::ShowVideoWidget(QWidget *para) : QOpenGLWidget(para)
{
    this->textureUniformY = 0;
    this->textureUniformU = 0;
    this->textureUniformV = 0;

    this->idY = 0;
    this->idU = 0;
    this->idV = 0;

    this->pVShader = NULL;
    this->pFShader = NULL;
    this->program = NULL;
    this->pTextureY = NULL;
    this->pTextureU = NULL;
    this->pTextureV = NULL;

    this->pBufYuv420p = NULL;
    this->mVideoW = 0;
    this->mVideoH = 0;
    viewFlag = false;
    gCameraManager->OpenCamera(1280, 960);
}

ShowVideoWidget::~ShowVideoWidget()
{

}

void ShowVideoWidget::SetVideoSize(int width, int height)
{
    this->mVideoW = width;
    this->mVideoH = height;

    this->pBufYuv420p = new unsigned char[this->mVideoW * this->mVideoH * 3 / 2];

    this->resize(width, height);
    this->resizeGL(width, height);
}

void ShowVideoWidget::SetYUVData(unsigned char *yuvData, int width, int height)
{
    memcpy(this->pBufYuv420p, yuvData, width * height * 3 / 2);
}

void ShowVideoWidget::SetViewStates(bool flag)
{
    viewFlag = flag;
    if(viewFlag){
        update();
    }
}

void ShowVideoWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    const char *vsrc = "attribute vec4 vertexIn;     \
                        attribute vec2 textureIn;    \
                        varying vec2 textureOut;     \
                        void main(void){             \
                            gl_Position = vertexIn;  \
                            textureOut = textureIn;  \
                        }";

    const char *fsrc = "varying vec2 textureOut;     \
                        uniform sampler2D tex_y;     \
                        uniform sampler2D tex_u;     \
                        uniform sampler2D tex_v;     \
                        void main(void){             \
                            vec3 yuv;                \
                            vec3 rgb;                \
                            yuv.x = texture2D(tex_y, textureOut).r;       \
                            yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
                            yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
                            rgb = mat3(1,       1,       1,                \
                                       0,       -0.39465,2.03211,          \
                                       1.13983, -0.58060,0) * yuv;         \
                            gl_FragColor = vec4(rgb, 1);                  \
                        }";

    this->pVShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    this->pFShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    bool bCompile = this->pVShader->compileSourceCode(vsrc);
    if(!bCompile){
        qDebug() << "compileSourceCode vsrc failed!\n" << vsrc;
    }
    bCompile = this->pFShader->compileSourceCode(fsrc);
    if(!bCompile){
        qDebug() << "compileSourceCode fsrc failed!\n" << fsrc;
    }

    this->program = new QOpenGLShaderProgram();
    this->program->addShader(this->pVShader);
    this->program->addShader(this->pFShader);

    this->program->bindAttributeLocation("vertexIn", 0);
    this->program->bindAttributeLocation("textureIn", 1);
    this->program->link();
    this->program->bind();

    this->program->enableAttributeArray(0);
    this->program->enableAttributeArray(1);

    this->program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    this->program->setAttributeBuffer(1, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

    this->textureUniformY = program->uniformLocation("tex_y");
    this->textureUniformU = program->uniformLocation("tex_u");
    this->textureUniformV = program->uniformLocation("tex_v");

    static const GLfloat vertexVertices[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             -1.0f, 1.0f,
             1.0f, 1.0f,
        };
        //纹理矩阵
    static const GLfloat textureVertices[] = {
            0.0f,  1.0f,
            1.0f,  1.0f,
            0.0f,  0.0f,
            1.0f,  0.0f,
        };

    //设置属性ATTRIB_VERTEX的顶点矩阵值以及格式
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertexVertices);
    qDebug() << "7";
    //设置属性ATTRIB_TEXTURE的纹理矩阵值以及格式
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices);
    qDebug() << "8";
    //启用ATTRIB_VERTEX属性的数据,默认是关闭的
    glEnableVertexAttribArray(0);
    //启用ATTRIB_TEXTURE属性的数据,默认是关闭的
    glEnableVertexAttribArray(1);


    pTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    pTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    pTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);

    pTextureY->create();
    pTextureU->create();
    pTextureV->create();

    idY = pTextureY->textureId();
    idU = pTextureU->textureId();
    idV = pTextureV->textureId();


    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void ShowVideoWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ShowVideoWidget::paintGL()
{
    if(!viewFlag){
        return;
    }

    gCameraManager->GetImageDataYUV420P(this->pBufYuv420p);

    //加载y数据纹理
    //激活纹理单元GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    //使用来自y数据生成纹理
    glBindTexture(GL_TEXTURE_2D, idY);
    //使用内存中m_pBufYuv420p数据创建真正的y数据纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW, mVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, pBufYuv420p);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //加载u数据纹理
    glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
    glBindTexture(GL_TEXTURE_2D, idU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW/2, mVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)pBufYuv420p+mVideoW*mVideoH);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //加载v数据纹理
    glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
    glBindTexture(GL_TEXTURE_2D, idV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW/2, mVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)pBufYuv420p+mVideoW*mVideoH*5/4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //指定y纹理要使用新值 只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
    //0对应纹理单元GL_TEXTURE0 1对应纹理单元GL_TEXTURE1 2对应纹理的单元
    glUniform1i(textureUniformY, 0);
    //指定u纹理要使用新值
    glUniform1i(textureUniformU, 1);
    //指定v纹理要使用新值
    glUniform1i(textureUniformV, 2);
    //使用顶点数组方式绘制图形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    update();

}
