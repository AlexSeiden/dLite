include(../dLite.pri)

# mostly for std::function, and other C++11 features.
# And RTTI, which (TODO) we can probably ditch since I'm only using
# it for node types, and those can be identified without having
# to add the RTTI overhead to everything.
CONFIG  += c++11
CONFIG  += rtti
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7
QMAKE_MAC_SDK = macosx10.11


TEMPLATE = app

TARGET = dLite

QT       += multimedia widgets

SOURCES  += \
    engine/dancefloor.cpp \
    engine/dancefloorhardware.cpp \
    engine/nodefactory.cpp \
    engine/nodeparameters.cpp \
    engine/region.cpp \
    engine/subrange.cpp \
    engine/audiofile.cpp \
    engine/audioplayback.cpp \
    engine/dispatch.cpp \
    engine/firing.cpp \
    engine/frequencyspectrum.cpp \
    engine/lightcolor.cpp \
    engine/main.cpp \
    engine/spectrumanalyser.cpp \
    engine/utils.cpp \
    nodes/BeatFiles.cpp \
    nodes/ColorNodes.cpp \
    nodes/DotCue.cpp \
    nodes/Node.cpp \
    nodes/PathNode.cpp \
    nodes/RandomNode.cpp \
    nodes/RegionCue.cpp \
    nodes/RegionNode.cpp \
    nodes/RemapperNode.cpp \
    nodes/RenderNode.cpp \
    nodes/Shape.cpp \
    nodes/SublevelNode.cpp \
    views/ColorChip.cpp \
    views/ConnectorItem.cpp \
    views/CueLibView.cpp \
    views/CuesheetScene.cpp \
    views/CuesheetView.cpp \
    views/GraphWidget.cpp \
    views/GroupNodeItem.cpp \
    views/GuiSettings.cpp \
    views/ModalDialogs.cpp \
    views/MyDoubleSpinBox.cpp \
    views/NodeItem.cpp    \
    views/RenameTabDialog.cpp \
    views/SegmentController.cpp \
    views/SublevelNodeItem.cpp \
    views/Transport.cpp \
    views/dancefloorwidget.cpp \
    views/mainwidget.cpp \
    views/settingsdialog.cpp \
    views/spectrograph.cpp \


HEADERS  += \
    engine/dancefloor.h \
    engine/dancefloorhardware.h \
    engine/nodefactory.h \
    engine/nodeparameters.h \
    engine/region.h \
    engine/subrange.h \
    engine/audiofile.h \
    engine/audioplayback.h \
    engine/dispatch.h \
    engine/firing.h \
    engine/frequencyspectrum.h \
    engine/lightcolor.h \
    engine/spectrum.h \
    engine/spectrumanalyser.h \
    engine/utils.h \
    nodes/BeatFiles.h \
    nodes/ColorNodes.h \
    nodes/DotCue.h \
    nodes/Node.h \
    nodes/PathNode.h \
    nodes/RandomNode.h \
    nodes/RegionCue.h \
    nodes/RegionNode.h \
    nodes/RemapperNode.h \
    nodes/RenderNode.h \
    nodes/Shape.h \
    nodes/SublevelNode.h \
    views/ColorChip.h \
    views/ConnectorItem.h \
    views/CueLibView.h \
    views/CuesheetScene.h     \
    views/CuesheetView.h \
    views/GraphWidget.h       \
    views/GroupNodeItem.h \
    views/GuiSettings.h \
    views/MyDoubleSpinBox.h \
    views/NodeItem.h          \
    views/RenameTabDialog.h \
    views/SegmentController.h \
    views/SublevelNodeItem.h \
    views/Transport.h \
    views/dancefloorwidget.h  \
    views/mainwidget.h \
    views/settingsdialog.h \
    views/spectrograph.h \


fftreal_dir = ../3rdparty/fftreal

INCLUDEPATH += $${fftreal_dir}

RESOURCES = \
    dLite.qrc

# Dynamic linkage against FFTReal DLL
macx {
        # Link to fftreal framework
        LIBS += -F$${fftreal_dir}
        LIBS += -framework fftreal
} else {
        LIBS += -L..$${spectrum_build_dir}
        LIBS += -lfftreal
}

target.path = ../install
INSTALLS += target


# Deployment
DESTDIR = ..$${spectrum_build_dir}
macx {
        # Relocate fftreal.framework into spectrum.app bundle
        framework_dir = ../dLite.app/Contents/Frameworks
        framework_name = fftreal.framework/Versions/1/fftreal
        QMAKE_POST_LINK = \
            mkdir -p $${framework_dir} &&\
            rm -rf $${framework_dir}/fftreal.framework &&\
            cp -R $${fftreal_dir}/fftreal.framework $${framework_dir} &&\
            install_name_tool -id @executable_path/../Frameworks/$${framework_name} \
                                $${framework_dir}/$${framework_name} &&\
            install_name_tool -change $${framework_name} \
                                @executable_path/../Frameworks/$${framework_name} \
                                ../dLite.app/Contents/MacOS/dLite
} else {
    linux-g++*: {
        # Provide relative path from application to fftreal library
        QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
    }
}


FORMS +=
