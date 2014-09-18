#ifndef DEVICE_H
#define DEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// sizeof(header) + 512
#define DMX_LEN 533

class Device {
public:
    Device();
    ~Device();

    void setlight(int controllerIndex, int lightIndex, unsigned char *rgb);
    void setpixel(int x, int y, unsigned char *rgb);
    void send();

    static const int xsize =24;
    static const int ysize =18;

private:
    void connect();
    static const int IPPort = 6038;

    struct sockaddr_in addr0, addr1;
    int sockfd;

    // Color Kinetics protocol header
    static unsigned char header[21];

    unsigned char dmx0[DMX_LEN];
    unsigned char dmx1[DMX_LEN];

    // Maps pixel x,y to lightID:
    static int remap[18][24];

};

unsigned char Device::header[21] = {
        0x04, 0x01, 0xdc, 0x4a,
        0x01, 0x00, 0x01, 0x01,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff,
        0x00};

int Device::remap[18][24] =
        {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,0,0},
        {0,0,264,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,285,0,0},
        {0,0,263,0,0,0,143,144,142,145,141,146,140,147,139,148,138,149,0,0,0,286,0,0},
        {0,0,262,0,0,0,131,132,130,133,129,134,128,135,127,136,126,137,0,0,0,287,0,0},
        {0,0,261,0,0,0,119,120,118,121,117,122,116,123,115,124,114,125,0,0,0,288,0,0},
        {0,0,260,0,0,0,107,108,106,109,105,110,104,111,103,112,102,113,0,0,0,289,0,0},
        {0,0,259,0,0,0,157,156,158,155,159,154,160,153,161,152,162,151,0,0,0,290,0,0},
        {0,0,258,0,0,0,169,168,170,167,171,166,172,165,173,164,174,163,0,0,0,291,0,0},
        {0,0,257,0,0,0,0,0,179,180,178,181,177,182,176,183,0,0,0,0,0,292,0,0},
        {0,0,256,0,0,0,0,0,187,188,186,189,185,190,184,191,0,0,0,0,0,293,0,0},
        {0,0,255,0,0,0,0,0,195,196,194,197,193,198,192,199,0,0,0,0,0,294,0,0},
        {0,0,254,0,0,0,0,0,214,213,215,212,216,211,217,210,0,0,0,0,0,295,0,0},
        {0,0,253,0,0,0,0,0,222,221,223,220,224,219,225,218,0,0,0,0,0,296,0,0},
        {0,0,252,0,0,0,0,0,230,229,231,228,232,227,233,226,0,0,0,0,0,297,0,0},
        {0,0,251,0,0,0,0,0,238,237,239,236,240,235,241,234,0,0,0,0,0,298,0,0},
        {0,0,250,0,0,0,0,0,246,245,247,244,248,243,249,242,0,0,0,0,0,299,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


#endif // DEVICE_H
