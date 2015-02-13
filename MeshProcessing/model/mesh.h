#ifndef MESH_H
#define MESH_H

#include "vertex.h"

#include <QVector>
#include <QVector3D>
#include <QDebug>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
using namespace Eigen;

#include <fstream>
#include <iostream>
using namespace std;

class Mesh
{
public:
    Mesh();
    ~Mesh();

    inline QVector<QVector3D>& getVertices() {
        return vertices;
    }

    inline QVector<QVector3D>& getNormals() {
        return normals;
    }

    inline QVector<QVector3D>& getFixedColor() {
        return fixedcolors;
    }

    inline QVector<QVector3D>& getColors() {
        return colors;
    }

    inline QVector<QVector3D>& getEigenColors() {
        return eigenColors;
    }

    inline QVector<QVector3D>& getFaces() {
        return faces;
    }

    inline QVector<double>& getLabels() {
        return Labels;
    }

    inline QVector< QSet<int> >& getP2P_Neigh() {
        return P2P_Neigh;
    }

    inline QVector< QSet<int> >& getP2F_Neigh() {
        return P2F_Neigh;
    }

    inline QVector< QSet<int> >& getF2F_Neigh() {
        return F2F_Neigh;
    }

    inline QVector<QVector3D>& getFaceNormals() {
        return faceNormals;
    }

    inline QVector<QVector3D>& getVertexNormalVertices() {
        return vertexNormalVertices;
    }

    inline QVector<QVector3D>& getVertexNormalNormals() {
        return vertexNormalNormals;
    }

    inline QVector<QVector3D>& getVertexNormalColors() {
        return vertexNormalColors;
    }

    inline QVector<QVector3D>& getVertexNormalRGBColors() {
        return vertexNormalRGBColors;
    }

    inline QVector<QVector3D>& getFaceNormalVertices(){
        return faceNormalVertices;
    }

    inline QVector<QVector3D>& getFaceNormalNormals(){
        return faceNormalNormals;
    }

    inline QVector<QVector3D>& getFaceNormalColors(){
        return faceNormalColors;
    }

    inline QVector<QVector3D>& getFaceNormalRGBColors(){
        return faceNormalRGBColors;
    }

    inline QVector<QVector3D>& getFaceNormalEigenColors(){
        return faceNormalEigenColors;
    }


    // Move to impl

    // Clear vertex array
    inline void ClearVertices() {
        vertices.clear();
    }

    // Vertex #i
    inline QVector3D& Vertex(int i) {
        assert( (i>=0) && (i<vertices.size()) );
        return vertices[i];
    }

    inline QVector3D& Vertex(int f, int v) {
        assert( (f>=0) && (f<faces.size()) );
        assert( (v>=0) && (v<=2) );
        assert( (faces[f][v]>=0) && (faces[f][v]<vertices.size()) );
        return vertices[faces[f][v]];
    }

    // Vertex normal of face #f with index #v
    // v is in range 0 to 2
    inline QVector3D& VertexNormal(int f, int v) {
        assert( (f>=0) && (f<faces.size()) );
        assert( (v>=0) && (v<=2) );
        assert( (faces[f][v]>=0) && (faces[f][v]<normals.size()) );
        return normals[faces[f][v]];
    }

    // Face normal #i
    inline QVector3D& FaceNormal(int i) {
        assert( (i>=0) && (i<faceNormals.size()) );
        return faceNormals[i];
    }

    // Compute raw normal of face #f
    inline QVector3D computeRawFaceNormal( int f ) {
        return QVector3D::crossProduct(Vertex(f,1)-Vertex(f,0),  Vertex(f,2)-Vertex(f,0));
    }

    // Compute raw normal of face {va, vb, vc}
    inline QVector3D computeRawFaceNormal( int va, int vb, int vc ) {
        return QVector3D::crossProduct(Vertex(vb)-Vertex(va),  Vertex(vc)-Vertex(va));
    }

