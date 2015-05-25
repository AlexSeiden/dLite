#include <iostream>
#include <fstream>
#include <sstream>

#include "BeatFiles.h"
#include "Node.h"
#include "Cupid.h"
#include "NodeFactory.h"

// TODO master todo here (because it's the first src file.)
/*
IP: Grouping
    save & restore
    display names
    rename
    group-aware duplicate

IP: multiple tabbed cue sheets
    save only current tab

Node Types:
   color nodes:
        palette  -- improve
        spline
   beats!
        other segmenters
        IP: breakdown viewer & editor
        multiply & divide beats (double/half)
        offsets
        bar float output
   region sequence
   Conversion nodes between float & int
   math nodes
   Paths

Fader

Compositing modes
Firing decay modes

playback controller
    multiple queued songs
    Load dlite file from dir with wave

Export/bake

IP:  more Shape rendering with QPainter

global hotkeys:
  frame all / frame selected  (Still a little funky)

compmode & decaymode pups on cuewidgets

put spectrograph options back in somewhere.

switch between 10 different tabs via control keys

Drawing:
    LOD
        Turn off level meters when small
        longer tangents when small
    drop shadow
    nicer active-mode control widget
    Grid BG option in graph view
    nicer colors
    color-coded paramitems
    color-coded connectors
    param-view transparent bg
    arrows on connectors
    be able to scroll even when current scene is entirely visible.

"_dirty" bit

Cleaning:
    Remnants of original spectrum audio recording stuff in bufferlength and
    windowchanged things
    Clean up object model & separation-of-concerns,
        esp wrt CuesheetView/Scene/GraphWidget
    Get rid of Lightcolor and use QColor

Bugs:
    XXX moving playhead back in time breaks beat nodes. (as well as new song)
    editor widgets should give up focus when enter is hit
    AudioNotify drop
    Potential UUID collisions; should only store UUIDs during
        serialization and deserialization

*/

// ------------------------------------------------------------------------------
//  BeatFiles
//
//      Nodes that read precomputed beat and bar info, and generate triggers
//      based on that.

// ------------------------------------------------------------------------------
//  utils
int findBeatIndexForSample(int thisSample, std::vector<int> beatvec)
{
#if 1
    int i=0;
    for (i=1; i<beatvec.size(); ++i) {
        if (beatvec[i]>thisSample)
            break;
    }
    return i-1;
#else
    // Use binary search to find the index of the closest beat
    // before thisSample.  Return -1 if thisSample is before all beats.
    std::vector<int>::iterator start = beats.begin();
    std::vector<int>::iterator finish = beats.end();
    int index = -1;
    while (start < finish) {
        if ((*start) < thisSample)
            break;
    }
#endif
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
    _type = BEAT;

    addParam<bool>("out", true, true);
    addParam<int>("interval", 500);

    // Set refresh to current time, so when we make a new one in the middle
    // of playing, it won't have to catch up.
    _nextRefresh = Cupid::getPlaybackPositionUSecs();
}

void TriggerEvery::operator()()
{
    // Only run any operator once per frame:
    if (evaluatedThisFrame())
        return;

    // Automatically reset after triggering.
    bool out = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    if (mSecs > _nextRefresh) {
       out = true;
       int interval;
       getValue("interval", interval);
       _nextRefresh += interval;
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
    _type = BEAT;

    addParam<bool>("out", true, true);
    addParam<int>("multiple", 2, false, false);
    addParam<bool>("input");

    // Set refresh to current time, so when we make a new one in the middle
    // of playing, it won't have to catch up.
    _nextRefresh = Cupid::getPlaybackPositionUSecs() / 1000;
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
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;

    float input;
    int multiple;
    getValue("input", input);
    getValue("multiple", multiple);

    // Input trigger
    if (input) {
        out = true;
        if (_lastInputBeat != 0) {

            _delta = mSecs - _lastInputBeat;
            _delta /= multiple;
            _nextRefresh = mSecs + _delta;
            _lastInputBeat = mSecs;
        }
        goto done;
    }

    // Interpolated trigger
    if (mSecs > _nextRefresh) {
       out = true;

       // Find new refresh value
#if 0
       ParamBase* inputParam = _input.connectedParam();
       if (! inputParam) goto done;
       Node* inputNode = inputParam->getParentNode();
       if (! inputNode) goto done;
       int nextInputRefresh = inputNode->_nextRefresh; // XXX this is private, and not available on most nodes.
       int delta = nextInputRefresh - _nextRefresh;
       delta /= multiple;
#endif
       _nextRefresh += _delta;
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
    _type = BEAT;

    addParam<bool>("out", true, true);

    loadFile();  // ErrorHandling
    // Do we want to make this some kind of singleton???
    // I guess it'd be useful to have muliple copies with different offsets.
    // But that seems rare....maybe there should be a "BeatDelay" node?
}

void NodeOnset::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Cupid::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString onsetFilename = path + "/" + basename + "_vamp_vamp-aubio_aubioonset_onsets.csv";
    loadFile(onsetFilename);
}

void NodeOnset::loadFile(QString filename)
{
    _nextIndex = 0;
    _nextRefresh = 0;
    _onsets.clear();
    // Onset files are csv.  Each line has the sample number of the next beat.
    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    if (! filestream.is_open()) {
        qDebug() << "ERROR: could not find filename " << filename << Q_FUNC_INFO;
        return; // ErrorHandling
    }

    std::string line;
    while ( getline (filestream,line) ) {
        int value = std::atoi(line.c_str());        // ErrorHandling
        _onsets.push_back(value);
    }
    filestream.close();
    convertSamplesToMS(_onsets);
}

void NodeOnset::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    bool out = checkBeat(mSecs, _lastSample, _onsets);
    _lastSample = mSecs;

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
    _type = BEAT;

    addParam<bool>("out", true, true);
    loadFile();  // ErrorHandling
}

