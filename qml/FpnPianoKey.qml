import QtQuick

Rectangle {
    id: fpnRoot

    required property int fpnNote
    required property bool fpnBlackKey
    property bool fpnKeyboardPressed: false
    property bool fpnMousePressed: false
    readonly property bool fpnPressed: fpnMousePressed || fpnKeyboardPressed

    signal fpnKeyPressed(int fpnNote)
    signal fpnKeyReleased(int fpnNote)

    radius: fpnBlackKey ? 3 : 5
    color: fpnPressed ? (fpnBlackKey ? "#334155" : "#dbeafe") : (fpnBlackKey ? "#111827" : "#f8fafc")
    border.color: fpnBlackKey ? "#020617" : "#94a3b8"
    border.width: 1

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
