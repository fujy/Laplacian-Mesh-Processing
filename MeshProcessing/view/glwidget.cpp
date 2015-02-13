#include "glwidget.h"
#include "constants.h"

#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QTimer>

using namespace std;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    mode = VERTEX_ONLY;

    alpha = 25;
    beta = -25;
    distance = 100;

    lightAngle = 0;

    lastCenterPosition = QVector3D(0,0,0);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(20);

}

GLWidget::~GLWidget()
{
    cleanup();
}

void GLWidget::cleanup()
{
    makeCurrent();
    doneCurrent();
}

QSize GLWidget::sizeHint() const
{
    return QSize(640, 480);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/lightingVertexShader.vsh");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/lightingFragmentShader.fsh");
    shaderProgram.link();
    coloringShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/coloringVertexShader.vsh");
    coloringShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/coloringFragmentShader.fsh");
    coloringShaderProgram.link();

    cout << "OpenGL Version : " << glGetString(GL_VERSION) << endl;
}

void GLWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);
    //    glEnable(GL_LIGHTING);
    //    glEnable(GL_LIGHT0);
    //    glEnable(GL_COLOR_MATERIAL);
    //    glEnable(GL_LIGHT_MODEL_AMBIENT);
    //    glEnable(GL_SHININESS);
    if (mesh != NULL) {
        paintMesh();
    }
}

void GLWidget::paintMesh()
{

    QMatrix4x4 mMatrix;
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    cameraTransformation.rotate(alpha, 0, 1, 0);
    cameraTransformation.rotate(beta, 1, 0, 0);

    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, lastCenterPosition, cameraUpDirection);

    mMatrix.setToIdentity();

    QMatrix4x4 mvMatrix;
    mvMatrix = vMatrix * mMatrix;

    QMatrix3x3 normalMatrix;
    normalMatrix = mvMatrix.normalMatrix();

    QMatrix4x4 lightTransformation;
    lightTransformation.rotate(lightAngle, 0, 1, 0);

    QVector3D lightPosition = lightTransformation * QVector3D(0, 10, 10);

    shaderProgram.bind();

    shaderProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);
    shaderProgram.setUniformValue("mvMatrix", mvMatrix);
    shaderProgram.setUniformValue("normalMatrix", normalMatrix);
    shaderProgram.setUniformValue("lightPosition", vMatrix * lightPosition);

    shaderProgram.setUniformValue("ambientColor", QColor(32, 32, 32));
    shaderProgram.setUniformValue("diffuseColor", QColor(128, 128, 128));
    shaderProgram.setUniformValue("specularColor", QColor(255, 255, 255));
    shaderProgram.setUniformValue("ambientReflection", (GLfloat) 1.0);
    shaderProgram.setUniformValue("diffuseReflection", (GLfloat) 1.0);
    shaderProgram.setUniformValue("specularReflection", (GLfloat) 1.0);
    shaderProgram.setUniformValue("shininess", (GLfloat) 100.0);

    glPointSize(INITIAL_POINT_SIZE);

    renderGrid();
    renderMesh(mode);

    shaderProgram.release();

    mMatrix.setToIdentity();
    mMatrix.translate(lightPosition);
    mMatrix.rotate(lightAngle, 0, 1, 0);
    mMatrix.rotate(45, 1, 0, 0);
    //    mMatrix.scale(0.1);

    coloringShaderProgram.bind();

    coloringShaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);

    coloringShaderProgram.setAttributeArray("vertex", lightSource.getSpotlightVertices().constData());
    coloringShaderProgram.enableAttributeArray("vertex");

    coloringShaderProgram.setAttributeArray("color", lightSource.getSpotlightColors().constData());
    coloringShaderProgram.enableAttributeArray("color");

    glDrawArrays(GL_TRIANGLES, 0, lightSource.getSpotlightVertices().size());

    coloringShaderProgram.disableAttributeArray("vertex");
    coloringShaderProgram.disableAttributeArray("color");

    coloringShaderProgram.release();
}

void GLWidget::renderGrid()
{
    shaderProgram.setAttributeArray("grid", grid.getGridLines().constData());
    shaderProgram.enableAttributeArray("grid");
    glDrawArrays(GL_POINTS, 0, grid.getGridLines().size());
    shaderProgram.disableAttributeArray("grid");
}

void GLWidget::renderMesh(RenderingModes mode)
{
    switch(mode)
    {
    case VERTEX_ONLY:
        renderVerticesOnly();
        break;
    case VERTEX_NORMAL:
        renderVertexNormal();
        break;
    case VERTEX_NORMAL_RGB:
        renderVertexNormalRGB();
        break;
    case FACE_NORMAL:
        renderFaceNormal();
        break;
    case FACE_NORMAL_RGB:
        renderFaceNormalRGB();
        break;
    case EIGEN_RGB:
        renderEigenRGB();
        break;
    }
}

