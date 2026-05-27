import QtQuick
import FreePiano

Rectangle {
    id: fpnRoot

    required property int fpnNote
    required property bool fpnBlackKey
    property string fpnLabel: ""
    property bool fpnKeyboardPressed: false
    property bool fpnMousePressed: false
    readonly property bool fpnPressed: fpnMousePressed || fpnKeyboardPressed

    signal fpnKeyPressed(int fpnNote)
    signal fpnKeyReleased(int fpnNote)

    radius: fpnBlackKey ? FpnTheme.fpnRadiusSmall : FpnTheme.fpnRadiusMedium
    color: fpnPressed ? (fpnBlackKey ? "#334155" : FpnTheme.fpnAccentSoftColor) : (fpnBlackKey ? "#111827" : FpnTheme.fpnPanelAltColor)
    border.color: fpnBlackKey ? "#020617" : FpnTheme.fpnStrongBorderColor
    border.width: 1

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: fpnRoot.fpnBlackKey ? 8 : 12
        text: fpnRoot.fpnLabel
        color: fpnRoot.fpnBlackKey ? "#e5e7eb" : FpnTheme.fpnMutedTextColor
        font.pixelSize: FpnTheme.fpnFontPixelSize(fpnRoot.fpnBlackKey ? 9 : 10)
        horizontalAlignment: Text.AlignHCenter
        visible: text.length > 0
    }

    MouseArea {
        id: fpnMouseArea

        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            fpnRoot.fpnMousePressed = true
            fpnRoot.fpnKeyPressed(fpnRoot.fpnNote)
        }
        onReleased: {
            fpnRoot.fpnMousePressed = false
            fpnRoot.fpnKeyReleased(fpnRoot.fpnNote)
        }
        onCanceled: {
            fpnRoot.fpnMousePressed = false
            fpnRoot.fpnKeyReleased(fpnRoot.fpnNote)
        }
        onExited: {
            if (fpnMouseArea.pressed) {
                fpnRoot.fpnMousePressed = false
                fpnRoot.fpnKeyReleased(fpnRoot.fpnNote)
            }
        }
    }
}