    // Compute unit normal of face #f
    inline QVector3D computeFaceNormal( int f ) {
        assert( (f>=0) && ( f <faces.size()) );
        return computeRawFaceNormal(f).normalized();
    }

    // Compute unit normal of face {va, vb, vc}
    inline QVector3D computeFaceNormal( int va, int vb, int vc ) {
        return computeRawFaceNormal(va, vb, vc).normalized();
    }

    // Compute area of face #i
    inline double computeFaceArea( int i ) {
        return 0.5 * computeRawFaceNormal(i).length();
    }

    // Compute area of face {va, vb ,vc}
    inline double computeFaceArea( int va, int vb, int vc ) {
        return 0.5 * computeRawFaceNormal(va, vb, vc).length();
    }

    //Initializing Laplcian Matrix
    void InitializeLaplace(unsigned int m){
        laplacianmatrix.resize(m,m);
        laplacianmatrix.setZero(m,m);
    }

    // Creating Laplacian Matrix
    void LaplacianMatrix(){
        int VSize = vertices.size();
        QSet<int>:: iterator it,it_term;
        for(int i = 0; i < VSize; i++){
            it_term = P2P_Neigh[i].end();
            laplacianmatrix.coeffRef(i, i) = P2P_Neigh.at(i).size();   //size of the connection
            for(it = P2P_Neigh[i].begin(); it != it_term; ++it){
                laplacianmatrix.coeffRef(i, *it) = -1;           //Put -1 where there is a connection
                laplacianmatrix.coeffRef(*it, i) = -1;           //As Laplacian Matrix is a symmetric Matrix
            }
        }
        std::ofstream file("matrix.txt");
        if(file.is_open()){
            file<<"Here is the Laplacian Matrix \n"<<laplacianmatrix<<"\n";
            file.close();
        }
        else
            cout << "Unable to open file";
    }

    //Eigen Vector Decomposition
    void EigenDecompose(){
        SelfAdjointEigenSolver<MatrixXd> e;
        e.compute(laplacianmatrix);
        eigenvector= (e.eigenvectors().real());
   }


    bool mapping(int value){
        //if(!canIgetEigen) return false;
        if(value > eigenvector.cols())
            return false;       //cant access non existent values
        VectorXd norm;
        norm.resize(eigenvector.rows());
        norm = eigenvector.col(value);
//        cout<<"Normalizing the Eigen values"<<endl;
        normalizeeigen(norm, 0, 1);
        return true;
    }


    QVector<double> normeigen;
    void normalizeeigen(VectorXd c, int a, int b){
        if(!normeigen.empty())
            normeigen.clear();
        int size = c.rows();
        double min = c.minCoeff();
        double max = c.maxCoeff();
        for(int i = 0; i < size; ++i){
            normeigen.push_back((b - a)*(c.coeffRef(i) - min)/(max - min) + a);
        }
        eigenColors.clear();
        for(int i=0; i < c.rows(); i++){
            eigenColors.append(DoubleToColor(normeigen.at(i)));
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

private:
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector3D> fixedcolors;
    QVector<QVector3D> colors;
    QVector<QVector3D> eigenColors;
    QVector<QVector3D> faces;
    QVector<QVector3D> faceNormals;

    QVector<double> Labels;
    QVector < QSet<int> > P2P_Neigh;
    QVector < QSet<int> > P2F_Neigh;
    QVector < QSet<int> > F2F_Neigh;

    QVector<QVector3D> vertexNormalVertices;
    QVector<QVector3D> vertexNormalNormals;
    QVector<QVector3D> vertexNormalColors;
    QVector<QVector3D> vertexNormalRGBColors;

    QVector<QVector3D> faceNormalVertices;
    QVector<QVector3D> faceNormalNormals;
    QVector<QVector3D> faceNormalColors;
    QVector<QVector3D> faceNormalRGBColors;

    QVector<QVector3D> faceNormalEigenColors;

    MatrixXd laplacianmatrix;
    MatrixXd eigenvector;
};

#endif // MESH_H
