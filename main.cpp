/* QVTKOpenGLWidget Initialization */
#include <vtkAutoInit.h>
#include <vtkOutputWindow.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
/***********************************/

#include <QApplication>
#include <QSurfaceFormat>
#include <QVTKOpenGLWidget.h>
#include "fdemwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

    QString path = QString(":/images/resources/");
    QIcon icon = QIcon(path + QString("logo.png"));

    FDEMWindow w;
    w.setWindowIcon(icon);
    w.showMaximized();

    return app.exec();
}
