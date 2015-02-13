
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include "model/mesh.h"
#include "constants.h"
#include "lightsource.h"
#include "grid.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    QSize sizeHint() const;

    inline RenderingModes getMode(){
        return mode;
    }

    inline void setMode(RenderingModes value){
        mode = value;
    }

    inline int getEigenValue(){
        return eigenValue;
    }

    inline void setEigenValue(int value){
        eigenValue = value;
    }

    inline LightSource& getLightSource(){
        return lightSource;
    }

    inline void setLightSource(LightSource &value){
        lightSource = value;
    }

public slots:
    void cleanup();

    inline Mesh* getMesh() {
        return mesh;
    }

    inline void setMesh(Mesh *value) {
        mesh = value;
    }

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QMatrix4x4 pMatrix;
    QOpenGLShaderProgram shaderProgram;
    QVector<QVector3D> vertices;
    QVector<QVector3D> colors;

    QOpenGLShaderProgram coloringShaderProgram;
    LightSource lightSource;
    Grid grid;
    double lightAngle;

    double alpha;
    double beta;
    double distance;
    QPoint lastMousePosition;
    QVector3D lastCenterPosition;

    Mesh* mesh;
    RenderingModes mode;
    int eigenValue;

    void paintMesh();
    void renderGrid();
    void renderMesh(RenderingModes mode);
    void renderVerticesOnly();
    void renderVertexNormal();
    void renderVertexNormalRGB();
    void renderFaceNormal();
    void renderFaceNormalRGB();
    void renderEigenRGB();

private Q_SLOTS:
    void timeout();
};

#endif // GLWIDGET_H
