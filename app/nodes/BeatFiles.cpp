// Copyright (c) Alex Seiden 2014-2016
//
// ------------------------------------------------------------------------------
//  BeatFiles
//
//      Nodes that read precomputed beat and bar info, and generate triggers
//      based on that.

// ------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>

#include "nodes/BeatFiles.h"
#include "nodes/Node.h"
#include "engine/dispatch.h"
#include "engine/nodefactory.h"

//  utils
int findBeatIndexForSample(int thisSample, std::vector<int> beatvec)
{
    int i=0;
    for (i=1; i<beatvec.size(); ++i) {
        if (beatvec[i]>thisSample)
            break;
    }
    return i-1;
}

int findBeatIndexForSample(int thisSample, std::vector<int> beatvec, int suggestedIndex)
{
    if (suggestedIndex < 0)
        suggestedIndex = 0;
    if (suggestedIndex > beatvec.size()-2)
        suggestedIndex = beatvec.size()-2;
    if (beatvec[suggestedIndex]<thisSample &&
        beatvec[suggestedIndex+1]>thisSample)
        return suggestedIndex;
    return findBeatIndexForSample(thisSample, beatvec);
}

bool checkBeatIndex(int index, int lastSample, std::vector<int> beatvec)
{
    if (index == -1)
        return false;
    if (beatvec[index] >= lastSample)
        return true;
    return false;
}

bool checkBeat(int thisSample, int lastSample, std::vector<int> beatvec)
{
    // Check to see if we've passed a beat:
    // If there is a beat between lastSample and thisSample
    int index = findBeatIndexForSample(thisSample, beatvec);
    return checkBeatIndex(index, lastSample, beatvec);
}

// ------------------------------------------------------------------------------
//  TriggerEvery
//      Triggers every _interval milliseconds

TriggerEvery::TriggerEvery()
{
    setName("TriggerEvery");
    m_type = BEAT;

    addParam<bool>("out", true, true);
    addParam<int>("interval", 500);

    // Set refresh to current time, so when we make a new one in the middle
    // of playing, it won't have to catch up.
    m_nextRefresh = Dispatch::getPlaybackPositionUSecs();
}

void TriggerEvery::operator()()
{
    // Only run any operator once per frame:
    if (evaluatedThisFrame())
        return;

    // Automatically reset after triggering.
    bool out = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;
    if (mSecs > m_nextRefresh) {
       out = true;
       int interval;
       getValue("interval", interval);
       m_nextRefresh += interval;
    }

    setValue("out", out);
}

TriggerEvery* TriggerEvery::clone()
{
    TriggerEvery* lhs = new TriggerEvery;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
//  Multiply
//      Interpolates beats

Multiply::Multiply()
{
    setName("Multiply");
    m_type = BEAT;

    addParam<bool>("out", true, true);
    addParam<int>("multiple", 2, false, false);
    addParam<bool>("input");

    // Set refresh to current time, so when we make a new one in the middle
    // of playing, it won't have to catch up.
    m_nextRefresh = Dispatch::getPlaybackPositionUSecs() / 1000;
}

void Multiply::operator()()
{
    // Only run any operator once per frame:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    // Automatically reset after triggering.
    bool out = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;

    float input;
    int multiple;
    getValue("input", input);
    getValue("multiple", multiple);

    // Input trigger
    if (input) {
        out = true;
        if (m_lastInputBeat != 0) {

            m_delta = mSecs - m_lastInputBeat;
            m_delta /= multiple;
            m_nextRefresh = mSecs + m_delta;
            m_lastInputBeat = mSecs;
        }
        goto done;
    }

    // Interpolated trigger
    if (mSecs > m_nextRefresh) {
       out = true;

       // Find new refresh value
       m_nextRefresh += m_delta;
    }

    done:
    setValue("out", out);
}

Multiply* Multiply::clone()
{
    Multiply* lhs = new Multiply;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
// NodeOnset
//      Loads "onset" info from precomputed file.
//      This node will generate an impulse (bool 'true')
//      for one frame after each onset.

/*
  Onset files look like this:
        15360
        20480
        25088
        29696
        33792
        69632
        87552
        96768
        105472
        123392
      etc....
  Each line gives the sample number where a beat (aka "onset") starts.

  Our onset files have names like:
      Awesome_vamp_vamp-aubio_aubioonset_onsets.csv
  Where in this case, "Awesome.wav" was the wave file originally processed.
  Suffixes for other processing types:
      Awesome_vamp_qm-vamp-plugins_qm-barbeattracker_beatcounts.csv
*/

NodeOnset::NodeOnset()
{
    setName("NodeOnset");
    m_type = BEAT;

    addParam<bool>("out", true, true);

    loadFile();  // ErrorHandling
    // Do we want to make this some kind of singleton???
    // I guess it'd be useful to have muliple copies with different offsets.
    // But that seems rare....maybe there should be a "BeatDelay" node?
}

void NodeOnset::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Dispatch::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString onsetFilename = path + "/" + basename + "_vamp_vamp-aubio_aubioonset_onsets.csv";
    loadFile(onsetFilename);
}

void NodeOnset::loadFile(QString filename)
{
    m_nextIndex = 0;
    m_nextRefresh = 0;
    m_onsets.clear();
    // Onset files are csv.  Each line has the sample number of the next beat.
    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    if (! filestream.is_open()) {
        qDebug() << "ERROR: could not find filename " << filename << Q_FUNC_INFO;
        return;
    }

    std::string line;
    while ( getline (filestream,line) ) {
        int value = std::atoi(line.c_str());        // ErrorHandling
        m_onsets.push_back(value);
    }
    filestream.close();
    convertSamplesToMS(m_onsets);
}

void NodeOnset::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;
    bool out = checkBeat(mSecs, m_lastSample, m_onsets);
    m_lastSample = mSecs;

    setValue("out", out);
}

