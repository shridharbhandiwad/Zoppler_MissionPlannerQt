#include "cvistarplanner.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLWidget>

#include <qgsapplication.h>
int main(int argc, char *argv[])
{
    // 🔥 Force GPU + desktop OpenGL instead of ANGLE
    qputenv("QT_OPENGL", "desktop");
    qputenv("QSG_RENDER_LOOP", "threaded");
    qputenv("QSG_INFO", "1"); // prints GPU info & backend in debug output

    // 🔥 Disable vsync (higher FPS)
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 3);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(0); // <--- disables vsync
    format.setSamples(4);      // MSAA
    QSurfaceFormat::setDefaultFormat(format);

    // 🔥 Tell Qt to use desktop OpenGL
    QgsApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QgsApplication  a(argc, argv,true);

    QgsApplication::setPrefixPath("C:/Users/Shridhar/AppData/Local/Programs/OSGeo4W/apps/qgis-ltr-dev", true);
    QgsApplication::initQgis();

    qDebug()<<"Hello";
    CVistarPlanner w;
    w.showMaximized();
    int ret = a.exec();

    QgsApplication::exitQgis();
    return ret;
}
