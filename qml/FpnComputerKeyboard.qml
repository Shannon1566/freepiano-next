import QtQuick
import FreePiano

Item {
    id: fpnRoot

    property var fpnActiveQtKeys: ({})

    readonly property real fpnKeySpacing: FpnTheme.fpnSpaceXs
    readonly property real fpnTotalColumns: 24
    readonly property int fpnTotalRows: 6
    readonly property real fpnUnitWidth: Math.max(26, (width - fpnKeySpacing * (fpnTotalColumns - 1)) / fpnTotalColumns)
    readonly property real fpnKeyHeight: Math.max(28, Math.min(40, (height - fpnKeySpacing * (fpnTotalRows - 1)) / fpnTotalRows))
    implicitWidth: 760
    implicitHeight: fpnKeyHeight * fpnTotalRows + fpnKeySpacing * (fpnTotalRows - 1)

    readonly property var fpnKeys: [
        { "fpnLabel": "Esc", "fpnKey": Qt.Key_Escape, "fpnColumn": 0, "fpnRow": 0, "fpnColumnSpan": 1.2 },
        { "fpnLabel": "F1", "fpnKey": Qt.Key_F1, "fpnColumn": 2, "fpnRow": 0 },
        { "fpnLabel": "F2", "fpnKey": Qt.Key_F2, "fpnColumn": 3, "fpnRow": 0 },
        { "fpnLabel": "F3", "fpnKey": Qt.Key_F3, "fpnColumn": 4, "fpnRow": 0 },
        { "fpnLabel": "F4", "fpnKey": Qt.Key_F4, "fpnColumn": 5, "fpnRow": 0 },
        { "fpnLabel": "F5", "fpnKey": Qt.Key_F5, "fpnColumn": 6.5, "fpnRow": 0 },
        { "fpnLabel": "F6", "fpnKey": Qt.Key_F6, "fpnColumn": 7.5, "fpnRow": 0 },
        { "fpnLabel": "F7", "fpnKey": Qt.Key_F7, "fpnColumn": 8.5, "fpnRow": 0 },
        { "fpnLabel": "F8", "fpnKey": Qt.Key_F8, "fpnColumn": 9.5, "fpnRow": 0 },
        { "fpnLabel": "F9", "fpnKey": Qt.Key_F9, "fpnColumn": 11, "fpnRow": 0 },
        { "fpnLabel": "F10", "fpnKey": Qt.Key_F10, "fpnColumn": 12, "fpnRow": 0 },
        { "fpnLabel": "F11", "fpnKey": Qt.Key_F11, "fpnColumn": 13, "fpnRow": 0 },
        { "fpnLabel": "F12", "fpnKey": Qt.Key_F12, "fpnColumn": 14, "fpnRow": 0 },

        { "fpnLabel": "`", "fpnKey": Qt.Key_QuoteLeft, "fpnColumn": 0, "fpnRow": 1 },
        { "fpnLabel": "1", "fpnKey": Qt.Key_1, "fpnColumn": 1, "fpnRow": 1 },
        { "fpnLabel": "2", "fpnKey": Qt.Key_2, "fpnColumn": 2, "fpnRow": 1 },
        { "fpnLabel": "3", "fpnKey": Qt.Key_3, "fpnColumn": 3, "fpnRow": 1 },
        { "fpnLabel": "4", "fpnKey": Qt.Key_4, "fpnColumn": 4, "fpnRow": 1 },
        { "fpnLabel": "5", "fpnKey": Qt.Key_5, "fpnColumn": 5, "fpnRow": 1 },
        { "fpnLabel": "6", "fpnKey": Qt.Key_6, "fpnColumn": 6, "fpnRow": 1 },
        { "fpnLabel": "7", "fpnKey": Qt.Key_7, "fpnColumn": 7, "fpnRow": 1 },
        { "fpnLabel": "8", "fpnKey": Qt.Key_8, "fpnColumn": 8, "fpnRow": 1 },
        { "fpnLabel": "9", "fpnKey": Qt.Key_9, "fpnColumn": 9, "fpnRow": 1 },
        { "fpnLabel": "0", "fpnKey": Qt.Key_0, "fpnColumn": 10, "fpnRow": 1 },
        { "fpnLabel": "-", "fpnKey": Qt.Key_Minus, "fpnColumn": 11, "fpnRow": 1 },
        { "fpnLabel": "=", "fpnKey": Qt.Key_Equal, "fpnColumn": 12, "fpnRow": 1 },
        { "fpnLabel": "Backspace", "fpnKey": Qt.Key_Backspace, "fpnColumn": 13, "fpnRow": 1, "fpnColumnSpan": 2 },

        { "fpnLabel": "Tab", "fpnKey": Qt.Key_Tab, "fpnColumn": 0, "fpnRow": 2, "fpnColumnSpan": 1.5 },
        { "fpnLabel": "Q", "fpnKey": Qt.Key_Q, "fpnColumn": 1.5, "fpnRow": 2 },
        { "fpnLabel": "W", "fpnKey": Qt.Key_W, "fpnColumn": 2.5, "fpnRow": 2 },
        { "fpnLabel": "E", "fpnKey": Qt.Key_E, "fpnColumn": 3.5, "fpnRow": 2 },
        { "fpnLabel": "R", "fpnKey": Qt.Key_R, "fpnColumn": 4.5, "fpnRow": 2 },
        { "fpnLabel": "T", "fpnKey": Qt.Key_T, "fpnColumn": 5.5, "fpnRow": 2 },
        { "fpnLabel": "Y", "fpnKey": Qt.Key_Y, "fpnColumn": 6.5, "fpnRow": 2 },
        { "fpnLabel": "U", "fpnKey": Qt.Key_U, "fpnColumn": 7.5, "fpnRow": 2 },
        { "fpnLabel": "I", "fpnKey": Qt.Key_I, "fpnColumn": 8.5, "fpnRow": 2 },
        { "fpnLabel": "O", "fpnKey": Qt.Key_O, "fpnColumn": 9.5, "fpnRow": 2 },
        { "fpnLabel": "P", "fpnKey": Qt.Key_P, "fpnColumn": 10.5, "fpnRow": 2 },
        { "fpnLabel": "[", "fpnKey": Qt.Key_BracketLeft, "fpnColumn": 11.5, "fpnRow": 2 },
        { "fpnLabel": "]", "fpnKey": Qt.Key_BracketRight, "fpnColumn": 12.5, "fpnRow": 2 },
        { "fpnLabel": "\\", "fpnKey": Qt.Key_Backslash, "fpnColumn": 13.5, "fpnRow": 2, "fpnColumnSpan": 1.5 },

        { "fpnLabel": "Caps", "fpnKey": Qt.Key_CapsLock, "fpnColumn": 0, "fpnRow": 3, "fpnColumnSpan": 1.75 },
        { "fpnLabel": "A", "fpnKey": Qt.Key_A, "fpnColumn": 1.75, "fpnRow": 3 },
        { "fpnLabel": "S", "fpnKey": Qt.Key_S, "fpnColumn": 2.75, "fpnRow": 3 },
        { "fpnLabel": "D", "fpnKey": Qt.Key_D, "fpnColumn": 3.75, "fpnRow": 3 },
        { "fpnLabel": "F", "fpnKey": Qt.Key_F, "fpnColumn": 4.75, "fpnRow": 3 },
        { "fpnLabel": "G", "fpnKey": Qt.Key_G, "fpnColumn": 5.75, "fpnRow": 3 },
        { "fpnLabel": "H", "fpnKey": Qt.Key_H, "fpnColumn": 6.75, "fpnRow": 3 },
        { "fpnLabel": "J", "fpnKey": Qt.Key_J, "fpnColumn": 7.75, "fpnRow": 3 },
        { "fpnLabel": "K", "fpnKey": Qt.Key_K, "fpnColumn": 8.75, "fpnRow": 3 },
        { "fpnLabel": "L", "fpnKey": Qt.Key_L, "fpnColumn": 9.75, "fpnRow": 3 },
        { "fpnLabel": ";", "fpnKey": Qt.Key_Semicolon, "fpnColumn": 10.75, "fpnRow": 3 },
        { "fpnLabel": "'", "fpnKey": Qt.Key_Apostrophe, "fpnColumn": 11.75, "fpnRow": 3 },
        { "fpnLabel": "Enter", "fpnKey": Qt.Key_Return, "fpnColumn": 12.75, "fpnRow": 3, "fpnColumnSpan": 2.25 },

        { "fpnLabel": "L Shift", "fpnKey": Qt.Key_Shift, "fpnColumn": 0, "fpnRow": 4, "fpnColumnSpan": 2.25, "fpnToken": "scan:42" },
        { "fpnLabel": "Z", "fpnKey": Qt.Key_Z, "fpnColumn": 2.25, "fpnRow": 4 },
        { "fpnLabel": "X", "fpnKey": Qt.Key_X, "fpnColumn": 3.25, "fpnRow": 4 },
        { "fpnLabel": "C", "fpnKey": Qt.Key_C, "fpnColumn": 4.25, "fpnRow": 4 },
        { "fpnLabel": "V", "fpnKey": Qt.Key_V, "fpnColumn": 5.25, "fpnRow": 4 },
        { "fpnLabel": "B", "fpnKey": Qt.Key_B, "fpnColumn": 6.25, "fpnRow": 4 },
        { "fpnLabel": "N", "fpnKey": Qt.Key_N, "fpnColumn": 7.25, "fpnRow": 4 },
        { "fpnLabel": "M", "fpnKey": Qt.Key_M, "fpnColumn": 8.25, "fpnRow": 4 },
        { "fpnLabel": ",", "fpnKey": Qt.Key_Comma, "fpnColumn": 9.25, "fpnRow": 4 },
        { "fpnLabel": ".", "fpnKey": Qt.Key_Period, "fpnColumn": 10.25, "fpnRow": 4 },
        { "fpnLabel": "/", "fpnKey": Qt.Key_Slash, "fpnColumn": 11.25, "fpnRow": 4 },
        { "fpnLabel": "R Shift", "fpnKey": Qt.Key_Shift, "fpnColumn": 12.25, "fpnRow": 4, "fpnColumnSpan": 2.75, "fpnToken": "scan:54" },

        { "fpnLabel": "L Ctrl", "fpnKey": Qt.Key_Control, "fpnColumn": 0, "fpnRow": 5, "fpnColumnSpan": 1.25, "fpnToken": "scan:29" },
        { "fpnLabel": "Win", "fpnKey": Qt.Key_Meta, "fpnColumn": 1.25, "fpnRow": 5, "fpnColumnSpan": 1.25 },
        { "fpnLabel": "L Alt", "fpnKey": Qt.Key_Alt, "fpnColumn": 2.5, "fpnRow": 5, "fpnColumnSpan": 1.25, "fpnToken": "scan:56" },
        { "fpnLabel": "Space", "fpnKey": Qt.Key_Space, "fpnColumn": 3.75, "fpnRow": 5, "fpnColumnSpan": 6.25 },
        { "fpnLabel": "R Alt", "fpnKey": Qt.Key_Alt, "fpnColumn": 10, "fpnRow": 5, "fpnColumnSpan": 1.5, "fpnTokens": ["scan:57400", "scan:312"] },
        { "fpnLabel": "Menu", "fpnKey": Qt.Key_Menu, "fpnColumn": 11.5, "fpnRow": 5, "fpnColumnSpan": 1.5 },
        { "fpnLabel": "R Ctrl", "fpnKey": Qt.Key_Control, "fpnColumn": 13, "fpnRow": 5, "fpnColumnSpan": 2, "fpnTokens": ["scan:57373", "scan:285"] },

        { "fpnLabel": "Ins", "fpnKey": Qt.Key_Insert, "fpnColumn": 16, "fpnRow": 1 },
        { "fpnLabel": "Home", "fpnKey": Qt.Key_Home, "fpnColumn": 17, "fpnRow": 1 },
        { "fpnLabel": "PgUp", "fpnKey": Qt.Key_PageUp, "fpnColumn": 18, "fpnRow": 1 },
        { "fpnLabel": "Del", "fpnKey": Qt.Key_Delete, "fpnColumn": 16, "fpnRow": 2 },
        { "fpnLabel": "End", "fpnKey": Qt.Key_End, "fpnColumn": 17, "fpnRow": 2 },
        { "fpnLabel": "PgDn", "fpnKey": Qt.Key_PageDown, "fpnColumn": 18, "fpnRow": 2 },
        { "fpnLabel": "Up", "fpnKey": Qt.Key_Up, "fpnColumn": 17, "fpnRow": 4 },
        { "fpnLabel": "Left", "fpnKey": Qt.Key_Left, "fpnColumn": 16, "fpnRow": 5 },
        { "fpnLabel": "Down", "fpnKey": Qt.Key_Down, "fpnColumn": 17, "fpnRow": 5 },
        { "fpnLabel": "Right", "fpnKey": Qt.Key_Right, "fpnColumn": 18, "fpnRow": 5 },

        { "fpnLabel": "Num", "fpnKey": Qt.Key_NumLock, "fpnColumn": 20, "fpnRow": 1, "fpnNumpad": true },
        { "fpnLabel": "/", "fpnKey": Qt.Key_Slash, "fpnColumn": 21, "fpnRow": 1, "fpnNumpad": true },
        { "fpnLabel": "*", "fpnKey": Qt.Key_Asterisk, "fpnColumn": 22, "fpnRow": 1, "fpnNumpad": true },
        { "fpnLabel": "-", "fpnKey": Qt.Key_Minus, "fpnColumn": 23, "fpnRow": 1, "fpnNumpad": true },
        { "fpnLabel": "7", "fpnKey": Qt.Key_7, "fpnColumn": 20, "fpnRow": 2, "fpnNumpad": true },
        { "fpnLabel": "8", "fpnKey": Qt.Key_8, "fpnColumn": 21, "fpnRow": 2, "fpnNumpad": true },
        { "fpnLabel": "9", "fpnKey": Qt.Key_9, "fpnColumn": 22, "fpnRow": 2, "fpnNumpad": true },
        { "fpnLabel": "+", "fpnKey": Qt.Key_Plus, "fpnColumn": 23, "fpnRow": 2, "fpnRowSpan": 2, "fpnNumpad": true },
        { "fpnLabel": "4", "fpnKey": Qt.Key_4, "fpnColumn": 20, "fpnRow": 3, "fpnNumpad": true },
        { "fpnLabel": "5", "fpnKey": Qt.Key_5, "fpnColumn": 21, "fpnRow": 3, "fpnNumpad": true },
        { "fpnLabel": "6", "fpnKey": Qt.Key_6, "fpnColumn": 22, "fpnRow": 3, "fpnNumpad": true },
        { "fpnLabel": "1", "fpnKey": Qt.Key_1, "fpnColumn": 20, "fpnRow": 4, "fpnNumpad": true },
        { "fpnLabel": "2", "fpnKey": Qt.Key_2, "fpnColumn": 21, "fpnRow": 4, "fpnNumpad": true },
        { "fpnLabel": "3", "fpnKey": Qt.Key_3, "fpnColumn": 22, "fpnRow": 4, "fpnNumpad": true },
        { "fpnLabel": "Enter", "fpnKey": Qt.Key_Enter, "fpnColumn": 23, "fpnRow": 4, "fpnRowSpan": 2, "fpnNumpad": true },
        { "fpnLabel": "0", "fpnKey": Qt.Key_0, "fpnColumn": 20, "fpnRow": 5, "fpnColumnSpan": 2, "fpnNumpad": true },
        { "fpnLabel": ".", "fpnKey": Qt.Key_Period, "fpnColumn": 22, "fpnRow": 5, "fpnNumpad": true }
    ]

    function fpnToken(fpnKey, fpnNumpad) {
        return (fpnNumpad ? "numpad:" : "main:") + fpnKey
    }

    function fpnPressedAny(fpnKey, fpnNumpad, fpnTokenOverride, fpnTokenOverrides) {
        if (fpnTokenOverrides !== undefined) {
            for (const fpnKeyToken of fpnTokenOverrides) {
                if (fpnRoot.fpnActiveQtKeys[fpnKeyToken] === true) {
                    return true
                }
            }
            return false
        }

        const fpnKeyToken = fpnTokenOverride === undefined ? fpnToken(fpnKey, fpnNumpad === true) : fpnTokenOverride
        return fpnRoot.fpnActiveQtKeys[fpnKeyToken] === true
    }

    Repeater {
        model: fpnRoot.fpnKeys

        Rectangle {
            required property var modelData

            readonly property real fpnColumnSpan: modelData.fpnColumnSpan === undefined ? 1 : modelData.fpnColumnSpan
            readonly property real fpnRowSpan: modelData.fpnRowSpan === undefined ? 1 : modelData.fpnRowSpan
            readonly property bool fpnPressed: fpnRoot.fpnPressedAny(modelData.fpnKey, modelData.fpnNumpad, modelData.fpnToken, modelData.fpnTokens)

            x: modelData.fpnColumn * (fpnRoot.fpnUnitWidth + fpnRoot.fpnKeySpacing)
            y: modelData.fpnRow * (fpnRoot.fpnKeyHeight + fpnRoot.fpnKeySpacing)
            width: fpnRoot.fpnUnitWidth * fpnColumnSpan + fpnRoot.fpnKeySpacing * Math.max(0, fpnColumnSpan - 1)
            height: fpnRoot.fpnKeyHeight * fpnRowSpan + fpnRoot.fpnKeySpacing * Math.max(0, fpnRowSpan - 1)
            radius: FpnTheme.fpnRadiusMedium
            color: fpnPressed ? FpnTheme.fpnAccentSoftColor : FpnTheme.fpnPanelColor
            border.color: fpnPressed ? FpnTheme.fpnAccentColor : FpnTheme.fpnBorderColor
            border.width: 1

            Text {
                anchors.centerIn: parent
                width: Math.max(0, parent.width - FpnTheme.fpnSpaceS)
                text: modelData.fpnLabel
                color: FpnTheme.fpnTextColor
                font.pixelSize: Math.max(FpnTheme.fpnFontPixelSize(10), Math.min(FpnTheme.fpnFontPixelSize(12), parent.width / Math.max(1, text.length) * 1.35))
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }
        }
    }
}
