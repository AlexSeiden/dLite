include(dLite.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered
CONFIG  += c++11

SUBDIRS += 3rdparty/fftreal
SUBDIRS += app

TARGET = dLite


# mostly for std::function
QMAKE_CXXFLAGS += -stdlib=libc++ -Wno-inconsistent-missing-override
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7
QMAKE_MAC_SDK = macosx10.11

