// Copyright (C) 2014-2016 Alex Seiden 
//
// Encapsulates the dance floor itself:
// Imports the physical layout of the LED lamps, stores that.
// Every frame,  evaluates all the cues, then composites each pixel which is
// connected to an actual lamp, as per the layout.
#include "engine/dancefloor.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>
#include <QString>
#include <QStringList>
#include "views/dance_floor_widget.h"
#include "engine/dispatch.h"
#include "views/graph_widget.h"

Dancefloor::Dancefloor(QObject *parent) :
    QObject(parent),
    m_frame(0),
    m_useAllCues(false)
{
    // _timeSinceLastUpdate is tracked just to see how well the interface is really
    // responding.
    m_timeSinceLastUpdate.start();
}

Dancefloor::~Dancefloor() { }

// ------------------------------------------------------------------------------
bool Dancefloor::ImportLayout(const std::string &layoutCsvFile)
{
    // TODO Import foot-squares from the layout file, as well as lights.
    std::string line;
    std::vector< std::string> lines;
    std::vector< std::vector <std::string> > cells;

    std::ifstream layoutIStream(layoutCsvFile);
    if (! layoutIStream.is_open())
        return false;

    while ( getline (layoutIStream,line, '\r') )
        lines.push_back(line);
    layoutIStream.close();

    m_ysize = lines.size();

    // Break into individual cells
    for (size_t i = 0; i<lines.size(); ++i) {
        // Split each row into cells
        line = lines[i];
        std::istringstream ss(line);
        std::string token;

        std::vector<std::string> row;
        while(getline(ss, token, ',')) {
            row.push_back(token);
        }
        cells.push_back(row);
    }

    // Find longest row, use that for x size.
   size_t rowSize = cells[0].size();
   for (size_t i = 1; i< cells.size(); ++i)
        rowSize = std::max(cells[i].size(),  rowSize);

    m_xsize = rowSize;

    // Alloc the Lights.
    // These store the pixel values, as well as the hardware
    // address of the particular LED.
    size_t arraySize = m_xsize*m_ysize;
    m_lights.resize(arraySize);

    // And fill 'em up
    for (int y=0; y<m_ysize; ++y){
        for (int x=0; x<m_xsize; ++x){
            int index = getIndex(x,y);
            std::string cell = cells[y][x];
            if (cell.size() == 0 || cell.compare("X")==0)
                m_lights[index].m_lightID = 0;
            else {
                int lightID = std::atoi(cell.c_str());
                m_lights[index].m_lightID = lightID;
                int stringNum = lightID / 100 - 1;
                int lightIndex = lightID % 100;

                m_lights[index].m_controllerIndex = stringNum/2;
                m_lights[index].m_lightIndex = lightIndex + stringNum%2*50;
            }
            m_lights[index].m_value = Lightcolor();
        }
    }

    return true;
}

void Dancefloor::printLayout()
{
    // Test routine to print layout on console.
    for (int y=0; y<m_ysize; ++y){
        for (int x=0; x<m_xsize; ++x)
            printf("%4d", m_lights[getIndex(x,y)].m_lightID);
        printf("\n");
    }
}

// ------------------------------------------------------------------------------
// Access
bool Dancefloor::hasPixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= m_xsize || y >= m_ysize)
        return false;

    int index = getIndex(x,y);
    if (m_lights[index].m_lightID == 0)
        return false;
    else
        return true;
}

#ifndef INLINE
int Dancefloor::getIndex(int x, int y)
{
    if (!(x >= 0 && x < m_xsize && y >= 0 && y < m_ysize)) {
        qDebug() << "BOUNDS ERROR: " <<x <<y;
        qDebug() << "MAX SIZE    : " <<m_xsize <<m_ysize;
        return 0;
    }
    Q_ASSERT(x >= 0 && x < m_xsize && y >= 0 && y < m_ysize);
    return m_xsize*y + x;
}
#endif