NodeOnset* NodeOnset::clone()
{
    NodeOnset* lhs = new NodeOnset;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
// NodeBar
/*
    Loads bar (measure) info from precomputed file.

Bar files look like this:
      51200,"1"
      122880,"2"
      194560,"3"
      265728,"4"
      337408,"5"
      409088,"6"
      480256,"7"
      551936,"8"
      623616,"9"
    etc...
The first value is the sample number where the given bar starts,
the second (in quotes, for no good reason) is the index of the bar.
We read the first number and discard the second.
Our bar files have names like:
    Awesome_vamp_qm-vamp-plugins_qm-barbeattracker_bars.csv
*/

NodeBar::NodeBar()
{
    setName("NodeBar");
    m_type = BEAT;

    addParam<bool>("out", true, true);
    loadFile();
}

void NodeBar::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Dispatch::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString filename = path + "/" + basename
            + "_vamp_qm-vamp-plugins_qm-barbeattracker_bars.csv";
    loadFile(filename);
}

void NodeBar::loadFile(QString filename)
{
    m_nextIndex = 0;
    m_nextRefresh = 0;
    m_bars.clear();

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    if (! filestream.is_open())
        return; // ErrorHandling

    std::string line;
    while ( getline (filestream,line) ) {
        // Split at comma
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');

        int value = std::atoi(token.c_str());
        m_bars.push_back(value);
    }
    filestream.close();
    convertSamplesToMS(m_bars);
}

void NodeBar::operator()() {
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    // Automatically reset after triggering.
    bool out = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;
    if (mSecs > m_nextRefresh) {
       out = true;
       m_nextRefresh = m_bars[++m_nextIndex];
    }

    setValue("out", out);
}

NodeBar* NodeBar::clone()
{
    NodeBar* lhs = new NodeBar;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
// NodeBarBeat
//      Loads bar (measure) & beat info from precomputed file.
/*
BarBeat files look like this:
          15360,3,"3"
          33280,4,"4"
          51200,1,"1"
          69120,2,"2"
          87040,3,"3"
          104960,4,"4"
          122880,1,"1"
          140800,2,"2"
          158720,3,"3"
          176640,4,"4"
          194560,1,"1"
    etc...
The first value is the sample number where the beat happens,
the second is the index of the beat. The third is the same as the second
(in quotes, for no good reason)
We read the first and second columns and discard the third..
Our barbeat files have names like:
    Awesome_vamp_qm-vamp-plugins_qm-barbeattracker_beatcounts.csv
*/

NodeBarBeat::NodeBarBeat()
{
    setName("NodeBarBeat");
    m_type = BEAT;

    addParam<bool>("Bar Trigger", true, true);
    addParam<bool>("Beat Trigger", true, true);
    addParam<int>("Bar Number", true, true);
    addParam<int>("Beat Number", true, true);
    addParam<float>("Beat Frac", true, true);
    addParam<float>("Bar Frac", true, true);

    loadFile();
}

void NodeBarBeat::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Dispatch::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString filename = path + "/" + basename
            + "_vamp_qm-vamp-plugins_qm-barbeattracker_beatcounts.csv";
    loadFile(filename);
}

void NodeBarBeat::loadFile(QString filename)
{
    m_beats.clear();
    m_beatnumber.clear();

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    if (! filestream.is_open()) {
        qDebug() << "Can't open" << filename;
        return;
    }

    std::string line;
    int barNumber = 0;
    while ( getline (filestream,line) ) {
        // Split at comma
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');
        int sampleTime = std::atoi(token.c_str());
        m_beats.push_back(sampleTime);

        getline(ss, token, ',');
        int beatNumber = std::atoi(token.c_str());
        m_beatnumber.push_back(beatNumber);

        if (beatNumber == 1)
            barNumber++;
        m_barnumber.push_back(barNumber);
    }
    filestream.close();
    convertSamplesToMS(m_beats);
}

