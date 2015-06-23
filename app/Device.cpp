// Code for interfacing with the Color Kinetics hardware.

#include <stdio.h>
#include <iostream>
#include <QDebug>
#include "Device.h"

using namespace std;


Device::Device(bool status) :
    m_isActive(status)
{
    // Init _dmx buffers to header, plus zeros.
    memset(this->m_dmx0, 0, DMX_LEN);
    memset(this->m_dmx1, 0, DMX_LEN);
    memcpy(this->m_dmx0, this->COLOR_KINETICS_HEADER,
           sizeof(this->COLOR_KINETICS_HEADER));
    memcpy(this->m_dmx1, this->COLOR_KINETICS_HEADER,
           sizeof(this->COLOR_KINETICS_HEADER));

    if (m_isActive)
        this->connect();
}

Device::~Device() { }

//  Set the rgb value on a individual light.
void
Device::setlight(int controllerIndex, int lightIndex, unsigned char *rgb) {
    unsigned char *p;
    if (controllerIndex == 0)
        p = this->m_dmx0;
    else
        p = this->m_dmx1;
    p += sizeof(this->COLOR_KINETICS_HEADER);

    p[lightIndex*3]   = rgb[0];
    p[lightIndex*3+1] = rgb[1];
    p[lightIndex*3+2] = rgb[2];
}

// ------------------------------------------------------------------------------
// Network stuff

bool Device::connect() {
    // These were presumeably set with QuickPlayPro or some other tool by Don,
    // or are the default values, or something...
    //      IPAddrs = ('10.0.1.21', '10.0.1.22',)
    //      tropic-ck1 IP '10.0.1.21'
    //      tropic-ck2 IP '10.0.1.22'
    //      port is 6038
    //      family is 2 AF_INET
    //      socktyp is 2 SOCK_DGRAM
    //      protos are 6 and 17 -- IPPROTO_TCP and IPPROTO_UDP

    this->m_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (this->m_sockfd == -1) {
        // ErrorHandling
        std::cerr << "WARNING socket error: " << errno
                  << " \t" << strerror(errno) << std::endl;
        std::cerr.flush();
        return false;
    }

    m_addr0.sin_family = AF_INET;
    m_addr0.sin_port = htons(this->IP_PORT);
    int result = inet_pton(AF_INET, "10.0.1.21", &m_addr0.sin_addr);
    if (result != 1) {
        // ErrorHandling
        std::cerr << "WARNING addr0 inet_pton result:" << result  << "\terror: "
                  << errno << " \t" << strerror(errno) << std::endl;
        std::cerr.flush();
        return false;
    }

    m_addr1.sin_family = AF_INET;
    m_addr1.sin_port = htons(this->IP_PORT);
    result = inet_pton(AF_INET, "10.0.1.22", &m_addr1.sin_addr);
    if (result != 1) {
        // ErrorHandling
        std::cerr << "WARNING addr1 inet_pton result:" << result << "\terror: "
                  << errno << " \t" << strerror(errno) << std::endl;
        std::cerr.flush();
        return false;
    }

    return true;
}

void Device::turnOff()
{
    if (! m_isActive)
        return;

    //  TODO close port
    m_isActive = false;
    return;
}


void Device::turnOn()
{
    if (m_isActive)
        return;

    m_isActive = connect();
    return;
}


void Device::setActive(bool status)
{
    if (status)
        turnOn();
    else
        turnOff();
}

void
Device::send() {
    if (! m_isActive)
        return;

    //TODO test to see if we're sending too often

    ssize_t bytes_sent;
    // ErrorHandling: Handle undersends
    bytes_sent = sendto(this->m_sockfd, this->m_dmx0, DMX_LEN, 0,
                        (struct sockaddr*)&this->m_addr0, sizeof(this->m_addr0));
    if (bytes_sent != DMX_LEN) {
        std::cerr << "WARNING addr0 bytes_sent: " << bytes_sent;
        std::cerr << "\t errno " << errno << " \t" << strerror(errno) << std::endl;
    }


    bytes_sent = sendto(this->m_sockfd, this->m_dmx1, DMX_LEN, 0,
                        (struct sockaddr*)&this->m_addr1, sizeof(this->m_addr1));
    if (bytes_sent != DMX_LEN) {
        std::cerr << "WARNING addr1 bytes_sent: " << bytes_sent;
        std::cerr << "\t errno " << errno << " \t" << strerror(errno) << std::endl;
    }
}

unsigned char Device::COLOR_KINETICS_HEADER[21] = {
        0x04, 0x01, 0xdc, 0x4a,
        0x01, 0x00, 0x01, 0x01,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff,
        0x00};
