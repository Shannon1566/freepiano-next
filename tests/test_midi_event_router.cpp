#include <QtTest/QtTest>

#include "midi/MidiEventRouter.h"

class TestMidiEventRouter : public QObject
{
    Q_OBJECT

private slots:
    void ignoresDuplicateNoteOn();
    void emitsAllNotesOffForPanic();
};

void TestMidiEventRouter::ignoresDuplicateNoteOn()
{
    MidiEventRouter router;
    router.noteOn(60, 100, InputSource::Keyboard);
    router.noteOn(60, 100, InputSource::Keyboard);

    const QVector<MidiEvent> events = router.takePendingEvents();
    QCOMPARE(events.size(), 1);
    QCOMPARE(events.at(0).status, static_cast<std::uint8_t>(0x90));
    QCOMPARE(events.at(0).data1, static_cast<std::uint8_t>(60));
}

void TestMidiEventRouter::emitsAllNotesOffForPanic()
{
    MidiEventRouter router;
    router.noteOn(60, 100, InputSource::Keyboard);
    router.takePendingEvents();

    router.allNotesOff(InputSource::System);
    const QVector<MidiEvent> events = router.takePendingEvents();

    QVERIFY(events.size() >= 2);
    QCOMPARE(events.at(0).status, static_cast<std::uint8_t>(0x80));
    QCOMPARE(events.last().status, static_cast<std::uint8_t>(0xB0));
    QCOMPARE(events.last().data1, static_cast<std::uint8_t>(123));
}

QTEST_MAIN(TestMidiEventRouter)

#include "test_midi_event_router.moc"
