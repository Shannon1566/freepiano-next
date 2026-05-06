import QtQuick

Rectangle {
    id: root

    required property int note
    required property bool blackKey
    property bool keyboardPressed: false
    property bool mousePressed: false
    readonly property bool pressed: mousePressed || keyboardPressed

    signal keyPressed(int note)
    signal keyReleased(int note)

    radius: blackKey ? 3 : 5
    color: pressed ? (blackKey ? "#334155" : "#dbeafe") : (blackKey ? "#111827" : "#f8fafc")
    border.color: blackKey ? "#020617" : "#94a3b8"
    border.width: 1

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            root.mousePressed = true
            root.keyPressed(root.note)
        }
        onReleased: {
            root.mousePressed = false
            root.keyReleased(root.note)
        }
        onCanceled: {
            root.mousePressed = false
            root.keyReleased(root.note)
        }
        onExited: {
            if (mouseArea.pressed) {
                root.mousePressed = false
                root.keyReleased(root.note)
            }
        }
    }
}
