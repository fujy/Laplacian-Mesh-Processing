#include "mesh.h"
#include "meshbuilder.h"
#include "model/face.h"
#include "model/vertex.h"
#include "constants.h"

#include <QTextStream>
#include <QFile>
#include <QDebug>

//#include <Eigen/Core>
//#include <Eigen/Geometry>
using namespace Eigen;


MeshBuilder::MeshBuilder()
{

}

MeshBuilder::~MeshBuilder()
{

}

Mesh* MeshBuilder::buildMesh(QFile* file)
{
    Mesh *mesh = new Mesh();
    readMeshFile(file, mesh);
    computeFaceNormals(mesh);
    computeVertexNormals(mesh);
    build_P2P_Neigh(mesh);
    buildDistanceLabels(mesh);
    setColorsFromLabels(mesh);

    buildRenderingVectors(mesh);
    return mesh;
}

Mesh* MeshBuilder::buildLaplacian(Mesh* mesh)
{
    int n = mesh->getVertices().size();
    mesh->InitializeLaplace(n);
    mesh->LaplacianMatrix();
    mesh->EigenDecompose();
    mesh->mapping(230);

    buildEigenRenderingVectors(mesh);
}

int MeshBuilder::readMeshFile(QFile* file, Mesh *mesh)
{
    QColor initialColor = INITIAL_VERTEX_COLOR;
    QVector3D initialColorVector(initialColor.redF(),initialColor.greenF(),initialColor.blueF());
    file->open(QIODevice::ReadOnly);
    QString originalText = QTextStream(file).readAll();

    QString text = originalText;
    int start = -1, end = -1;


    start = text.indexOf("point");

    end = text.indexOf("coordIndex");

    text = text.mid(start, end);

    // replace the 'n' character with ''
    text.replace("n", "");
    // replace the ' ' character with ''
    //    text.replace(" ", "");
    // find the index position of the first occurrence of the string '[' in text string
    start = text.indexOf("[");
    // find the index position of the first occurrence of the string ']' in text string
    end = text.indexOf("]");
    // get the substring that is located between (start + 1) and (end - start - 1) indexes
    text = text.mid(start + 1, end - start - 1);
    // show the text that is going to be parsed

    // split the text into substrings wherever ',' occurs
    QStringList list = text.split(",");
    // for each substrings
    for(int i = 0; i < list.size(); i++) {
        // split the text into substrings wherever ' ' occurs to obtain x, y and z points
        QStringList points = list.at(i).trimmed().split(" ");
        // show i.th substring //
        // If points list not empty
        if(!points.isEmpty()) { // get x position
            QString x = points.at(0); // get y position
            QString y = points.at(1); // get z position
            QString z = points.at(2);
            // show the position
            // create the position vector
            QVector3D position(x.toDouble(), y.toDouble(), z.toDouble());
            // add the node to the node vector of the
            mesh->getVertices().append(position);
            mesh->getFixedColor().append(initialColorVector);
        }
    }
    // READ FACE INFORMATION
    // put original string to text variable
    text = originalText;
    // find the index position of the first occurrence of the string 'coordIndex' in text string
    start = text.indexOf("coordIndex");
    // get the substring that starts from start index
    text = text.mid(start);
    // replace the 'n' character with ''
    text.replace("n", "");
    // replace the ' ' character with ''
    text.replace(" ", "");
    // find the index position of the first occurrence of the string '[' in text string
    start = text.indexOf("[");
    // find the index position of the first occurrence of the string ']' in text string
    end = text.indexOf("]");
    // get the substring that is located between (start + 1) and (end - start - 1) indexes
    text = text.mid(start + 1, end - start - 1);
    // split the text into substrings wherever ', -1,' occurs
    list = text.split(",-1,");
    // for each substrings
    for(int i = 0; i < list.size(); i++) {
        // split the text into substrings wherever ' ' occurs to obtain i, j and k nodes
        QStringList faces = list.at(i).trimmed().split(",");
        // show i.th substring
        //        qDebug() << list.at(i);
        // If faces list not empty
        if(!faces.isEmpty()) {
            // get i.th node
            QString i = faces.at(0);
            // get j.th node
            QString j = faces.at(1);
            // get k.th node
            QString k = faces.at(2);
            // create the face
            QVector3D face(i.toDouble(), j.toDouble(), k.toDouble());
            // add the face to the face vector of the
            mesh->getFaces().append(face);
        }
    }
//    qDebug() << "here";
    file->close();
    return 1;
}