void NodeBarBeat::operator()() {
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    // Automatically reset after triggering.
    bool barTrigger = false;
    bool beatTrigger = false;

    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;

    m_beatIndex = findBeatIndexForSample(mSecs, m_beats, m_beatIndex);
    beatTrigger = checkBeatIndex(m_beatIndex, m_lastSample, m_beats);

    m_lastSample = mSecs;

    int nextbeat = m_beatIndex + 1;
    float beatFrac = 1.0;
    if (nextbeat < m_beats.size())
        beatFrac = (float(mSecs) - m_beats[m_beatIndex])/(float(m_beats[nextbeat]) - m_beats[m_beatIndex]);

    int barStart = findStartOfBar(m_beatIndex);
    int nextBarStart = findStartOfNextBar(m_beatIndex);
    float barFrac = (float(mSecs) - barStart)/(float(nextBarStart) - barStart);

    setValue("Beat Trigger", beatTrigger);
    setValue("Bar Number", m_barnumber[m_beatIndex]);
    setValue("Beat Number", m_beatnumber[m_beatIndex]);
    if (beatTrigger && m_beatnumber[m_beatIndex] == 1)
        barTrigger = true;
    setValue("Bar Trigger", barTrigger);
    setValue("Beat Frac", beatFrac);
    setValue("Bar Frac", barFrac);
}

int NodeBarBeat::findStartOfBar(int beatindex)
{
    int index = beatindex;
    while (index >=0) {
       if (m_beatnumber[index] == 1)
            break;
       index--;
    }
    return m_beats[index];
}

int NodeBarBeat::findStartOfNextBar(int beatindex)
{
    int barnumber = m_barnumber[beatindex];
    int index = beatindex;
    while (index < m_beats.size()) {
       if (m_barnumber[index] != barnumber)
            break;
       index++;
    }
    return m_beats[index];
}

NodeBarBeat* NodeBarBeat::clone()
{
    NodeBarBeat* lhs = new NodeBarBeat;
    cloneHelper(*lhs);
    return lhs;
}


// ------------------------------------------------------------------------------
// Segmentino files
//      Loads segment info from precomputed file.
/*
Here's a complete segmention file:
      0,534016,0,"N1"
      534016,893952,2,"B"
      1427968,571904,1,"A"
      1999872,822784,0,"N3"
      2822656,982528,2,"B"
      3805184,572928,1,"A"
      4378112,249856,0,"N5"
      4627968,500736,3,"C"
      5128704,643584,0,"N6"
      5772288,589312,3,"C"
      6361600,571392,1,"A"
      6932992,243712,0,"N8"
    The first and second columns are the start and duration (in samples) of each segment.
    The third column is an int describing the segment; matching values correspond
    to sonically similar sections.
    The fourth column is a string.  It seems to correspond to the type of segment,
    but with some extra info....TODO I need to research a litte more.
  The suffix is "_vamp_segmentino_segmentino_segmentation.csv"
*/

Segmentino::Segmentino()
{
    setName("Segmentino");
    m_type = BEAT;
    addParam<int>("Segment Index", true, true);
    loadFile();
}

void Segmentino::loadFile()
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

void Segmentino::loadFile(QString filename)
{
    _segments.clear();

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    double conversion = 1000. / Dispatch::Singleton()->getEngine()->format().sampleRate();

    if (! filestream.is_open())
        return; // ErrorHandling

    std::string line;
    while ( getline (filestream,line) ) {
        // Split at comma
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');
        int startTime = std::atoi(token.c_str());
        startTime  *= conversion;

        getline(ss, token, ',');
        int duration = std::atoi(token.c_str());
        Q_UNUSED(duration);
        // We don't use duration; we assume each segment lasts until
        // the start of the next segment.

        getline(ss, token, ',');
        int index = std::atoi(token.c_str());
        _segments.push_back(std::pair<int, int>(startTime, index));

        //TODO read and output segment variant indices
    }
    filestream.close();
}

int Segmentino::findSegment(int msecs)
{
    for (auto i = _segments.begin(); i < _segments.end(); ++i) {
        if ((*i).first > msecs)
            return (*(i-1)).second;
    }
    return 0;
}

void Segmentino::operator()() {
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    qint64 mSecs =  Dispatch::getPlaybackPositionUSecs() / 1000;
    int segmentIndex = findSegment(mSecs);
    setValue("Segment Index", segmentIndex);
}

Segmentino* Segmentino::clone()
{
    Segmentino* lhs = new Segmentino;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
// Utility function

// The onset and BarBeats files are indexed based on samples.
// We use milliseconds.  Convert a vector of from sample-indices to ms.
void convertSamplesToMS(std::vector<int> &samples)
{
    double conversion = 1000. / Dispatch::Singleton()->getEngine()->format().sampleRate();
    for (auto &sample : samples ) {
        sample *= conversion;
    }
}

// ------------------------------------------------------------------------------
// Register
static Registrar<TriggerEvery>  registrar0("TriggerEvery", Node::BEAT);
static Registrar<Multiply>      registrar5("Multiply", Node::BEAT);
static Registrar<NodeOnset>     registrar1("Onset", Node::BEAT);
static Registrar<NodeBar>       registrar2("Bar", Node::BEAT);
static Registrar<NodeBarBeat>   registrar3("Bars&&Beats", Node::BEAT);
static Registrar<Segmentino>    registrar4("Segmentino", Node::BEAT);
