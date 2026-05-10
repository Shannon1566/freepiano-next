import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ApplicationWindow {
    id: fpnWindow

    width: 980
    height: 620
    visible: true
    title: qsTr("freepiano-next")
    color: "#f1f5f9"

    property var fpnActiveKeyboardNotes: ({})
    property var fpnActiveQtKeys: ({})

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

        function fpnSetQtKeyActive(fpnKey, fpnActive) {
            if (fpnActiveQtKeys[fpnKey] === fpnActive) {
                return
            }

            const fpnKeys = ({})
            for (const fpnExistingKey in fpnActiveQtKeys) {
                fpnKeys[fpnExistingKey] = fpnActiveQtKeys[fpnExistingKey]
            }

            if (fpnActive) {
                fpnKeys[fpnKey] = true
            } else {
                delete fpnKeys[fpnKey]
            }
            fpnActiveQtKeys = fpnKeys
        }

        function fpnClearKeyboardNotes() {
            fpnActiveKeyboardNotes = ({})
        }

        function fpnClearQtKeys() {
            fpnActiveQtKeys = ({})
        }

        function fpnClearPressedState() {
            fpnClearKeyboardNotes()
            fpnClearQtKeys()
        }

        Keys.onPressed: fpnEvent => {
            if (fpnEvent.isAutoRepeat) {
                fpnEvent.accepted = true
                return
            }

            fpnSetQtKeyActive(fpnEvent.key, true)

            if (fpnEvent.key === Qt.Key_Space) {
                fpnPianoController.fpnSetSustain(true)
                fpnEvent.accepted = true
                return
            }

            const fpnNote = fpnPianoController.fpnKeyboardMapper.fpnNoteForKey(fpnEvent.key)
            if (fpnNote >= 0 && fpnActiveKeyboardNotes[fpnNote] !== true) {
                fpnSetKeyboardNoteActive(fpnNote, true)
                fpnPianoController.fpnNoteOn(fpnNote, 100)
            }
            fpnEvent.accepted = true
        }
        Keys.onReleased: fpnEvent => {
            if (fpnEvent.isAutoRepeat) {
                fpnEvent.accepted = true
                return
            }

            fpnSetQtKeyActive(fpnEvent.key, false)

            if (fpnEvent.key === Qt.Key_Space) {
                fpnPianoController.fpnSetSustain(false)
                fpnEvent.accepted = true
                return
            }

            const fpnNote = fpnPianoController.fpnKeyboardMapper.fpnNoteForKey(fpnEvent.key)
            if (fpnNote >= 0) {
                fpnSetKeyboardNoteActive(fpnNote, false)
                fpnPianoController.fpnNoteOff(fpnNote)
            }
            fpnEvent.accepted = true
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
                        fpnFocusScope.fpnClearPressedState()
                    }
                }
            }

            FpnPianoKeyboard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 210
                fpnActiveNotes: fpnWindow.fpnActiveKeyboardNotes
                onFpnKeyPressed: fpnNote => fpnPianoController.fpnNoteOn(fpnNote, 100)
                onFpnKeyReleased: fpnNote => fpnPianoController.fpnNoteOff(fpnNote)
            }

            FpnComputerKeyboard {
                Layout.fillWidth: true
                Layout.preferredHeight: 234
                fpnActiveQtKeys: fpnWindow.fpnActiveQtKeys
            }
        }

        Component.onCompleted: forceActiveFocus()
        onActiveFocusChanged: {
            if (!activeFocus) {
                fpnPianoController.fpnPanic()
                fpnClearPressedState()
            }
        }
    }
}