int MeshBuilder::computeFaceNormals(Mesh *mesh)
{
    int facesSize = mesh->getFaces().size();
    qDebug() << "fsize = " << facesSize;
    qDebug() << "fsize = " << mesh->getFaceNormals().size();
    for( int i=0; i<facesSize; i++ )
    {
        mesh->getFaceNormals().append(mesh->computeFaceNormal(i));
    }
    qDebug() << "fsize = " << mesh->getFaceNormals().size();
}

int MeshBuilder::computeVertexNormals(Mesh *mesh)
{
    mesh->getNormals().fill(QVector3D(0,0,0), mesh->getVertices().size());

    for (int i = 0; i < mesh->getFaces().size(); ++i) {
        mesh->VertexNormal(i,0) += mesh->FaceNormal(i);
        mesh->VertexNormal(i,1) += mesh->FaceNormal(i);
        mesh->VertexNormal(i,2) += mesh->FaceNormal(i);
//        qDebug() << N[0] << N[1] << N[2];
    }
    for (int i = 0; i < mesh->getVertices().size(); ++i) {
        mesh->getNormals().value(i).normalize();
    }
}

bool MeshBuilder::build_P2P_Neigh(Mesh *mesh)
{
    if ( mesh->getVertices().empty() || mesh->getFaces().empty() ) return false;

    //first build an array of empty sets

    for( int i=0; i<mesh->getVertices().size(); i++)
    {
        QSet <int> myset;
        mesh->getP2P_Neigh().append(myset);
    }

    for( int i=0; i<mesh->getFaces().size(); i++)
    {
        QVector3D F(mesh->getFaces()[i]);

        mesh->getP2P_Neigh()[ F[0] ].insert(F[1]);
        mesh->getP2P_Neigh()[ F[0] ].insert(F[2]);

        mesh->getP2P_Neigh()[ F[1] ].insert(F[0]);
        mesh->getP2P_Neigh()[ F[1] ].insert(F[2]);

        mesh->getP2P_Neigh()[ F[2] ].insert(F[0]);
        mesh->getP2P_Neigh()[ F[2] ].insert(F[1]);
    }

    return true;
}

int MeshBuilder::buildDistanceLabels(Mesh *mesh)
{
    int A = 0;
    mesh->getLabels().clear();
    for(int i=0; i<mesh->getVertices().size();i++)
        mesh->getLabels().append(1e30); // set dummy labels

    mesh->getLabels()[A] = 0; //initialize the distance to 0 for the starting point

    // compute the minimal distance for advancing front

    QSet<int> advancingfront = mesh->getP2P_Neigh()[A];
    QSet<int>::iterator it1,it2;
    for(it1 = advancingfront.begin(); it1!=advancingfront.end(); it1++)
    {
        mesh->getLabels()[*it1] =  (mesh->getVertices()[A] - mesh->getVertices()[*it1] ).length();
    }

    bool ok=true; double dist;
    QSet<int> dumset;
    while(ok)
    {   ok = false; // suppose algo should stop
        //compute the new points to be evaluated
        dumset = advancingfront;
        advancingfront.clear();

        //compute and update the distance from existing points to advancing front
        //add the P2P neigh, if required, of the advancing front to dumset
        for(it1 = dumset.begin(); it1!=dumset.end(); it1++)
        {
            for(it2=mesh->getP2P_Neigh()[*it1].begin(); it2 != mesh->getP2P_Neigh()[*it1].end(); it2++)
            {
                //compute the distance from it1 to it2. If inferior to D[it2], then update and insert
                dist = (mesh->getVertices()[*it1] - mesh->getVertices()[*it2]).length();
                if(mesh->getLabels()[*it2] > dist + mesh->getLabels()[*it1]) // this point can be reached with a shorter path
                {
                    mesh->getLabels()[*it2] = dist + mesh->getLabels()[*it1];
                    advancingfront.insert(*it2);
                    ok = true; // new path added, should continue working
                }
            }
        }
    }
}

