QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui
RCC_DIR = build/rcc

TARGET = build/VistarPlanner
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# QGIS paths - Linux configuration
unix {
    # Try to detect QGIS installation
    exists(/usr/include/qgis) {
        QGIS_INCLUDE = /usr/include/qgis
    } else:exists(/usr/local/include/qgis) {
        QGIS_INCLUDE = /usr/local/include/qgis
    }
    
    INCLUDEPATH += $$QGIS_INCLUDE
    DEPENDPATH += $$QGIS_INCLUDE
    
    LIBS += -lqgis_core \
            -lqgis_gui \
            -lqgis_analysis \
            -lgeos_c \
            -lproj \
            -lspatialite
}

# Windows configuration
win32 {
    QGIS_PREFIX = C:\Users\Shridhar\AppData\Local\Programs\OSGeo4W
    QGIS_INCLUDE = $$QGIS_PREFIX/apps/qgis-ltr-dev/include
    
    DEFINES += _USE_MATH_DEFINES
    
    INCLUDEPATH += $$QGIS_INCLUDE
    DEPENDPATH += $$QGIS_INCLUDE
    LIBS += -L$$QGIS_PREFIX/apps/qgis-ltr-dev/lib \
        -lqgis_core \
         -lqgis_gui \
         -lqgis_analysis

     LIBS += -L$$QGIS_PREFIX/lib \
         -lgeos_c \
         -lproj \
         -lspatialite
}

SOURCES += \
    MapDisplay/cmapcanvas.cpp \
    MapDisplay/cmaptoolselectobjects.cpp \
    MapDisplay/cvistarobject.cpp \
    MapDisplay/cvistarroute.cpp \
    MapDisplay/qgspointxyz.cpp \
    cnetworkinterface.cpp \
    cupdateposition.cpp \
    cupdateroute.cpp \
    cscenariomanager.cpp \
    cloadscenariodialog.cpp \
    csavescenariodialog.cpp \
    main.cpp \
    cvistarplanner.cpp

HEADERS += \
    MapDisplay/cmapcanvas.h \
    MapDisplay/cmaptoolselectobjects.h \
    MapDisplay/cvistarobject.h \
    MapDisplay/cvistarroute.h \
    MapDisplay/qgspointxyz.h \
    cnetworkinterface.h \
    cupdateposition.h \
    cupdateroute.h \
    cscenariomanager.h \
    cloadscenariodialog.h \
    csavescenariodialog.h \
    cvistarplanner.h \
    globalConstants.h

FORMS += \
    cupdateposition.ui \
    cupdateroute.ui \
    cloadscenariodialog.ui \
    csavescenariodialog.ui \
    cvistarplanner.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myRes.qrc
