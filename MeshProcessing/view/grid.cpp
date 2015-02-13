#include "grid.h"
#include "constants.h"

#include <QVector3D>

Grid::Grid()
{
    for(int i=-HALF_GRID_SIZE;i<=HALF_GRID_SIZE;i++)
    {
        gridLines << QVector3D((double)i,0,(double)-HALF_GRID_SIZE);
        gridLines << QVector3D((double)i,0,(double)HALF_GRID_SIZE);
        gridLines << QVector3D((double)-HALF_GRID_SIZE,0,(double)i);
        gridLines << QVector3D((double)HALF_GRID_SIZE,0,(double)i);
    }
}

Grid::~Grid()
{

}