void NodeBar::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Cupid::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString filename = path + "/" + basename
            + "_vamp_qm-vamp-plugins_qm-barbeattracker_bars.csv";
    loadFile(filename);
}

void NodeBar::loadFile(QString filename)
{
    _nextIndex = 0;
    _nextRefresh = 0;
    _bars.clear();

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
        _bars.push_back(value);
    }
    filestream.close();
    convertSamplesToMS(_bars);
}

void NodeBar::operator()() {
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    // Automatically reset after triggering.
    bool out = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    if (mSecs > _nextRefresh) {
       out = true;
       _nextRefresh = _bars[++_nextIndex];
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
    _type = BEAT;

    addParam<bool>("Bar Trigger", true, true);
    addParam<bool>("Beat Trigger", true, true);
    addParam<int>("Bar Number", true, true);
    addParam<int>("Beat Number", true, true);
    addParam<float>("Beat Frac", true, true);
    addParam<float>("Bar Frac", true, true);

    loadFile();  // ErrorHandling
}

void NodeBarBeat::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Cupid::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString filename = path + "/" + basename
            + "_vamp_qm-vamp-plugins_qm-barbeattracker_beatcounts.csv";
    loadFile(filename);
}

void NodeBarBeat::loadFile(QString filename)
{
    _beats.clear();
    _beatnumber.clear();

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    if (! filestream.is_open()) {
        qDebug() << "Can't open" << filename;
        return; // ErrorHandling
    }

    std::string line;
    int barNumber = 0;
    while ( getline (filestream,line) ) {
        // Split at comma
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');
        int sampleTime = std::atoi(token.c_str());
        _beats.push_back(sampleTime);

        getline(ss, token, ',');
        int beatNumber = std::atoi(token.c_str());
        _beatnumber.push_back(beatNumber);

        if (beatNumber == 1)
            barNumber++;
        _barnumber.push_back(barNumber);
    }
    filestream.close();
    convertSamplesToMS(_beats);
}

void NodeBarBeat::operator()() {
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    // Automatically reset after triggering.
    bool barTrigger = false;
    bool beatTrigger = false;

    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
#if 1
    int oldbi = _beatIndex;
    _beatIndex = findBeatIndexForSample(mSecs, _beats, _beatIndex);
    beatTrigger = checkBeatIndex(_beatIndex, _lastSample, _beats);

//    qDebug() << _lastSample << mSecs << oldbi << _beatIndex << beatTrigger;

    _lastSample = mSecs;
#else
    if ((mSecs - _lastSample)> 300) {
        beatTrigger = true;
        _lastSample = mSecs;
    }
#endif
    int nextbeat = _beatIndex + 1;
    float beatFrac = 1.0;
    if (nextbeat < _beats.size())
        beatFrac = (float(mSecs) - _beats[_beatIndex])/(float(_beats[nextbeat]) - _beats[_beatIndex]);

    int barStart = findStartOfBar(_beatIndex);
    int nextBarStart = findStartOfNextBar(_beatIndex);
    float barFrac = (float(mSecs) - barStart)/(float(nextBarStart) - barStart);

    setValue("Beat Trigger", beatTrigger);
    setValue("Bar Number", _barnumber[_beatIndex]);
    setValue("Beat Number", _beatnumber[_beatIndex]);
    if (beatTrigger && _beatnumber[_beatIndex] == 1)
        barTrigger = true;
    setValue("Bar Trigger", barTrigger);
    setValue("Beat Frac", beatFrac);
    setValue("Bar Frac", barFrac);
}

int NodeBarBeat::findStartOfBar(int beatindex)
{
    int index = beatindex;
    while (index >=0) {
       if (_beatnumber[index] == 1)
            break;
       index--;
    }
    return _beats[index];
}

int NodeBarBeat::findStartOfNextBar(int beatindex)
{
    int barnumber = _barnumber[beatindex];
    int index = beatindex;
    while (index < _beats.size()) {
       if (_barnumber[index] != barnumber)
            break;
       index++;
    }
    return _beats[index];
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
    _type = BEAT;

    addParam<int>("Segment Index", true, true);

    loadFile();  // ErrorHandling
}

void Segmentino::loadFile()
{
    // Guess filename from audio file:
    QString audioFilename = Cupid::getAudioFilename();
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
    double conversion = 1000. / Cupid::Singleton()->getEngine()->format().sampleRate();

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
    // ErrorHandling
    return 0;
}

void Segmentino::operator()() {
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
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
    double conversion = 1000. / Cupid::Singleton()->getEngine()->format().sampleRate();
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
