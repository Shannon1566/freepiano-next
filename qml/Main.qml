import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ApplicationWindow {
    id: fpnWindow

    width: 980
    height: 420
    visible: true
    title: qsTr("freepiano-next")
    color: "#f1f5f9"

    readonly property var fpnWhiteNotes: [60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83]
    readonly property var fpnBlackNotes: [
        { "fpnNote": 61, "fpnSlot": 0 },
        { "fpnNote": 63, "fpnSlot": 1 },
        { "fpnNote": 66, "fpnSlot": 3 },
        { "fpnNote": 68, "fpnSlot": 4 },
        { "fpnNote": 70, "fpnSlot": 5 },
        { "fpnNote": 73, "fpnSlot": 7 },
        { "fpnNote": 75, "fpnSlot": 8 },
        { "fpnNote": 78, "fpnSlot": 10 },
        { "fpnNote": 80, "fpnSlot": 11 },
        { "fpnNote": 82, "fpnSlot": 12 }
    ]
    property var fpnActiveKeyboardNotes: ({})

    FocusScope {
        id: fpnFocusScope

        anchors.fill: parent
        focus: true

        function fpnSetKeyboardNoteActive(fpnNote, fpnActive) {
            if (fpnActiveKeyboardNotes[fpnNote] === fpnActive) {
                return
            }

            const fpnNotes = ({})
            for (const fpnKey in fpnActiveKeyboardNotes) {
                fpnNotes[fpnKey] = fpnActiveKeyboardNotes[fpnKey]
            }

            if (fpnActive) {
                fpnNotes[fpnNote] = true
            } else {
                delete fpnNotes[fpnNote]
            }
            fpnActiveKeyboardNotes = fpnNotes
        }

        function fpnClearKeyboardNotes() {
            fpnActiveKeyboardNotes = ({})
        }

        Keys.onPressed: fpnEvent => {
            if (fpnEvent.isAutoRepeat) {
                fpnEvent.accepted = true
                return
            }

            if (fpnEvent.key === Qt.Key_Space) {
                fpnPianoController.fpnSetSustain(true)
                fpnEvent.accepted = true
                return
            }

            const fpnNote = fpnPianoController.fpnKeyboardMapper.fpnNoteForKey(fpnEvent.key)
            if (fpnNote >= 0 && fpnActiveKeyboardNotes[fpnNote] !== true) {
                fpnSetKeyboardNoteActive(fpnNote, true)
                fpnPianoController.fpnNoteOn(fpnNote, 100)
                fpnEvent.accepted = true
            }
        }
        Keys.onReleased: fpnEvent => {
            if (fpnEvent.isAutoRepeat) {
                fpnEvent.accepted = true
                return
            }

            if (fpnEvent.key === Qt.Key_Space) {
                fpnPianoController.fpnSetSustain(false)
                fpnEvent.accepted = true
                return
            }

            const fpnNote = fpnPianoController.fpnKeyboardMapper.fpnNoteForKey(fpnEvent.key)
            if (fpnNote >= 0) {
                fpnSetKeyboardNoteActive(fpnNote, false)
                fpnPianoController.fpnNoteOff(fpnNote)
                fpnEvent.accepted = true
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
                    text: fpnPianoController.fpnStatusText
                    elide: Text.ElideRight
                    color: "#0f172a"
                }

                ComboBox {
                    id: fpnInstrumentBox

                    Layout.preferredWidth: 240
                    model: fpnPianoController.fpnAvailableInstruments
                    currentIndex: fpnPianoController.fpnCurrentInstrumentIndex
                    textRole: ""
                    enabled: count > 0
                    onActivated: index => {
                        fpnPianoController.fpnLoadInstrument(index)
                        fpnFocusScope.forceActiveFocus()
                    }

                    Connections {
                        target: fpnPianoController
                        function onFpnCurrentInstrumentChanged() {
                            fpnInstrumentBox.currentIndex = fpnPianoController.fpnCurrentInstrumentIndex
                        }
                        function onFpnAvailableInstrumentsChanged() {
                            fpnInstrumentBox.currentIndex = fpnPianoController.fpnCurrentInstrumentIndex
                        }
                    }
                }

                Button {
                    text: qsTr("Refresh")
                    onClicked: {
                        fpnPianoController.fpnRefreshInstruments()
                        fpnFocusScope.forceActiveFocus()
                    }
                }

                Button {
                    text: qsTr("Load")
                    onClicked: fpnPianoController.fpnLoadDefaultInstrument()
                }

                Button {
                    text: qsTr("Panic")
                    onClicked: {
                        fpnPianoController.fpnPanic()
                        fpnFocusScope.fpnClearKeyboardNotes()
                    }
                }
            }

            Item {
                id: fpnPiano

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 240

                readonly property real fpnWhiteKeyWidth: width / fpnWhiteNotes.length

                Row {
                    anchors.fill: parent
                    spacing: 0

                    Repeater {
                        model: fpnWhiteNotes

                        FpnPianoKey {
                            required property int modelData

                            width: fpnPiano.fpnWhiteKeyWidth
                            height: fpnPiano.height
                            fpnNote: modelData
                            fpnBlackKey: false
                            fpnKeyboardPressed: fpnActiveKeyboardNotes[modelData] === true
                            onFpnKeyPressed: fpnNote => fpnPianoController.fpnNoteOn(fpnNote, 100)
                            onFpnKeyReleased: fpnNote => fpnPianoController.fpnNoteOff(fpnNote)
                        }
                    }
                }

                Repeater {
                    model: fpnBlackNotes

                    FpnPianoKey {
                        required property var modelData

                        x: (modelData.fpnSlot + 0.68) * fpnPiano.fpnWhiteKeyWidth
                        y: 0
                        width: fpnPiano.fpnWhiteKeyWidth * 0.62
                        height: fpnPiano.height * 0.62
                        z: 2
                        fpnNote: modelData.fpnNote
                        fpnBlackKey: true
                        fpnKeyboardPressed: fpnActiveKeyboardNotes[modelData.fpnNote] === true
                        onFpnKeyPressed: fpnNote => fpnPianoController.fpnNoteOn(fpnNote, 100)
                        onFpnKeyReleased: fpnNote => fpnPianoController.fpnNoteOff(fpnNote)
                    }
                }
            }
        }

        Component.onCompleted: forceActiveFocus()
        onActiveFocusChanged: {
            if (!activeFocus) {
                fpnPianoController.fpnPanic()
                fpnClearKeyboardNotes()
            }
        }
    }
}
