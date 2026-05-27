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
    property var fpnPressedKeyNotes: ({})
    property var fpnActiveNoteCounts: ({})

    readonly property bool fpnCompact: width < FpnTheme.fpnBreakpointCompact

    FocusScope {
        id: fpnFocusScope

        anchors.fill: parent
        focus: true

        function fpnSetKeyboardNoteCount(fpnNote, fpnDelta) {
            const fpnNotes = ({})
            for (const fpnKey in fpnActiveKeyboardNotes) {
                fpnNotes[fpnKey] = fpnActiveKeyboardNotes[fpnKey]
            }

            const fpnCounts = ({})
            for (const fpnCountKey in fpnActiveNoteCounts) {
                fpnCounts[fpnCountKey] = fpnActiveNoteCounts[fpnCountKey]
            }

            const fpnNextCount = Math.max(0, (fpnCounts[fpnNote] === undefined ? 0 : fpnCounts[fpnNote]) + fpnDelta)
            if (fpnNextCount > 0) {
                fpnCounts[fpnNote] = fpnNextCount
                fpnNotes[fpnNote] = true
            } else {
                delete fpnCounts[fpnNote]
                delete fpnNotes[fpnNote]
            }
            fpnActiveNoteCounts = fpnCounts
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
            if (fpnEvent.nativeScanCode > 0) {
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
            fpnPressedKeyNotes = ({})
            fpnActiveNoteCounts = ({})
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
            const fpnCanonicalKeyToken = fpnPianoController.fpnKeyboardMapper.fpnKeyTokenForTokens(fpnKeyTokens(fpnEvent))
            if (fpnCanonicalKeyToken.length > 0) {
                fpnSetQtKeyActive(fpnCanonicalKeyToken, true)
            }

            if (fpnEvent.key === Qt.Key_Space) {
                fpnPianoController.fpnSetSustain(true)
                fpnEvent.accepted = true
                return
            }

            const fpnNote = fpnPianoController.fpnKeyboardMapper.fpnNoteForTokens(fpnKeyTokens(fpnEvent))
            if (fpnNote >= 0 && fpnCanonicalKeyToken.length > 0 && fpnPressedKeyNotes[fpnCanonicalKeyToken] === undefined) {
                const fpnPressed = ({})
                for (const fpnPressedKey in fpnPressedKeyNotes) {
                    fpnPressed[fpnPressedKey] = fpnPressedKeyNotes[fpnPressedKey]
                }
                fpnPressed[fpnCanonicalKeyToken] = fpnNote
                fpnPressedKeyNotes = fpnPressed
                fpnSetKeyboardNoteCount(fpnNote, 1)
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
            const fpnCanonicalKeyToken = fpnPianoController.fpnKeyboardMapper.fpnKeyTokenForTokens(fpnKeyTokens(fpnEvent))
            if (fpnCanonicalKeyToken.length > 0) {
                fpnSetQtKeyActive(fpnCanonicalKeyToken, false)
            }

            if (fpnEvent.key === Qt.Key_Space) {
                fpnPianoController.fpnSetSustain(false)
                fpnEvent.accepted = true
                return
            }

            const fpnNote = fpnPressedKeyNotes[fpnCanonicalKeyToken]
            if (fpnNote !== undefined) {
                const fpnPressed = ({})
                for (const fpnPressedKey in fpnPressedKeyNotes) {
                    fpnPressed[fpnPressedKey] = fpnPressedKeyNotes[fpnPressedKey]
                }
                delete fpnPressed[fpnCanonicalKeyToken]
                fpnPressedKeyNotes = fpnPressed
                fpnSetKeyboardNoteCount(fpnNote, -1)
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
                fpnKeyboardMapper: fpnPianoController.fpnKeyboardMapper
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
