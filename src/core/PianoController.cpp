#include "core/PianoController.h"

#include <QtGlobal>

PianoController::PianoController(QObject *parent)
    : QObject(parent)
{
    connect(&m_audioEngine, &WasapiAudioEngine::runningChanged, this, &PianoController::audioRunningChanged);
    connect(&m_audioEngine, &WasapiAudioEngine::statusTextChanged, this, &PianoController::updateStatusText);
    connect(&m_vst3Host, &Vst3Host::instrumentLoadedChanged, this, &PianoController::instrumentLoadedChanged);
    connect(&m_vst3Host, &Vst3Host::statusTextChanged, this, &PianoController::updateStatusText);

    m_audioEngine.setVst3Host(&m_vst3Host);
    m_audioEngine.start();
    loadDefaultInstrument();
    updateStatusText();
}

bool PianoController::audioRunning() const
{
    return m_audioEngine.isRunning();
}

bool PianoController::instrumentLoaded() const
{
    return m_vst3Host.isInstrumentLoaded();
}

QString PianoController::statusText() const
{
    return m_statusText;
}

QObject *PianoController::keyboardMapper()
{
    return &m_keyboardMapper;
}

void PianoController::noteOn(int note, int velocity)
{
    m_router.noteOn(note + m_octaveShift, velocity, InputSource::Keyboard);
    flushMidiEvents();
}

void PianoController::noteOff(int note)
{
    m_router.noteOff(note + m_octaveShift, InputSource::Keyboard);
    flushMidiEvents();
}

void PianoController::setSustain(bool enabled)
{
    m_router.sustain(enabled, InputSource::Keyboard);
    flushMidiEvents();
}

void PianoController::setOctaveShift(int semitones)
{
    const int clamped = qBound(-24, semitones, 24);
    if (m_octaveShift == clamped) {
        return;
    }

    panic();
    m_octaveShift = clamped;
    updateStatusText();
}

void PianoController::panic()
{
    m_router.allNotesOff(InputSource::System);
    flushMidiEvents();
}

void PianoController::loadDefaultInstrument()
{
    m_vst3Host.loadDefaultInstrument();
    updateStatusText();
}

void PianoController::flushMidiEvents()
{
    const QVector<MidiEvent> events = m_router.takePendingEvents();
    if (events.isEmpty()) {
        return;
    }

    m_vst3Host.processEvents(events);
    m_audioEngine.submitEvents(events);
}

void PianoController::updateStatusText()
{
    const QString text = QStringLiteral("%1 | %2 | Octave shift: %3")
                             .arg(m_audioEngine.statusText(), m_vst3Host.statusText())
                             .arg(m_octaveShift);
    if (m_statusText == text) {
        return;
    }

    m_statusText = text;
    emit statusTextChanged();
}
