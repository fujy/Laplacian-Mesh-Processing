#include "lightsource.h"
#include <QVector3D>

LightSource::LightSource()
{

    spotlightVertices << QVector3D(   50,    50,    0) << QVector3D(45.5,    50,  0.5) << QVector3D( 50.5,    50,  0.5) // Front
                      << QVector3D(   50,    50,    0) << QVector3D( 50.5,    50, -0.5) << QVector3D(45.5,    50, -0.5) // Back
                      << QVector3D(   50,    50,    0) << QVector3D(45.5,    50, -0.5) << QVector3D(45.5,    50,  0.5) // Left
                      << QVector3D(   50,    50,    0) << QVector3D( 50.5,    50,  0.5) << QVector3D( 50.5,    50, -0.5) // Right
                      << QVector3D(45.5,    50, -0.5) << QVector3D( 50.5,    50, -0.5) << QVector3D( 50.5,    50,  0.5) // Bottom
                      << QVector3D( 50.5,    50,  0.5) << QVector3D(45.5,    50,  0.5) << QVector3D(45.5,    50, -0.5);
    spotlightColors << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) // Front
                    << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) // Back
                    << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) // Left
                    << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) << QVector3D(0.2, 0.2, 0.2) // Right
                    << QVector3D(  1,   1,   1) << QVector3D(  1,   1,   1) << QVector3D(  1,   1,   1) // Bottom
                    << QVector3D(  1,   1,   1) << QVector3D(  1,   1,   1) << QVector3D(  1,   1,   1);
}

LightSource::~LightSource()
{

}

