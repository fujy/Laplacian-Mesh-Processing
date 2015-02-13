#ifndef GRID_H
#define GRID_H

#include <QVector>
#include <QVector3D>


class Grid
{
public:
    Grid();
    ~Grid();

    inline QVector<QVector3D>& getGridLines() {
        return gridLines;
    }

private:
    QVector<QVector3D> gridLines;
};

#endif // GRID_H
