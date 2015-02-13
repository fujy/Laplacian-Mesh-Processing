#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QProgressDialog>

QProgressDialog *pd;

int updateProgress(int pos)
{
  static int q=0;

  if (q % 100 == 0)
    pd->setValue(pos);
    q++;

  return pd->wasCanceled();
}

void setTotal(int size)
{
  pd->setMaximum(size);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMeshGadgets(0);
    ui->radioButton_6->setEnabled(0);
    ui->spinBox->setEnabled(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMeshGadgets(bool enabled)
{
    ui->radioButton->setEnabled(enabled);
    ui->radioButton_2->setEnabled(enabled);
    ui->radioButton_3->setEnabled(enabled);
    ui->radioButton_3->setEnabled(enabled);
    ui->radioButton_4->setEnabled(enabled);
    ui->radioButton_5->setEnabled(enabled);
    ui->pushButton_2->setEnabled(enabled);
}

void MainWindow::on_pushButton_clicked()
{
    ui->radioButton_6->setEnabled(0);
    ui->spinBox->setEnabled(0);
    QString filepath = QFileDialog::getOpenFileName(
                this,
                tr("Open Mesh File"),
                "D://University//SoftwareEngineering//selaplacian//trunk//vrml",
                "Mesh Files (*.wrl);");
    ui->openGLWidget->setMode(VERTEX_ONLY);
    loadAndRenderMeshFile(filepath);
    setMeshGadgets(1);
    ui->radioButton->setChecked(1);
}

int MainWindow::loadAndRenderMeshFile(const QString filepath)
{
    QFile *file = new QFile(filepath);
    if (!file->exists())
        return 1;

    mesh = meshBuilder.buildMesh(file);
    ui->openGLWidget->setMesh(mesh);

    int eigenVectorsCount = mesh->getVertices().size()-1;
    ui->spinBox->setMaximum(eigenVectorsCount);
    QString text("Eigen Values from 0 to ");
            text.append(eigenVectorsCount);
    ui->label->setText(text);

    delete file;
}

void MainWindow::on_radioButton_clicked()
{
    ui->openGLWidget->setMode(VERTEX_ONLY);
    ui->openGLWidget->update();
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->openGLWidget->setMode(VERTEX_NORMAL);
    ui->openGLWidget->update();
}

void MainWindow::on_radioButton_3_clicked()
{
    ui->openGLWidget->setMode(FACE_NORMAL);
    ui->openGLWidget->update();
}

void MainWindow::on_radioButton_4_clicked()
{
    ui->openGLWidget->setMode(VERTEX_NORMAL_RGB);
    ui->openGLWidget->update();
}

void MainWindow::on_radioButton_5_clicked()
{
    ui->openGLWidget->setMode(FACE_NORMAL_RGB);
    ui->openGLWidget->update();
}

void MainWindow::on_radioButton_6_clicked()
{
    ui->openGLWidget->setMode(EIGEN_RGB);
    ui->openGLWidget->update();
}

void MainWindow::on_spinBox_editingFinished()
{
    const QString value(ui->spinBox->value());
    updateEigen(value);
}

void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{
    updateEigen(arg1);
}

void MainWindow::updateEigen(const QString value)
{
    int valueInt = value.toInt();
    mesh->mapping(valueInt);
    meshBuilder.buildEigenRenderingVectors(mesh);
    ui->openGLWidget->update();
}

void MainWindow::on_pushButton_2_clicked()
{
    meshBuilder.buildLaplacian(mesh);
    ui->radioButton_6->setEnabled(1);
    ui->spinBox->setEnabled(1);
}
