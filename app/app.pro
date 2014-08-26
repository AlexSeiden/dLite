include(../dLite.pri)

# mostly for std::function
CONFIG  += c++11
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7


TEMPLATE = app

TARGET = dLite

QT       += multimedia widgets

SOURCES  += main.cpp \
            engine.cpp \
            frequencyspectrum.cpp \
            levelmeter.cpp \
            mainwidget.cpp \
            progressbar.cpp \
            settingsdialog.cpp \
            spectrograph.cpp \
            spectrumanalyser.cpp \
            sublevel.cpp \
            utils.cpp \
            wavfile.cpp \
    dancefloormodel.cpp \
    lightcolor.cpp \
    dancefloorwidget.cpp \
    controlpanel.cpp \
    Param.cpp \
    Cue.cpp \
    CueView.cpp \
    Whip.cpp \
    CueLibView.cpp

HEADERS  += engine.h \
            frequencyspectrum.h \
            levelmeter.h \
            mainwidget.h \
            progressbar.h \
            settingsdialog.h \
            spectrograph.h \
            spectrum.h \
            spectrumanalyser.h \
            sublevel.h \
            utils.h \
            wavfile.h \
    dancefloorwidget.h \
    dancefloormodel.h \
    lightcolor.h \
    controlpanel.h \
    Param.h \
    Cue.h \
    CueView.h \
    Whip.h \
    CueLibView.h

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
