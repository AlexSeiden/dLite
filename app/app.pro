include(../spectrum.pri)

# mostly for std::function
CONFIG  += c++11
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7


TEMPLATE = app

TARGET = spectrum

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
    cue.cpp \
    CueSheet.cpp \
    CueView.cpp \
    Param.cpp

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
    cue.h \
    CueSheet.h \
    CueView.h \
    Param.h

fftreal_dir = ../3rdparty/fftreal

INCLUDEPATH += $${fftreal_dir}

RESOURCES = spectrum.qrc

# Dynamic linkage against FFTReal DLL
macx {
        # Link to fftreal framework
        LIBS += -F$${fftreal_dir}
        LIBS += -framework fftreal
} else {
        LIBS += -L..$${spectrum_build_dir}
        LIBS += -lfftreal
}

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/spectrum
INSTALLS += target

# Deployment

DESTDIR = ..$${spectrum_build_dir}
macx {
        # Relocate fftreal.framework into spectrum.app bundle
        framework_dir = ../spectrum.app/Contents/Frameworks
        framework_name = fftreal.framework/Versions/1/fftreal
        QMAKE_POST_LINK = \
            mkdir -p $${framework_dir} &&\
            rm -rf $${framework_dir}/fftreal.framework &&\
            cp -R $${fftreal_dir}/fftreal.framework $${framework_dir} &&\
            install_name_tool -id @executable_path/../Frameworks/$${framework_name} \
                                $${framework_dir}/$${framework_name} &&\
            install_name_tool -change $${framework_name} \
                                @executable_path/../Frameworks/$${framework_name} \
                                ../spectrum.app/Contents/MacOS/spectrum
} else {
    linux-g++*: {
        # Provide relative path from application to fftreal library
        QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
    }
}


FORMS += \
    CueSheet.ui \
    CueView.ui
