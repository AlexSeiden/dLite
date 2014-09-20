#include "BeatFiles.h"
#include "Node.h"
#include "Cupid.h"
#include <iostream>
#include <fstream>
#include <sstream>


// TODO master todo here (because it's the first src file.)
/*

 AudioNotify drop

 Hardware interface!
 Saving!
      restore graphwidget positions

 Node Types:
   color nodes:
        palette
        spline
   beats!
        segmentino
        breakdown viewer
   Regions
   Paths

 Grid BG option in graph view

 Compositing modes
 Firing decay modes

 playback controller
      progress bar with clicking
      multiple queued songs

 multiple tabbed cue sheets
 Export/bake
 more Shape rendering with QPainter

global hotkeys:
  space to start/stop
  frame all / frame selected  (Still a little funky)
  rewind

 compmode & decaymode pups on cuewidgets

Cleaning:
    Remnants of original spectrum audio recording stuff in bufferlength and
    windowchanged things
    Clean up object model & separation-of-concerns

Bugs:

*/

// ------------------------------------------------------------------------------
//  TriggerEvery
//      Triggers every _interval milliseconds

TriggerEvery::TriggerEvery() :
    _output(true),
    _interval(500)
{
    setName("TriggerEvery");
    _type = BEAT;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _interval.setName("interval");
    _interval.setOutput(false);
    _interval.setConnectable(false);

    // Set refresh to current time, so when we make a new one in the middle
    // of playing, it won't have to catch up.
    _nextRefresh = Cupid::getPlaybackPositionUSecs();
    _paramList << &_output << &_interval;
    setParamParent();
}

void TriggerEvery::operator()()
{
    // Only run any operator once per frame:
    if (evaluatedThisFrame())
        return;

    // Automatically reset after triggering.
    _output._value = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    if (mSecs > _nextRefresh) {
       _output._value = true;
       _nextRefresh += _interval._value;
    }

    _output._qvOutput.setValue(_output._value);
}


// ------------------------------------------------------------------------------
// NodeOnset
//      Loads "onset" info from precomputed file.
//      This node will generate an impulse (bool 'true')
//      for one frame after each onset.

NodeOnset::NodeOnset()
{
    setName("NodeOnset");
    _type = BEAT;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _offset.setName("offset");
    _offset.setOutput(false);
    _offset.setConnectable(false);

    loadOnsetFile();  // ErrorHandling
    // Do we want to make this some kind of singleton???
    // I guess it'd be useful to have muliple copies with different offsets.
    // But that seems rare....maybe there should be a "BeatDelay" node?

    _nextIndex = 0;
    _nextRefresh = 0;

    _paramList << &_output << &_offset;
    setParamParent();
}

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


void NodeOnset::loadOnsetFile()
{
    // Guess filename from audio file:
    QString audioFilename = Cupid::getAudioFilename();
    QFileInfo finfo = QFileInfo(audioFilename);
    QString path = finfo.path();
    QString basename = finfo.completeBaseName();
    QString onsetFilename = path + "/" + basename + "_vamp_vamp-aubio_aubioonset_onsets.csv";
    loadOnsetFile(onsetFilename);
}

void NodeOnset::loadOnsetFile(QString filename)
{
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

    // Automatically reset after triggering.
    _output._value = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    if (mSecs > _nextRefresh) {
        qDebug() << mSecs << _nextRefresh;
       _output._value = true;
       _nextRefresh = _onsets[++_nextIndex];
    }

    // Boilerplate end of operator:
    _output._qvOutput = _output._value;
}

// ------------------------------------------------------------------------------
// NodeBar
//      Loads bar (measure) info from precomputed file.

NodeBar::NodeBar()
{
    setName("NodeBar");
    _type = BEAT;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _offset.setName("offset");
    _offset.setOutput(false);
    _offset.setConnectable(false);

    loadFile();  // ErrorHandling

    _nextIndex = 0;
    _nextRefresh = 0;

    _paramList << &_output << &_offset;
    setParamParent();
}


//  Bar files look like this:
//        51200,"1"
//        122880,"2"
//        194560,"3"
//        265728,"4"
//        337408,"5"
//        409088,"6"
//        480256,"7"
//        551936,"8"
//        623616,"9"
//      etc...
//  The first value is the sample number where the given bar starts,
//  the second (in quotes, for no good reason) is the index of the bar.
//  We read the first number and discard the second.
//  Our bar files have names like:
//      Awesome_vamp_qm-vamp-plugins_qm-barbeattracker_bars.csv
//  Where in this case, "Awesome.wav" was the wave file originally processed.

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
    _output._value = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    if (mSecs > _nextRefresh) {
       _output._value = true;
       _nextRefresh = _bars[++_nextIndex];
    }

    // Boilerplate end of operator:
    _output._qvOutput = _output._value;
}

