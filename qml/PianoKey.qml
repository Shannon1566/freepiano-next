import QtQuick

Rectangle {
    id: root

    required property int note
    required property bool blackKey
    property bool pressed: false

    signal keyPressed(int note)
    signal keyReleased(int note)

    radius: blackKey ? 3 : 5
    color: pressed ? (blackKey ? "#334155" : "#dbeafe") : (blackKey ? "#111827" : "#f8fafc")
    border.color: blackKey ? "#020617" : "#94a3b8"
    border.width: 1

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            root.pressed = true
            root.keyPressed(root.note)
        }
        onReleased: {
            root.pressed = false
            root.keyReleased(root.note)
        }
        onCanceled: {
            root.pressed = false
            root.keyReleased(root.note)
        }
        onExited: {
            if (pressed) {
                root.pressed = false
                root.keyReleased(root.note)
            }
        }
    }
}