void GLWidget::renderVerticesOnly()
{
    shaderProgram.setAttributeArray("vertex", mesh->getVertices().constData());
    shaderProgram.setAttributeArray("color", mesh->getFixedColor().constData());
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.enableAttributeArray("color");
    glDrawArrays(GL_POINTS, 0, mesh->getVertices().size());
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");
}

void GLWidget::renderVertexNormal()
{
    shaderProgram.setAttributeArray("vertex", mesh->getVertexNormalVertices().constData());
    shaderProgram.setAttributeArray("normal", mesh->getVertexNormalNormals().constData());
    shaderProgram.setAttributeArray("color", mesh->getVertexNormalColors().constData());
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.enableAttributeArray("color");
    shaderProgram.enableAttributeArray("normal");
    glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexNormalNormals().size());
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("color");
}

void GLWidget::renderVertexNormalRGB()
{
    shaderProgram.setAttributeArray("vertex", mesh->getVertexNormalVertices().constData());
    shaderProgram.setAttributeArray("normal", mesh->getVertexNormalNormals().constData());
    shaderProgram.setAttributeArray("color", mesh->getVertexNormalRGBColors().constData());
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.enableAttributeArray("color");
    shaderProgram.enableAttributeArray("normal");
    glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexNormalNormals().size());
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("color");
}
void GLWidget::renderFaceNormal()
{
    shaderProgram.setAttributeArray("vertex", mesh->getFaceNormalVertices().constData());
    shaderProgram.setAttributeArray("normal", mesh->getFaceNormalNormals().constData());
    shaderProgram.setAttributeArray("color", mesh->getFaceNormalColors().constData());
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.enableAttributeArray("color");
    shaderProgram.enableAttributeArray("normal");
    glDrawArrays(GL_TRIANGLES, 0, mesh->getFaceNormalVertices().size());
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("color");
}
void GLWidget::renderFaceNormalRGB()
{
    shaderProgram.setAttributeArray("vertex", mesh->getFaceNormalVertices().constData());
    shaderProgram.setAttributeArray("normal", mesh->getFaceNormalNormals().constData());
    shaderProgram.setAttributeArray("color", mesh->getFaceNormalRGBColors().constData());
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.enableAttributeArray("color");
    shaderProgram.enableAttributeArray("normal");
    glDrawArrays(GL_TRIANGLES, 0, mesh->getFaceNormalVertices().size());
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("color");
}
void GLWidget::renderEigenRGB()
{
    shaderProgram.setAttributeArray("vertex", mesh->getFaceNormalVertices().constData());
    shaderProgram.setAttributeArray("normal", mesh->getFaceNormalNormals().constData());
    shaderProgram.setAttributeArray("color", mesh->getFaceNormalEigenColors().constData());
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.enableAttributeArray("color");
    shaderProgram.enableAttributeArray("normal");
    glDrawArrays(GL_TRIANGLES, 0, mesh->getFaceNormalVertices().size());
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("color");
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePosition = event->pos();

    event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int deltaX = event->x() - lastMousePosition.x();
    int deltaY = event->y() - lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        alpha -= deltaX;
        while (alpha < 0) {
            alpha += 360;
        }
        while (alpha >= 360) {
            alpha -= 360;
        }

        beta -= deltaY;
        if (beta < -90) {
            beta = -90;
        }
        if (beta > 90) {
            beta = 90;
        }

        update();
    }

    if (event->buttons() & Qt::RightButton) {
//        qDebug() << "deltax = " << deltaX;
//        qDebug() << "deltay = " << deltaY;
//        alpha -= deltaX;
        float value = 0.2;
        if (deltaX < 0) {
            lastCenterPosition.setX(lastCenterPosition.x() + value) ;
        }
        if (deltaX > 0) {
            lastCenterPosition.setX(lastCenterPosition.x() - value) ;
        }

//        beta -= deltaY;
        if (deltaY < 0) {
            lastCenterPosition.setY(lastCenterPosition.y() - value) ;
        }
        if (deltaY > 0) {
            lastCenterPosition.setY(lastCenterPosition.y() + value) ;
        }
        update();
    }

    lastMousePosition = event->pos();

    event->accept();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (event->orientation() == Qt::Vertical) {
        if (delta < 0) {
            distance *= 1.1;
        } else if (delta > 0) {
            distance *= 0.9;
        }

        update();
    }

    event->accept();
}

void GLWidget::timeout()
{
    lightAngle += 1;
    while (lightAngle >= 360) {
        lightAngle -= 360;
    }
    update();
}
