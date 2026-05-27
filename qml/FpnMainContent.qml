import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import FreePiano

Rectangle {
    id: fpnRoot

    required property var fpnActiveKeyboardNotes
    required property var fpnActiveQtKeys
    property bool fpnCompact: false

    signal fpnPianoKeyPressed(int fpnNote)
    signal fpnPianoKeyReleased(int fpnNote)

    color: FpnTheme.fpnPanelColor
    border.color: FpnTheme.fpnBorderColor
    implicitWidth: 720
    implicitHeight: 440

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: FpnTheme.fpnSpaceL
        spacing: FpnTheme.fpnSpaceL

        FpnPianoKeyboard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: fpnRoot.fpnCompact ? 180 : 220
            Layout.preferredHeight: fpnRoot.fpnCompact ? 220 : 300
            fpnActiveNotes: fpnRoot.fpnActiveKeyboardNotes
            onFpnKeyPressed: fpnNote => fpnRoot.fpnPianoKeyPressed(fpnNote)
            onFpnKeyReleased: fpnNote => fpnRoot.fpnPianoKeyReleased(fpnNote)
        }

        FpnComputerKeyboard {
            Layout.fillWidth: true
            Layout.preferredHeight: fpnRoot.fpnCompact ? 190 : 250
            Layout.maximumHeight: fpnRoot.fpnCompact ? 210 : 280
            fpnActiveQtKeys: fpnRoot.fpnActiveQtKeys
        }
    }
}
