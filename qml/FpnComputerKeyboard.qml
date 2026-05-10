import QtQuick

Item {
    id: fpnRoot

    property var fpnActiveQtKeys: ({})
    readonly property real fpnUnitWidth: Math.max(34, Math.min(48, width / 23))
    readonly property real fpnKeyHeight: 34
    readonly property var fpnRows: [
        [
            { "fpnLabel": "Esc", "fpnKey": Qt.Key_Escape, "fpnUnits": 1.2 },
            { "fpnLabel": "F1", "fpnKey": Qt.Key_F1, "fpnUnits": 1 },
            { "fpnLabel": "F2", "fpnKey": Qt.Key_F2, "fpnUnits": 1 },
            { "fpnLabel": "F3", "fpnKey": Qt.Key_F3, "fpnUnits": 1 },
            { "fpnLabel": "F4", "fpnKey": Qt.Key_F4, "fpnUnits": 1 },
            { "fpnLabel": "F5", "fpnKey": Qt.Key_F5, "fpnUnits": 1 },
            { "fpnLabel": "F6", "fpnKey": Qt.Key_F6, "fpnUnits": 1 },
            { "fpnLabel": "F7", "fpnKey": Qt.Key_F7, "fpnUnits": 1 },
            { "fpnLabel": "F8", "fpnKey": Qt.Key_F8, "fpnUnits": 1 },
            { "fpnLabel": "F9", "fpnKey": Qt.Key_F9, "fpnUnits": 1 },
            { "fpnLabel": "F10", "fpnKey": Qt.Key_F10, "fpnUnits": 1 },
            { "fpnLabel": "F11", "fpnKey": Qt.Key_F11, "fpnUnits": 1 },
            { "fpnLabel": "F12", "fpnKey": Qt.Key_F12, "fpnUnits": 1 }
        ],
        [
            { "fpnLabel": "`", "fpnKey": Qt.Key_QuoteLeft, "fpnUnits": 1 },
            { "fpnLabel": "1", "fpnKey": Qt.Key_1, "fpnUnits": 1 },
            { "fpnLabel": "2", "fpnKey": Qt.Key_2, "fpnUnits": 1 },
            { "fpnLabel": "3", "fpnKey": Qt.Key_3, "fpnUnits": 1 },
            { "fpnLabel": "4", "fpnKey": Qt.Key_4, "fpnUnits": 1 },
            { "fpnLabel": "5", "fpnKey": Qt.Key_5, "fpnUnits": 1 },
            { "fpnLabel": "6", "fpnKey": Qt.Key_6, "fpnUnits": 1 },
            { "fpnLabel": "7", "fpnKey": Qt.Key_7, "fpnUnits": 1 },
            { "fpnLabel": "8", "fpnKey": Qt.Key_8, "fpnUnits": 1 },
            { "fpnLabel": "9", "fpnKey": Qt.Key_9, "fpnUnits": 1 },
            { "fpnLabel": "0", "fpnKey": Qt.Key_0, "fpnUnits": 1 },
            { "fpnLabel": "-", "fpnKey": Qt.Key_Minus, "fpnUnits": 1 },
            { "fpnLabel": "=", "fpnKey": Qt.Key_Equal, "fpnUnits": 1 },
            { "fpnLabel": "Backspace", "fpnKey": Qt.Key_Backspace, "fpnUnits": 2 }
        ],
        [
            { "fpnLabel": "Tab", "fpnKey": Qt.Key_Tab, "fpnUnits": 1.5 },
            { "fpnLabel": "Q", "fpnKey": Qt.Key_Q, "fpnUnits": 1 },
            { "fpnLabel": "W", "fpnKey": Qt.Key_W, "fpnUnits": 1 },
            { "fpnLabel": "E", "fpnKey": Qt.Key_E, "fpnUnits": 1 },
            { "fpnLabel": "R", "fpnKey": Qt.Key_R, "fpnUnits": 1 },
            { "fpnLabel": "T", "fpnKey": Qt.Key_T, "fpnUnits": 1 },
            { "fpnLabel": "Y", "fpnKey": Qt.Key_Y, "fpnUnits": 1 },
            { "fpnLabel": "U", "fpnKey": Qt.Key_U, "fpnUnits": 1 },
            { "fpnLabel": "I", "fpnKey": Qt.Key_I, "fpnUnits": 1 },
            { "fpnLabel": "O", "fpnKey": Qt.Key_O, "fpnUnits": 1 },
            { "fpnLabel": "P", "fpnKey": Qt.Key_P, "fpnUnits": 1 },
            { "fpnLabel": "[", "fpnKey": Qt.Key_BracketLeft, "fpnUnits": 1 },
            { "fpnLabel": "]", "fpnKey": Qt.Key_BracketRight, "fpnUnits": 1 },
            { "fpnLabel": "\\", "fpnKey": Qt.Key_Backslash, "fpnUnits": 1.5 }
        ],
        [
            { "fpnLabel": "Caps", "fpnKey": Qt.Key_CapsLock, "fpnUnits": 1.75 },
            { "fpnLabel": "A", "fpnKey": Qt.Key_A, "fpnUnits": 1 },
            { "fpnLabel": "S", "fpnKey": Qt.Key_S, "fpnUnits": 1 },
            { "fpnLabel": "D", "fpnKey": Qt.Key_D, "fpnUnits": 1 },
            { "fpnLabel": "F", "fpnKey": Qt.Key_F, "fpnUnits": 1 },
            { "fpnLabel": "G", "fpnKey": Qt.Key_G, "fpnUnits": 1 },
            { "fpnLabel": "H", "fpnKey": Qt.Key_H, "fpnUnits": 1 },
            { "fpnLabel": "J", "fpnKey": Qt.Key_J, "fpnUnits": 1 },
            { "fpnLabel": "K", "fpnKey": Qt.Key_K, "fpnUnits": 1 },
            { "fpnLabel": "L", "fpnKey": Qt.Key_L, "fpnUnits": 1 },
            { "fpnLabel": ";", "fpnKey": Qt.Key_Semicolon, "fpnUnits": 1 },
            { "fpnLabel": "'", "fpnKey": Qt.Key_Apostrophe, "fpnUnits": 1 },
            { "fpnLabel": "Enter", "fpnKey": Qt.Key_Return, "fpnUnits": 2.25 }
        ],
        [
            { "fpnLabel": "Shift", "fpnKey": Qt.Key_Shift, "fpnUnits": 2.25 },
            { "fpnLabel": "Z", "fpnKey": Qt.Key_Z, "fpnUnits": 1 },
            { "fpnLabel": "X", "fpnKey": Qt.Key_X, "fpnUnits": 1 },
            { "fpnLabel": "C", "fpnKey": Qt.Key_C, "fpnUnits": 1 },
            { "fpnLabel": "V", "fpnKey": Qt.Key_V, "fpnUnits": 1 },
            { "fpnLabel": "B", "fpnKey": Qt.Key_B, "fpnUnits": 1 },
            { "fpnLabel": "N", "fpnKey": Qt.Key_N, "fpnUnits": 1 },
            { "fpnLabel": "M", "fpnKey": Qt.Key_M, "fpnUnits": 1 },
            { "fpnLabel": ",", "fpnKey": Qt.Key_Comma, "fpnUnits": 1 },
            { "fpnLabel": ".", "fpnKey": Qt.Key_Period, "fpnUnits": 1 },
            { "fpnLabel": "/", "fpnKey": Qt.Key_Slash, "fpnUnits": 1 },
            { "fpnLabel": "Shift", "fpnKey": Qt.Key_Shift, "fpnUnits": 2.75 },
            { "fpnLabel": "Up", "fpnKey": Qt.Key_Up, "fpnUnits": 1 }
        ],
        [
            { "fpnLabel": "Ctrl", "fpnKey": Qt.Key_Control, "fpnUnits": 1.25 },
            { "fpnLabel": "Win", "fpnKey": Qt.Key_Meta, "fpnUnits": 1.25 },
            { "fpnLabel": "Alt", "fpnKey": Qt.Key_Alt, "fpnUnits": 1.25 },
            { "fpnLabel": "Space", "fpnKey": Qt.Key_Space, "fpnUnits": 6.25 },
            { "fpnLabel": "Alt", "fpnKey": Qt.Key_Alt, "fpnUnits": 1.25 },
            { "fpnLabel": "Menu", "fpnKey": Qt.Key_Menu, "fpnUnits": 1.25 },
            { "fpnLabel": "Ctrl", "fpnKey": Qt.Key_Control, "fpnUnits": 1.25 },
            { "fpnLabel": "Left", "fpnKey": Qt.Key_Left, "fpnUnits": 1 },
            { "fpnLabel": "Down", "fpnKey": Qt.Key_Down, "fpnUnits": 1 },
            { "fpnLabel": "Right", "fpnKey": Qt.Key_Right, "fpnUnits": 1 }
        ]
    ]

    Column {
        anchors.fill: parent
        spacing: 6

        Repeater {
            model: fpnRoot.fpnRows

            Row {
                required property var modelData

                spacing: 6

                Repeater {
                    model: parent.modelData

                    Rectangle {
                        required property var modelData

                        width: fpnRoot.fpnUnitWidth * modelData.fpnUnits
                        height: fpnRoot.fpnKeyHeight
                        radius: 5
                        color: fpnRoot.fpnActiveQtKeys[modelData.fpnKey] === true ? "#bfdbfe" : "#ffffff"
                        border.color: fpnRoot.fpnActiveQtKeys[modelData.fpnKey] === true ? "#2563eb" : "#cbd5e1"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            width: parent.width - 8
                            text: modelData.fpnLabel
                            color: "#0f172a"
                            font.pixelSize: Math.min(12, parent.width / Math.max(1, text.length) * 1.6)
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
