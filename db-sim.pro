######################################################################
# Automatically generated by qmake (3.0) Mon Oct 31 13:45:50 2016
######################################################################

QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TEMPLATE = app
TARGET = db-sim
INCLUDEPATH += .

# Input
HEADERS += \
	src/gui/application.h \
	src/settings/settings.h \
	src/gui/widgets/design_widget.h \
	src/gui/widgets/dialog_panel.h \
	src/gui/widgets/info_panel.h \
	src/gui/widgets/primitives/layer.h \
	src/gui/widgets/primitives/dbdot.h

SOURCES += \
	src/main.cpp \
	src/gui/application.cpp \
	src/settings/settings.cpp \
	src/gui/widgets/design_widget.cpp \
	src/gui/widgets/dialog_panel.cpp \
	src/gui/widgets/info_panel.cpp \
	src/gui/widgets/primitives/layer.cpp \
	src/gui/widgets/primitives/dbdot.cpp
