#ifndef CONSTANTS
#define CONSTANTS

#include <QColor>

#define ONE 1
#define ZERO 0;
#define HALF_GRID_SIZE 10

#define INITIAL_POINT_SIZE 2
#define INITIAL_VERTEX_COLOR QColor(248,80,7);

enum RenderingModes
{
    VERTEX_ONLY,
    VERTEX_NORMAL,
    FACE_NORMAL,
    VERTEX_NORMAL_RGB,
    FACE_NORMAL_RGB,
    EIGEN_RGB
};

#endif // CONSTANTS

