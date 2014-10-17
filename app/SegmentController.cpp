#include "SegmentController.h"
#include "Cupid.h"
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
    loadFile();  // ErrorHandling
}

// Loads file from "Segmentino" output.
// TODO code duped from BeatFiles.cpp
void SegmentController::loadFile()
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

void SegmentController::loadFile(QString filename)
{
    _segmentation._segments.clear();
    _segmentation._segmentIndices.clear();

    QSet<int> segmentSet;

    std::ifstream filestream;
    filestream.open(filename.toStdString(), std::ios::in);
    double conversion = 1000. / Cupid::Singleton()->getEngine()->format().sampleRate();

    if (! filestream.is_open())
        return; // ErrorHandling

    // ErrorHandling: This assumes the startTimes are strictly increasing and
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

        _segmentation._segments << seg;
    }
    filestream.close();

    // Calculate total duration from last segment start & duration
    _segmentation._duration =   _segmentation._segments.last().startTime +
                                _segmentation._segments.last().duration;

    // Find all indices used by the segmenter.
    _segmentation._segmentIndices = segmentSet.values();
    qSort(_segmentation._segmentIndices);
}

// Given a time, finds the segment inside the song that we are currently in.
int SegmentController::findSegment(int msecs)
{
    foreach (Segment seg, _segmentation._segments) {
        if (seg.startTime <= msecs && (seg.startTime + seg.duration) >= msecs)
            return seg.segmentIndex;
    }

    // ErrorHandling
    return 0;
}

int SegmentController::findSegment() {
    qint64 mSecs =  Cupid::getPlaybackPositionUSecs() / 1000;
    return findSegment(mSecs);
}


// ------------------------------------------------------------------------------
// Seg controller gui

#if 0
SegGui::SegGui(SegmentController *segcont, QWidget *parent) :
    QWidget(parent),
    _sc(segcont)
{
    QGridLayout *grid = new QGridLayout(this);
    QFrame *bigframe = new QFrame;
    QVBoxLayout *vlay = new QVBoxLayout;
//    QCheckBox* useAllCues = new QCheckBox;
//    useAllCues->setEnabled(true);
//    useAllCues->setChecked(true);

//    QLabel *checkboxlabel = new QLabel;
//    checkboxlabel->setText(QString("Use all cues"));

    foreach (int seg, _sc->_segmentation._segmentIndices) {
        QLabel *lab = new QLabel;
        QString labeltext = QString("Segment #%1").arg(seg);
        lab->setText(labeltext);
        QLabel *le = new QLabel(this);
        _indexToLE[seg] = le;

        QSpinBox *spinbox = new QSpinBox;
        _indexToSpinbox[seg] = spinbox;

        QHBoxLayout *hlay = new QHBoxLayout;
        hlay->addWidget(lab);
        hlay->addWidget(le);
        hlay->addWidget(spinbox);

        QFrame *frame = new QFrame;
        frame->setContentsMargins(2,2,2,2);
        frame->setFrameStyle(QFrame::Box);
        frame->setLayout(hlay);

        vlay->addWidget(frame);
    }
    bigframe->setContentsMargins(2,2,2,2);
    bigframe->setFrameStyle(QFrame::Box);
    bigframe->setLayout(vlay);

    grid->addWidget(useAllCues,0,0);
    grid->addWidget(checkboxlabel,0,1);
    grid->addWidget(bigframe,1,0,1,-1);
    setLayout(grid);

    setWindowTitle(tr("Segments"));
    setWindowFlags(Qt::WindowTitleHint  | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    // GROSS !! this is here because the gui is a QObject and Segmentation controller isn't
    CHECKED_CONNECT(Cupid::Singleton()->getEngine(), SIGNAL(playPositionChanged(qint64)),
                    this, SLOT(whatToActivate()));
}
int SegGui::whatToActivate()
{
    int ms = Cupid::getPlaybackPositionUSecs()/1000;
    int segmentIndex = _sc->findSegment(ms);
    QSpinBox *spb = _indexToSpinbox[segmentIndex];
    int cuesheetIndex = spb->value();
    emit setCuesheet(cuesheetIndex);
    return cuesheetIndex;
}

void SegGui::setNumCues(int nCues, )
{
    if (nCues == 0)
        _nCues = 1;
    else
        _nCues = nCues;
    int i = 0;
    foreach (int seg, _sc->_segmentation._segmentIndices) {
        _indexToSpinbox[seg]->setValue(i%_nCues);
        _indexToLE[seg]->setText(i%_nCues);
        i++;
    }
    update();
}
#endif
