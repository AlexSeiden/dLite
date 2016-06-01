// Copyright (C) 2014-2016 Alex Seiden 
//
#include "engine/segmentation.h"
#include "engine/dispatch.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <iostream>
#include <fstream>
#include <sstream>

SegmentController::SegmentController()
{
    loadFile();
}

// Loads file from "Segmentino" output.
void SegmentController::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Dispatch::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString filename = path + "/" + basename
            + "_vamp_segmentino_segmentino_segmentation.csv";
    loadFile(filename);
}

void SegmentController::loadFile(QString filename)
{
    _segmentation.m_segments.clear();
    _segmentation.m_segmentIndices.clear();

    QSet<int> segmentSet;

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    double conversion = 1000. / Dispatch::Singleton()->getEngine()->format().sampleRate();

    if (! filestream.is_open())
        return; // ErrorHandling

    // XXX This assumes the startTimes are strictly increasing and
    // the durations add up.
    std::string line;
    while ( getline (filestream,line) ) {
        Segment seg;

        // Split line at commas
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');

        // Starting time
        int startTime = std::atoi(token.c_str());
        startTime  *= conversion;
        seg.startTime = startTime;

        // Duration
        getline(ss, token, ',');
        int duration = std::atoi(token.c_str());
        duration  *= conversion;
        seg.duration = duration;

        // Segment "index" -- an integer
        getline(ss, token, ',');
        int index = std::atoi(token.c_str());
        seg.segmentIndex = index;
        segmentSet << index;

        // segment "variant" -- a string
        getline(ss, token);
        QString variant=QString::fromStdString(token);
        variant.remove(QChar('"'));
        seg.segmentVariant = variant;

        _segmentation.m_segments << seg;
    }
    filestream.close();

    // Calculate total duration from last segment start & duration
    _segmentation.m_duration =   _segmentation.m_segments.last().startTime +
                                _segmentation.m_segments.last().duration;

    // Find all indices used by the segmenter.
    _segmentation.m_segmentIndices = segmentSet.values();
    qSort(_segmentation.m_segmentIndices);
    _segmentation.m_nSegments = _segmentation.m_segmentIndices.count();
}


// Loads file from QM College segmenter.
/* They look like this:
0,7,"7"
185220,4,"4"
361620,10,"10"
917280,9,"9"
1102500,6,"6"
1984500,1,"1"
2293200,5,"5"
2628360,3,"3"
2937060,9,"9"
3148740,7,"7"
4057200,6,"6"
4886280,2,"2"
5106780,4,"4"
5583060,9,"9"
Start, segment, "segment"
*/
void SegmentController::loadFileQM(QString filename)
{
    _segmentation.m_segments.clear();
    _segmentation.m_segmentIndices.clear();

    QSet<int> segmentIndexSet;
    QSet<QString> segmentStringSet;

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    double conversion = 1000. / Dispatch::Singleton()->getEngine()->format().sampleRate();

    if (! filestream.is_open())
        return; // ErrorHandling

    // XXX This assumes the startTimes are strictly increasing and
    // the durations add up.
    std::string line;
    while ( getline (filestream,line) ) {
        Segment seg;

        // Split line at commas
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');

        // Starting time
        int startTime = std::atoi(token.c_str());
        startTime  *= conversion;
        seg.startTime = startTime;

        // Segment "index" -- an integer
        getline(ss, token, ',');
        int index = std::atoi(token.c_str());
        seg.segmentIndex = index;
        segmentIndexSet << index;

        // segment "variant" -- a string
        // (for these files, always the same as the integer)
        getline(ss, token);
        QString variant=QString::fromStdString(token);
        variant.remove(QChar('"'));
        seg.segmentVariant = variant;
        segmentStringSet << variant;

        _segmentation.m_segments << seg;
    }
    filestream.close();


    // Calculate total duration from last segment start & duration
    _segmentation.m_duration =    Dispatch::Singleton()->getEngine()->bufferLengthMS();

    // Calculate durations:

    QList<Segment>::iterator seg = _segmentation.m_segments.begin();
    QList<Segment>::iterator end = _segmentation.m_segments.end();
    for (; seg < (end-1); ++seg) {
        seg->duration = (seg+1)->startTime - seg->startTime;
    }
    seg->duration = _segmentation.m_duration - seg->startTime;

    // Find all indices used by the segmenter.
    // (Presumably, will be all integers from 1-|segmentSet|, but you
    // can never be too careful about these things.)
    _segmentation.m_segmentIndices = segmentIndexSet.values();
    qSort(_segmentation.m_segmentIndices);
    _segmentation.m_nSegments = _segmentation.m_segmentIndices.count();
}

// Given a time, finds the segment inside the song that we are currently in.
int SegmentController::findSegment(int msecs)
{
    foreach (Segment seg, _segmentation.m_segments) {
        if (seg.startTime <= msecs && (seg.startTime + seg.duration) >= msecs)
            return seg.segmentIndex;
    }

    return 0;
}

int SegmentController::findSegment() {
    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;
    return findSegment(mSecs);
}
