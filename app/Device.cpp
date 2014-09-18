#include <stdio.h>
#include <iostream>
#include "Device.h"

using namespace std;

Device::Device() {
    // Init _dmx buffers to header, plus zeros.
    memset(this->dmx0, 0, DMX_LEN);
    memset(this->dmx1, 0, DMX_LEN);
    memcpy(this->dmx0, this->header, sizeof(this->header));
    memcpy(this->dmx1, this->header, sizeof(this->header));

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


//  Set pixel at x,y to rgb, per the mapping.
void
Device::setpixel(int x, int y, unsigned char *rgb) {
    int lightID = this->remap[x][y];
    if (lightID) {
        int controllerIndex = lightID /100 - 1;
        int lightIndex = lightID % 100;
        this->setlight(controllerIndex, lightIndex, rgb);
    }
}



// ------------------------------------------------------------------------------
// Network stuff

void
Device::connect() {
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

    addr0.sin_family = AF_INET;
    addr0.sin_port = htons(this->IPPort);
    // ErrorHandling
    inet_pton(AF_INET, "10.0.1.21", &addr0.sin_addr);

    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(this->IPPort);
    // ErrorHandling
    inet_pton(AF_INET, "10.0.1.22", &addr1.sin_addr);
}

void
Device::send() {
    ssize_t bytes_sent;
    // ErrorHandling handle undersends
    bytes_sent = sendto(this->sockfd, this->dmx0, DMX_LEN, 0,
                        (struct sockaddr*)&this->addr0, sizeof(this->addr0));
    cout << "addr 0 bytes sent " << bytes_sent << endl;
    bytes_sent = sendto(this->sockfd, this->dmx1, DMX_LEN, 0,
                        (struct sockaddr*)&this->addr1, sizeof(this->addr1));
    cout << "addr 1 bytes sent " << bytes_sent << endl;
}
