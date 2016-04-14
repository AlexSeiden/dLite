#include "mainwidget.h"
#include "Cupid.h"
#include <QFileDialog>

void MainWidget::showLoadSongDialog()
{
    // TODO Load & queue multiple files
    QString dir = "/Users/alex/Documents/WAVS";
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open WAV file"), dir, "*.wav");
    if (fileNames.count()) {
        reset();
        m_engine->loadSong(fileNames.front()); // TODO open multiple files
//        emit updateButtonStates();
        updateMenuStates();
        m_engine->startPlayback();
    }
}

void MainWidget::showSaveDialog()
{
    const QString dir = QDir::homePath(); // TODO better default path
    m_filename = QFileDialog::getSaveFileName(this, tr("Save file"), dir);
    if (! m_filename.isEmpty()) {
        if (! m_filename.endsWith(".dlite"))
            m_filename.append(".dlite");
        NodeFactory::Singleton()->saveToFile(m_filename);
    }
}

void MainWidget::showOpenDialog()
{
    const QString dir = QDir::homePath(); // TODO better default path
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open dLite file"), dir, "*.dLite");
    // TODO open multiple files
    if (! fileName.isEmpty() && ! fileName.isNull()) {
        bool result = NodeFactory::Singleton()->readFromFile(fileName);
        if (result)
            m_filename = fileName;
//        emit updateButtonStates();
        updateMenuStates();
    }
}

void MainWidget::showImportDialog()
{
    const QString dir = QDir::homePath(); // TODO better default path
    const QString fileName =
            QFileDialog::getOpenFileName(this, tr("Import dLite file"),
                                         dir, "*.dLite");

    // TODO allow user to open multiple files at the same time and have
    // them loaded into separate cuesheets.
    if (! fileName.isEmpty() && ! fileName.isNull()) {
        NodeFactory::Singleton()->readFromFile(fileName, true);
        updateMenuStates();
    }
}

void MainWidget::showSettingsDialog()
{
    m_settingsDialog->exec();
    if (m_settingsDialog->result() == QDialog::Accepted) {
        m_engine->setWindowFunction(m_settingsDialog->windowFunction());
        m_engine->setUpdateInterval(m_settingsDialog->interval());
        m_dancefloor->setHardwareStatus(m_settingsDialog->useHardware());
    }
}
