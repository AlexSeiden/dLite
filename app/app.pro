include(../dLite.pri)
include(Viewing.pri)

# mostly for std::function, and other C++11 features.  And RTTI.
CONFIG  += c++11
CONFIG  += rtti
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7


TEMPLATE = app

TARGET = dLite

QT       += multimedia widgets

SOURCES  += \
    Remap.cpp \
    main.cpp \
    engine.cpp \
    frequencyspectrum.cpp \
    spectrumanalyser.cpp \
    utils.cpp \
    wavfile.cpp \
    lightcolor.cpp \
    Param.cpp \
    Cue.cpp \
    BeatFiles.cpp \
    Node.cpp \
    SublevelNode.cpp \
    Subrange.cpp \
    RandomNode.cpp \
    ColorNodes.cpp \
    Cupid.cpp \
    Imager.cpp \
    DanceFloor.cpp \
    PathNode.cpp \
    RegionNode.cpp \
    Firing.cpp \
    Device.cpp \
    Region.cpp \
    RegionCue.cpp \
    DotCue.cpp

HEADERS  += \
    engine.h \
    frequencyspectrum.h \
    spectrum.h \
    spectrumanalyser.h \
    utils.h \
    wavfile.h \
    lightcolor.h \
    Param.h \
    Cue.h \
    BeatFiles.h \
    Node.h \
    SublevelNode.h \
    Subrange.h \
    RandomNode.h \
    ColorNodes.h \
    Cupid.h \
    Imager.h \
    DanceFloor.h \
    PathNode.h \
    RegionNode.h \
    Firing.h \
    Device.h \
    Region.h \
    RegionCue.h \
    DotCue.h \
    Remap.h

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