QVector3D DoubleToColor( const double d ){

    if(d<0) return QVector3D(0,0,0);
    if(0<=d && d<0.25)		{ return QVector3D(0 , d*4.0 ,1);}

    if(0.25<=d && d<0.5)	{ return QVector3D(0 , 1.0 , 2.0-4.0*d);}

    if(0.5<=d && d<0.75)	{ return QVector3D(4.0*d - 2.0 , 1.0   ,0);	}

    if(0.75<=d && d<=1.0)	{ return QVector3D(1.0 , 4.0-4.0*d   ,0);}

    return QVector3D(1,1,1);
}

void MeshBuilder::setColorsFromLabels(Mesh *mesh)
{
    // brute force approach assuming that values are uniformly distributed (no outlier)
    if (mesh->getLabels().empty()) return;

    // get min and max labels
    double Lmin(mesh->getLabels()[0]),Lmax(mesh->getLabels()[0]);
    for(int i=1; i<mesh->getLabels().size();i++)
    {
        Lmin = qMin(Lmin, mesh->getLabels()[i]);
        Lmax = qMax(Lmax, mesh->getLabels()[i]);
    }

    mesh->getColors().clear();
    //translate values into [0,1], then convert into color, and store
    for(int i=0; i<mesh->getLabels().size();i++)
    {
        mesh->getColors().append( DoubleToColor ( (mesh->getLabels()[i] - Lmin )/(Lmax-Lmin) ) );
    }
}

int MeshBuilder::buildRenderingVectors(Mesh *mesh)
{
    QColor initialColor = INITIAL_VERTEX_COLOR;
    QVector3D initialColorVector(initialColor.redF(),initialColor.greenF(),initialColor.blueF());
    for(int i=0 ; i<mesh->getFaces().size(); i++ )
    {
        for(int j=0; j<3; j++)	{
            int vertex_index = mesh->getFaces()[i][j];
            QVector3D N(mesh->getNormals().at(vertex_index));
            QVector3D V(mesh->getVertices().at(vertex_index));
            QVector3D Col(mesh->getColors().at(vertex_index));
            mesh->getVertexNormalVertices() << QVector3D(V[0],V[1],V[2]);
            mesh->getVertexNormalNormals() << QVector3D(N[0],N[1],N[2]);
//            qDebug() << N[0] << N[1] << N[2];
            mesh->getVertexNormalColors() << initialColorVector;
            mesh->getVertexNormalRGBColors() << QVector3D(Col[0],Col[1],Col[2]);
        }
    }

    for(int i=0 ; i<mesh->getFaces().size(); i++ )
    {
        for(int j=0; j<3; j++)	{
            QVector3D V = mesh->Vertex(i,j);
            QVector3D C= mesh->getColors()[mesh->getFaces()[i][j]];
            mesh->getFaceNormalVertices() << QVector3D(V[0],V[1],V[2]);
            mesh->getFaceNormalNormals() << QVector3D(mesh->getFaceNormals()[i][0],mesh->getFaceNormals()[i][1],mesh->getFaceNormals()[i][2]);
            mesh->getFaceNormalColors() << initialColorVector;
            mesh->getFaceNormalRGBColors() << QVector3D(C[0],C[1],C[2]);
        }
    }
}


int MeshBuilder::buildEigenRenderingVectors(Mesh *mesh)
{
    mesh->getFaceNormalEigenColors().clear();
    for(int i=0 ; i<mesh->getFaces().size(); i++ )
    {
        for(int j=0; j<3; j++)	{
            int vertex_index = mesh->getFaces()[i][j];
            QVector3D N(mesh->getNormals().at(vertex_index));
            QVector3D V(mesh->getVertices().at(vertex_index));
            mesh->getVertexNormalNormals() << QVector3D(N[0],N[1],N[2]);
            mesh->getVertexNormalVertices() << QVector3D(V[0],V[1],V[2]);
            QVector3D Col(mesh->getEigenColors().at(vertex_index));
            mesh->getFaceNormalEigenColors() << QVector3D(Col[0],Col[1],Col[2]);
        }
    }
}