Lightcolor Dancefloor::getPixel(int x, int y) {
    return m_lights[getIndex(x,y)].m_value;
}

QColor Dancefloor::getQColor(int x, int y) {
    // Convinience method to return light value as a QColor rather than a Lightcolor
    return m_lights[getIndex(x,y)].m_value.toQColor();
}

// ------------------------------------------------------------------------------
// fireLight
//      Called by the cues to set up firings.
void Dancefloor::fireLight(int x, int y, Firing *firing)
{
    // TODO Need to insert the firings correctly into the buffer,
    // so that repeat firings by the same cue do the correct thing.
    // Should check firing->cue, and remove old firings from the same
    // cue, if that is desired behavior.
    Light &light = m_lights[getIndex(x,y)];
    light.m_firings.push_back(firing);
}

// ------------------------------------------------------------------------------
// evaluate
void Dancefloor::evaluate()
{
//    qDebug("Time elapsed: %d ms", _timeSinceLastUpdate.elapsed());
    m_timeSinceLastUpdate.restart();

    // Calls all active cues, which will set new firings.
    evaluateAllCues();

    // For every light, get the firing vector:
    for (auto light = m_lights.begin(); light != m_lights.end(); ++light) {

        // For every firing, apply the decay & composite.
#if 0
        // start with what was in buffer.
        // TODO: Add this as a RenderNode option.
        Lightcolor lightColor(light->_value);
#else
        // start with black
        Lightcolor lightColor(0,0,0);
#endif
        // This starts with the oldest firing;
        // Right now, firings are added to the back of the vector
        auto firing = light->m_firings.begin();
        while (firing != light->m_firings.end())  {
            // Calculate the value of this firing
            bool keep = (*firing)->evaluate();

            // Comp it over the others
            // TODO need UI to set comp modes.
            switch ((*firing)->m_compMode) {
            case OVER:
                lightColor = (*firing)->compOver(lightColor);
                break;
            default:
            case ADD:
                lightColor = (*firing)->compAdd(lightColor);
                break;
            }

            // Remove firing from list if the event is over.
            if (! keep) {
                delete (*firing);
                firing = light->m_firings.erase(firing);
            }
            else
                firing++;
        }
        light->m_value = lightColor;
    }

    // STYLE should this be a signal/slot?
    // Update the UI
    m_dfWidget->update();

    // Send to the hardware
    sendToDevice();


    // This keeps track of the current "frame" that's being evaluated.
    // It's used to make sure certain things are evaluated once--and only
    // once--per frame, as well as allowing triggers to reset correctly.
    // Note that each "frame" will *not* take the exact same amount of time,
    // although they should in general be pretty close.
    m_frame++;
}

void Dancefloor::setUseAllCues(bool status) {
    m_useAllCues = status;
}

void Dancefloor::evaluateAllCues() {
    if (m_useAllCues)
        for (RenderNode *cue : m_cues)
            cue->evaluate();
    else
        for (RenderNode *cue :  Dispatch::Singleton()->getGraphWidget()->getCurrentCues())
            cue->evaluate();
}

void Dancefloor::addRenderNode(RenderNode *cue) {
    m_cues.push_back(cue);
}

void Dancefloor::removeRenderNode(RenderNode *cue) {
    auto cueIter = m_cues.begin();
    while (cueIter != m_cues.end())  {
        // Remove firing from list if the event is over.
        if (*cueIter == cue)
            cueIter = m_cues.erase(cueIter);
        else
            cueIter++;
    }
}

void Dancefloor::sendToDevice()
{
    for (auto light = m_lights.begin(); light != m_lights.end(); ++light) {
        if (light->m_lightID == 0)
            continue;

        unsigned char rgb[3];
        light->m_value.setRGB(rgb);
        m_device.setlight(light->m_controllerIndex, light->m_lightIndex, rgb);
    }
    // Output to network.
    m_device.send();
}

void Dancefloor::setHardwareStatus(bool status)
{
    m_device.setActive(status);
}