// ------------------------------------------------------------------------------
// NodeBar
//      Loads bar (measure) info from precomputed file.

NodeBarBeat::NodeBarBeat()
{
    setName("NodeBarBeat");
    _type = BEAT;

    _barTriggerOutput.setName("Bar Trigger");
    _barTriggerOutput.setOutput(true);
    _barTriggerOutput.setConnectable(true);

    _beatTriggerOutput.setName("Beat Trigger");
    _beatTriggerOutput.setOutput(true);
    _beatTriggerOutput.setConnectable(true);

    _barNumberOutput.setName("Bar Number");
    _barNumberOutput.setOutput(true);
    _barNumberOutput.setConnectable(true);

    _beatNumberOutput.setName("Beat Number");
    _beatNumberOutput.setOutput(true);
    _beatNumberOutput.setConnectable(true);

    _offset.setName("offset");
    _offset.setOutput(false);
    _offset.setConnectable(false);

    loadFile();  // ErrorHandling

    _nextIndex = 0;
    _nextRefresh = 0;

    _paramList << &_barNumberOutput << &_beatNumberOutput <<
                  &_barTriggerOutput << &_beatTriggerOutput << &_offset;
    setParamParent();
}


//  BarBeat files look like this:
//            15360,3,"3"
//            33280,4,"4"
//            51200,1,"1"
//            69120,2,"2"
//            87040,3,"3"
//            104960,4,"4"
//            122880,1,"1"
//            140800,2,"2"
//            158720,3,"3"
//            176640,4,"4"
//            194560,1,"1"
//      etc...
//  The first value is the sample number where the beat happens,
//  the second is the index of the beat. The third is the same as the second
// (in quotes, for no good reason)
//  We read the first and second columns and discard the third..
//  Our barbeat files have names like:
//      Awesome_vamp_qm-vamp-plugins_qm-barbeattracker_beatcounts.csv
//  Where in this case, "Awesome.wav" was the wave file originally processed.

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
        int sampleTime = std::atoi(token.c_str());
        _beats.push_back(sampleTime);

        getline(ss, token, ',');
        int beatNumber = std::atoi(token.c_str());
        _beatnumber.push_back(beatNumber);
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
    _barTriggerOutput._value = false;
    _beatTriggerOutput._value = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    if (mSecs > _nextRefresh) {
        _beatTriggerOutput._value = true;
        _beatNumberOutput._value = _beatnumber[_nextIndex];
        if (_beatNumberOutput._value == 1) {
            _barTriggerOutput._value = true;
            _barNumberOutput._value++;
        }
        _nextRefresh = _beats[++_nextIndex];
    }

    // Boilerplate end of operator:
    _barTriggerOutput._qvOutput  = _barTriggerOutput._value;
    _barNumberOutput._qvOutput   = _barNumberOutput._value;
    _beatTriggerOutput._qvOutput = _beatTriggerOutput._value;
    _beatNumberOutput._qvOutput  = _beatNumberOutput._value;
}


// ------------------------------------------------------------------------------
// Utility function
void convertSamplesToMS(std::vector<int> &samples)
{
    // The onset and BarBeats files are indexed based on samples.
    // We use milliseconds.
    double conversion = 1000. / Cupid::Singleton()->getEngine()->format().sampleRate();
    for (auto &sample : samples ) {
        sample *= conversion;
    }
}

// ------------------------------------------------------------------------------
// TODO Segmentino files
//      Loads segment info from precomputed file.

//  Here's a complete segmention file:
//        0,534016,0,"N1"
//        534016,893952,2,"B"
//        1427968,571904,1,"A"
//        1999872,822784,0,"N3"
//        2822656,982528,2,"B"
//        3805184,572928,1,"A"
//        4378112,249856,0,"N5"
//        4627968,500736,3,"C"
//        5128704,643584,0,"N6"
//        5772288,589312,3,"C"
//        6361600,571392,1,"A"
//        6932992,243712,0,"N8"
//      The first and second columns are the start and end (in samples) of each segment.
//      The third column is an int describing the segment; matching values correspond
//      to sonically similar sections.
//      The fourth column is a string.  It seems to correspond to the type of segment,
//      but with some extra info....TODO I need to research a litte more.
//    The suffix is "_vamp_segmentino_segmentino_segmentation.csv"


// ------------------------------------------------------------------------------
// Register
static Registrar<TriggerEvery>  registrar0("TriggerEvery", Node::BEAT);
static Registrar<NodeOnset>     registrar1("Onset", Node::BEAT);
static Registrar<NodeBar>       registrar2("Bar", Node::BEAT);
static Registrar<NodeBarBeat>   registrar3("Bars&&Beats", Node::BEAT);
