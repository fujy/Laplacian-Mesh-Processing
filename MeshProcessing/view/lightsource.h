#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <QVector>
#include <QVector3D>

class LightSource
{
public:
    LightSource();
    ~LightSource();

    inline QVector<QVector3D>& getSpotlightVertices() {
        return spotlightVertices;
    }

    inline QVector<QVector3D>& getSpotlightColors() {
        return spotlightColors;
    }

private:
    QVector<QVector3D> spotlightVertices;
    QVector<QVector3D> spotlightColors;
};

#endif // LIGHTSOURCE_H
