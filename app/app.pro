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
    engine/audiofile.cpp \
    engine/audioplayback.cpp \
    engine/dancefloor.cpp \
    engine/dancefloorhardware.cpp \
    engine/dispatch.cpp \
    engine/firing.cpp \
    engine/frequencyspectrum.cpp \
    engine/lightcolor.cpp \
    engine/main.cpp \
    engine/nodefactory.cpp \
    engine/nodeparameters.cpp \
    engine/region.cpp \
    engine/spectrumanalyser.cpp \
    engine/subrange.cpp \
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
    views/cuesheet_view.cpp \
    views/group_node_item.cpp \
    views/node_item.cpp    \
    engine/segmentation.cpp \
    views/color_chip.cpp \
    views/connector_item.cpp \
    views/cuesheet_scene.cpp \
    views/dance_floor_widget.cpp \
    views/filtered_spin_box.cpp \
    views/graph_widget.cpp \
    views/gui_settings.cpp \
    views/main_window.cpp \
    views/modal_dialogs.cpp \
    views/node_library.cpp \
    views/rename_tab_dialog.cpp \
    views/settings_dialog.cpp \
    views/spectrograph.cpp \
    views/transport.cpp \
    views/sublevel_node_item.cpp


HEADERS  += \
    engine/audiofile.h \
    engine/audioplayback.h \
    engine/dancefloor.h \
    engine/dancefloorhardware.h \
    engine/dispatch.h \
    engine/firing.h \
    engine/frequencyspectrum.h \
    engine/lightcolor.h \
    engine/nodefactory.h \
    engine/nodeparameters.h \
    engine/region.h \
    engine/spectrum.h \
    engine/spectrumanalyser.h \
    engine/subrange.h \
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
    views/connector_item.h \
    views/node_library.h \
    views/cuesheet_view.h \
    views/graph_widget.h       \
    views/group_node_item.h \
    views/node_item.h          \
    engine/segmentation.h \
    views/color_chip.h \
    views/cuesheet_scene.h     \
    views/dance_floor_widget.h  \
    views/filtered_spin_box.h \
    views/gui_settings.h \
    views/main_window.h \
    views/rename_tab_dialog.h \
    views/settings_dialog.h \
    views/spectrograph.h \
    views/transport.h \
    views/sublevel_node_item.h


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
