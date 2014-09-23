#include <stdio.h>
#include <iostream>
#include "Device.h"
#include <QDebug>

using namespace std;


Device::Device(bool status) :
    _isActive(status)
{
    // Init _dmx buffers to header, plus zeros.
    memset(this->dmx0, 0, DMX_LEN);
    memset(this->dmx1, 0, DMX_LEN);
    memcpy(this->dmx0, this->header, sizeof(this->header));
    memcpy(this->dmx1, this->header, sizeof(this->header));

    if (_isActive)
        this->connect();
}

Device::~Device() { }


//  Set the value of a single light to rgb.
void
Device::setlight(int controllerIndex, int lightIndex, unsigned char *rgb) {
    unsigned char *p;
    if (controllerIndex == 0)
        p = this->dmx0;
    else
        p = this->dmx1;
    p += sizeof(this->header);

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

    this->sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (this->sockfd == -1) {
        // ErrorHandling
        std::cerr << "WARNING socket error: " << errno
                  << " \t" << strerror(errno) << std::endl;
        std::cerr.flush();
        return false;
    }

    addr0.sin_family = AF_INET;
    addr0.sin_port = htons(this->IPPort);
    int result = inet_pton(AF_INET, "10.0.1.21", &addr0.sin_addr);
    if (result != 1) {
        // ErrorHandling
        std::cerr << "WARNING addr0 inet_pton result:" << result << "\terror: " << errno
                  << " \t" << strerror(errno) << std::endl;
        std::cerr.flush();
        return false;
    }

    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(this->IPPort);
    result = inet_pton(AF_INET, "10.0.1.22", &addr1.sin_addr);
    if (result != 1) {
        // ErrorHandling
        std::cerr << "WARNING addr1 inet_pton result:" << result << "\terror: " << errno
                  << " \t" << strerror(errno) << std::endl;
        std::cerr.flush();
        return false;
    }

    return true;
}

void Device::turnOff()
{
    if (! _isActive)
        return;

    //  TODO close port
    _isActive = false;
    return;
}


void Device::turnOn()
{
    if (_isActive)
        return;

    _isActive = connect();
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
    if (! _isActive)
        return;

    //TODO test to see if we're sending too often

    ssize_t bytes_sent;
    // ErrorHandling handle undersends
    bytes_sent = sendto(this->sockfd, this->dmx0, DMX_LEN, 0,
                        (struct sockaddr*)&this->addr0, sizeof(this->addr0));
    if (bytes_sent != DMX_LEN) {
        std::cerr << "WARNING addr0 bytes_sent: " << bytes_sent;
        std::cerr << "\t errno " << errno << " \t" << strerror(errno) << std::endl;
    }


    bytes_sent = sendto(this->sockfd, this->dmx1, DMX_LEN, 0,
                        (struct sockaddr*)&this->addr1, sizeof(this->addr1));
    if (bytes_sent != DMX_LEN) {
        std::cerr << "WARNING addr1 bytes_sent: " << bytes_sent;
        std::cerr << "\t errno " << errno << " \t" << strerror(errno) << std::endl;
    }
}

unsigned char Device::header[21] = {
        0x04, 0x01, 0xdc, 0x4a,
        0x01, 0x00, 0x01, 0x01,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff,
        0x00};
