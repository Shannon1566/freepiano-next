import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 980
    height: 420
    visible: true
    title: qsTr("freepiano-next")
    color: "#f1f5f9"

    readonly property var whiteNotes: [60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83]
    readonly property var blackNotes: [
        { "note": 61, "slot": 0 },
        { "note": 63, "slot": 1 },
        { "note": 66, "slot": 3 },
        { "note": 68, "slot": 4 },
        { "note": 70, "slot": 5 },
        { "note": 73, "slot": 7 },
        { "note": 75, "slot": 8 },
        { "note": 78, "slot": 10 },
        { "note": 80, "slot": 11 },
        { "note": 82, "slot": 12 }
    ]
    property var activeKeyboardNotes: ({})

    FocusScope {
        id: focusScope

        anchors.fill: parent
        focus: true

        function setKeyboardNoteActive(note, active) {
            if (activeKeyboardNotes[note] === active) {
                return
            }

            const notes = ({})
            for (const key in activeKeyboardNotes) {
                notes[key] = activeKeyboardNotes[key]
            }

            if (active) {
                notes[note] = true
            } else {
                delete notes[note]
            }
            activeKeyboardNotes = notes
        }

        function clearKeyboardNotes() {
            activeKeyboardNotes = ({})
        }

        Keys.onPressed: event => {
            if (event.isAutoRepeat) {
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Space) {
                pianoController.setSustain(true)
                event.accepted = true
                return
            }

            const note = pianoController.keyboardMapper.noteForKey(event.key)
            if (note >= 0 && activeKeyboardNotes[note] !== true) {
                setKeyboardNoteActive(note, true)
                pianoController.noteOn(note, 100)
                event.accepted = true
            }
        }
        Keys.onReleased: event => {
            if (event.isAutoRepeat) {
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Space) {
                pianoController.setSustain(false)
                event.accepted = true
                return
            }

            const note = pianoController.keyboardMapper.noteForKey(event.key)
            if (note >= 0) {
                setKeyboardNoteActive(note, false)
                pianoController.noteOff(note)
                event.accepted = true
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Label {
                    Layout.fillWidth: true
                    text: pianoController.statusText
                    elide: Text.ElideRight
                    color: "#0f172a"
                }

                ComboBox {
                    id: instrumentBox

                    Layout.preferredWidth: 240
                    model: pianoController.availableInstruments
                    currentIndex: pianoController.currentInstrumentIndex
                    textRole: ""
                    enabled: count > 0
                    onActivated: index => {
                        pianoController.loadInstrument(index)
                        focusScope.forceActiveFocus()
                    }

                    Connections {
                        target: pianoController
                        function onCurrentInstrumentChanged() {
                            instrumentBox.currentIndex = pianoController.currentInstrumentIndex
                        }
                        function onAvailableInstrumentsChanged() {
                            instrumentBox.currentIndex = pianoController.currentInstrumentIndex
                        }
                    }
                }

                Button {
                    text: qsTr("Refresh")
                    onClicked: {
                        pianoController.refreshInstruments()
                        focusScope.forceActiveFocus()
                    }
                }

                Button {
                    text: qsTr("Load")
                    onClicked: pianoController.loadDefaultInstrument()
                }

                Button {
                    text: qsTr("Panic")
                    onClicked: {
                        pianoController.panic()
                        focusScope.clearKeyboardNotes()
                    }
                }
            }

            Item {
                id: piano

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 240

                readonly property real whiteKeyWidth: width / whiteNotes.length

                Row {
                    anchors.fill: parent
                    spacing: 0

                    Repeater {
                        model: whiteNotes

                        PianoKey {
                            required property int modelData

                            width: piano.whiteKeyWidth
                            height: piano.height
                            note: modelData
                            blackKey: false
                            keyboardPressed: activeKeyboardNotes[modelData] === true
                            onKeyPressed: note => pianoController.noteOn(note, 100)
                            onKeyReleased: note => pianoController.noteOff(note)
                        }
                    }
                }

                Repeater {
                    model: blackNotes

                    PianoKey {
                        required property var modelData

                        x: (modelData.slot + 0.68) * piano.whiteKeyWidth
                        y: 0
                        width: piano.whiteKeyWidth * 0.62
                        height: piano.height * 0.62
                        z: 2
                        note: modelData.note
                        blackKey: true
                        keyboardPressed: activeKeyboardNotes[modelData.note] === true
                        onKeyPressed: note => pianoController.noteOn(note, 100)
                        onKeyReleased: note => pianoController.noteOff(note)
                    }
                }
            }
        }

        Component.onCompleted: forceActiveFocus()
        onActiveFocusChanged: {
            if (!activeFocus) {
                pianoController.panic()
                clearKeyboardNotes()
            }
        }
    }
}
