QT       += core gui xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

OBJECTS_DIR = ../obj/OBJ
MOC_DIR     = ../obj/MOC
UI_DIR      = ../obj/UI
RCC_DIR     = ../obj/RCC
TARGET      = ../../bin/VistarPlanner
TEMPLATE    = app

DEFINES += _USE_MATH_DEFINES

# ============================================================
#  OS-agnostic QGIS paths
#  Windows : QGIS via OSGeo4W  (override with qmake OSGEO4W_ROOT=...)
#  Linux   : sudo apt install libqgis-dev qgis-providers
#  macOS   : brew install qgis
# ============================================================
win32 {
    isEmpty(OSGEO4W_ROOT) {
        # Default OSGeo4W system-wide install
        OSGEO4W_ROOT = C:/OSGeo4W
        # Per-user AppData install (Shridhar's layout) — used if it exists
        exists(C:/Users/$$(USERNAME)/AppData/Local/Programs/OSGeo4W/apps/qgis-ltr-dev/include) {
            OSGEO4W_ROOT = C:/Users/$$(USERNAME)/AppData/Local/Programs/OSGeo4W
        }
    }
    QGIS_INCLUDE = $$OSGEO4W_ROOT/apps/qgis-ltr-dev/include
    QGIS_LIB     = $$OSGEO4W_ROOT/apps/qgis-ltr-dev/lib
    QGIS_DEP_LIB = $$OSGEO4W_ROOT/lib

    INCLUDEPATH += $$QGIS_INCLUDE
    DEPENDPATH  += $$QGIS_INCLUDE

    LIBS += -L$$QGIS_LIB \
            -lqgis_core \
            -lqgis_gui \
            -lqgis_analysis

    LIBS += -L$$QGIS_DEP_LIB \
            -lgeos_c \
            -lproj \
            -lspatialite
}

unix:!macx {
    QGIS_INCLUDE = /usr/include/qgis
    QGIS_LIB     = /usr/lib

    INCLUDEPATH += $$QGIS_INCLUDE
    DEPENDPATH  += $$QGIS_INCLUDE

    LIBS += -L$$QGIS_LIB \
            -lqgis_core \
            -lqgis_gui \
            -lqgis_analysis \
            -lgeos_c \
            -lproj \
            -lspatialite
}

macx {
    QGIS_INCLUDE = /usr/local/include/qgis
    QGIS_LIB     = /usr/local/lib

    INCLUDEPATH += $$QGIS_INCLUDE
    DEPENDPATH  += $$QGIS_INCLUDE

    LIBS += -L$$QGIS_LIB \
            -lqgis_core \
            -lqgis_gui \
            -lqgis_analysis \
            -lgeos_c \
            -lproj \
            -lspatialite
}

# ============================================================
#  Sources  (kept exactly as in original)
# ============================================================
SOURCES += \
    MapDisplay/cmapcanvas.cpp \
    MapDisplay/cmaptoolselectobjects.cpp \
    MapDisplay/cpathgenerator.cpp \
    MapDisplay/cvistarobject.cpp \
    MapDisplay/cvistarroute.cpp \
    MapDisplay/qgspointxyz.cpp \
    RadarView/radarmanager.cpp \
    RadarView/radarattributesdialog.cpp \
    RadarView/radarlistpanel.cpp \
    RadarView/radarppiwidget.cpp \
    RadarView/radardockwidget.cpp \
    RadarView/radardisplaypanel.cpp \
    cclutterparamsdialog.cpp \
    ccoverageparamsdialog.cpp \
    cloadscenariodialog.cpp \
    cnetworkinterface.cpp \
    cpathsettingsdialog.cpp \
    csavescenariodialog.cpp \
    cscenariomanager.cpp \
    cupdateposition.cpp \
    cupdateroute.cpp \
    main.cpp \
    cvistarplanner.cpp

HEADERS += \
    MapDisplay/cmapcanvas.h \
    MapDisplay/cmaptoolselectobjects.h \
    MapDisplay/cpathgenerator.h \
    MapDisplay/cvistarobject.h \
    MapDisplay/cvistarroute.h \
    MapDisplay/qgspointxyz.h \
    RadarView/radarattributes.h \
    RadarView/radarattributesdialog.h \
    RadarView/radarmanager.h \
    RadarView/radarlistpanel.h \
    RadarView/radarppiwidget.h \
    RadarView/radardockwidget.h \
    RadarView/radardisplaypanel.h \
    cclutterparamsdialog.h \
    ccoverageparamsdialog.h \
    cloadscenariodialog.h \
    cnetworkinterface.h \
    cpathsettingsdialog.h \
    csavescenariodialog.h \
    cscenariomanager.h \
    cupdateposition.h \
    cupdateroute.h \
    cvistarplanner.h \
    globalConstants.h

FORMS += \
    cloadscenariodialog.ui \
    cpathsettingsdialog.ui \
    csavescenariodialog.ui \
    cupdateposition.ui \
    cupdateroute.ui \
    cvistarplanner.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myRes.qrc
