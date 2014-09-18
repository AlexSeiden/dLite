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
    Device(bool status=false);
    ~Device();

    void setlight(int controllerIndex, int lightIndex, unsigned char *rgb);
    void setpixel(int x, int y, unsigned char *rgb);
    void send();
    void setActive(bool status = true)  {_isActive = status;}
    bool isActive()                     {return _isActive;}
    bool connect();

    static const int xsize =24;     //XXX
    static const int ysize =18;     //XXX

private:
    bool    _isActive;
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

#endif // DEVICE_H
