// Copyright (C) 2014-2016 Alex Seiden 
//
#ifndef DANCE_FLOOR_HARDWARE
#define DANCE_FLOOR_HARDWARE

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

class DanceFloorHardware {
public:
    DanceFloorHardware(bool status=false);
    ~DanceFloorHardware();

    void setlight(int controllerIndex, int lightIndex, unsigned char *rgb);
    void send();
    void setActive(bool status = true);
    bool isActive() {return m_isActive;}
    bool connect();

private:
    void turnOn();
    void turnOff();

    bool    m_isActive;
    static const int IP_PORT = 6038;

    struct sockaddr_in m_addr0, m_addr1;
    int m_sockfd;

    // Color Kinetics protocol header
    static unsigned char COLOR_KINETICS_HEADER[21];

    unsigned char m_dmx0[DMX_LEN];
    unsigned char m_dmx1[DMX_LEN];
};

#endif // DANCE_FLOOR_HARDWARE
