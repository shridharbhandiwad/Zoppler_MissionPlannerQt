QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


OBJECTS_DIR = ../obj/OBJ
MOC_DIR = ../obj/MOC
UI_DIR = ../obj/UI
RCC_DIR = ../obj/RCC

TARGET = ../../bin/VistarPlanner
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Change these to your actual paths
QGIS_PREFIX = C:\Users\Shridhar\AppData\Local\Programs\OSGeo4W
QGIS_INCLUDE = $$QGIS_PREFIX/apps/qgis-ltr-dev/include
#QGIS_LIB = $$QGIS_PREFIX/

DEFINES += _USE_MATH_DEFINES

INCLUDEPATH += $$QGIS_INCLUDE
DEPENDPATH += $$QGIS_INCLUDE
LIBS += -L$$QGIS_PREFIX/apps/qgis-ltr-dev/lib \
    -lqgis_core \
     -lqgis_gui
     -lqgis_analysis

 LIBS += -L$$QGIS_PREFIX/lib \
     -lgeos_c \
     -lproj \
     -lspatialite

SOURCES += \
    MapDisplay/cmapcanvas.cpp \
    MapDisplay/cmaptoolselectobjects.cpp \
    MapDisplay/cvistarobject.cpp \
    MapDisplay/cvistarroute.cpp \
    MapDisplay/qgspointxyz.cpp \
    cnetworkinterface.cpp \
    main.cpp \
    cvistarplanner.cpp

HEADERS += \
    MapDisplay/cmapcanvas.h \
    MapDisplay/cmaptoolselectobjects.h \
    MapDisplay/cvistarobject.h \
    MapDisplay/cvistarroute.h \
    MapDisplay/qgspointxyz.h \
    cnetworkinterface.h \
    cvistarplanner.h \
    globalConstants.h

FORMS += \
    cvistarplanner.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myRes.qrc
