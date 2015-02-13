#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include "mesh.h"
#include <QString>
#include <QFile>

class MeshBuilder
{
public:
    MeshBuilder();
    ~MeshBuilder();

    Mesh* buildMesh(QFile *file);
    Mesh* buildLaplacian(Mesh *mesh);
    int buildEigenRenderingVectors(Mesh* mesh);

private:
    int readMeshFile(QFile* file, Mesh* mesh);
    bool build_P2P_Neigh(Mesh *mesh);
    int computeFaceNormals(Mesh* mesh);
    int computeVertexNormals(Mesh* mesh);
    int buildDistanceLabels(Mesh* mesh);
    void setColorsFromLabels(Mesh* mesh);
    int buildRenderingVectors(Mesh* mesh);
};

#endif // MESHBUILDER_H
