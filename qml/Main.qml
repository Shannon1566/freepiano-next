import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import FreePiano

ApplicationWindow {
    id: fpnWindow

    minimumWidth: FpnTheme.fpnWindowMinimumWidth
    minimumHeight: FpnTheme.fpnWindowMinimumHeight
    width: 1280
    height: 720
    visible: true
    title: qsTr("freepiano-next")
    color: FpnTheme.fpnWindowColor

    property var fpnActiveKeyboardNotes: ({})
    property var fpnActiveQtKeys: ({})

    readonly property bool fpnCompact: width < FpnTheme.fpnBreakpointCompact

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

        function fpnKeyTokens(fpnEvent) {
            const fpnTokens = []
            if ((fpnEvent.key === Qt.Key_Shift || fpnEvent.key === Qt.Key_Control || fpnEvent.key === Qt.Key_Alt || fpnEvent.key === Qt.Key_AltGr)
                    && fpnEvent.nativeScanCode > 0) {
                fpnTokens.push("scan:" + fpnEvent.nativeScanCode)
            }

            const fpnPrefix = (fpnEvent.modifiers & Qt.KeypadModifier) ? "numpad:" : "main:"
            fpnTokens.push(fpnPrefix + fpnEvent.key)
            return fpnTokens
        }

        function fpnSetQtKeyTokensActive(fpnTokens, fpnActive) {
            for (const fpnToken of fpnTokens) {
                fpnSetQtKeyActive(fpnToken, fpnActive)
            }
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

            fpnSetQtKeyTokensActive(fpnKeyTokens(fpnEvent), true)

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

            fpnSetQtKeyTokensActive(fpnKeyTokens(fpnEvent), false)

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
            anchors.margins: FpnTheme.fpnSpaceL
            spacing: FpnTheme.fpnSpaceL

            FpnControlBar {
                Layout.fillWidth: true
                Layout.preferredHeight: FpnTheme.fpnHeaderHeight
                fpnController: fpnPianoController
                fpnCompact: fpnWindow.fpnCompact
                onFpnActionFinished: fpnFocusScope.forceActiveFocus()
                onFpnPanicRequested: {
                    fpnPianoController.fpnPanic()
                    fpnFocusScope.fpnClearPressedState()
                    fpnFocusScope.forceActiveFocus()
                }
            }

            FpnResponsiveWorkspace {
                Layout.fillWidth: true
                Layout.fillHeight: true
                fpnActiveKeyboardNotes: fpnWindow.fpnActiveKeyboardNotes
                fpnActiveQtKeys: fpnWindow.fpnActiveQtKeys
                onFpnPianoKeyPressed: fpnNote => fpnPianoController.fpnNoteOn(fpnNote, 100)
                onFpnPianoKeyReleased: fpnNote => fpnPianoController.fpnNoteOff(fpnNote)
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
