import QtQuick
import QtQuick.Controls.Basic
import FreePiano

Item {
    id: fpnRoot

    property var fpnActiveQtKeys: ({})
    required property var fpnKeyboardMapper
    property string fpnSelectedToken: ""
    property int fpnSelectedNote: 60
    property real fpnEditorX: 0
    property real fpnEditorY: 0

    readonly property real fpnKeySpacing: FpnTheme.fpnSpaceXs
    readonly property real fpnTotalColumns: 24
    readonly property int fpnTotalRows: 6
    readonly property real fpnUnitWidth: Math.max(26, (width - fpnKeySpacing * (fpnTotalColumns - 1)) / fpnTotalColumns)
    readonly property real fpnKeyHeight: Math.max(28, Math.min(40, (height - fpnKeySpacing * (fpnTotalRows - 1)) / fpnTotalRows))
    readonly property real fpnEditorWidth: Math.min(width - FpnTheme.fpnSpaceL, 360)
    readonly property real fpnEditorHeight: FpnTheme.fpnToolbarControlHeight + FpnTheme.fpnSpaceM * 2
    implicitWidth: 760
    implicitHeight: fpnKeyHeight * fpnTotalRows + fpnKeySpacing * (fpnTotalRows - 1)

    readonly property var fpnLegacyTokens: ({
        "main:16777216": "dik:1", "main:16777264": "dik:59", "main:16777265": "dik:60",
        "main:16777266": "dik:61", "main:16777267": "dik:62", "main:16777268": "dik:63",
        "main:16777269": "dik:64", "main:16777270": "dik:65", "main:16777271": "dik:66",
        "main:16777272": "dik:67", "main:16777273": "dik:68", "main:16777274": "dik:87",
        "main:16777275": "dik:88", "main:96": "dik:41", "main:49": "dik:2",
        "main:50": "dik:3", "main:51": "dik:4", "main:52": "dik:5", "main:53": "dik:6",
        "main:54": "dik:7", "main:55": "dik:8", "main:56": "dik:9", "main:57": "dik:10",
        "main:48": "dik:11", "main:45": "dik:12", "main:61": "dik:13",
        "main:16777219": "dik:14", "main:16777217": "dik:15", "main:81": "dik:16",
        "main:87": "dik:17", "main:69": "dik:18", "main:82": "dik:19", "main:84": "dik:20",
        "main:89": "dik:21", "main:85": "dik:22", "main:73": "dik:23", "main:79": "dik:24",
        "main:80": "dik:25", "main:91": "dik:26", "main:93": "dik:27", "main:92": "dik:43",
        "main:16777252": "dik:58", "main:65": "dik:30", "main:83": "dik:31",
        "main:68": "dik:32", "main:70": "dik:33", "main:71": "dik:34", "main:72": "dik:35",
        "main:74": "dik:36", "main:75": "dik:37", "main:76": "dik:38", "main:59": "dik:39",
        "main:39": "dik:40", "main:16777220": "dik:28", "main:90": "dik:44",
        "main:88": "dik:45", "main:67": "dik:46", "main:86": "dik:47", "main:66": "dik:48",
        "main:78": "dik:49", "main:77": "dik:50", "main:44": "dik:51", "main:46": "dik:52",
        "main:47": "dik:53", "main:16777249": "dik:29", "main:16777250": "dik:56",
        "main:32": "dik:57", "main:16777222": "dik:210", "main:16777232": "dik:199",
        "main:16777238": "dik:201", "main:16777223": "dik:211", "main:16777233": "dik:207",
        "main:16777239": "dik:209", "main:16777235": "dik:200", "main:16777234": "dik:203",
        "main:16777237": "dik:208", "main:16777236": "dik:205", "numpad:16777253": "dik:69",
        "numpad:47": "dik:181", "numpad:42": "dik:55", "numpad:45": "dik:74",
        "numpad:55": "dik:71", "numpad:56": "dik:72", "numpad:57": "dik:73",
        "numpad:43": "dik:78", "numpad:52": "dik:75", "numpad:53": "dik:76",
        "numpad:54": "dik:77", "numpad:49": "dik:79", "numpad:50": "dik:80",
        "numpad:51": "dik:81", "numpad:16777221": "dik:156", "numpad:48": "dik:82",
        "numpad:46": "dik:83"
    })

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

    function fpnKeyToken(fpnModelData) {
        if (fpnModelData.fpnToken !== undefined) {
            return fpnModelData.fpnToken
        }
        if (fpnModelData.fpnTokens !== undefined && fpnModelData.fpnTokens.length > 0) {
            return fpnModelData.fpnTokens[0]
        }
        return fpnLegacyTokens[fpnToken(fpnModelData.fpnKey, fpnModelData.fpnNumpad === true)] || ""
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

    function fpnOpenEditor(fpnToken, fpnKeyX, fpnKeyY, fpnKeyWidth, fpnKeyHeightValue) {
        if (fpnToken.length <= 0) {
            return
        }

        const fpnCurrent = fpnRoot.fpnKeyboardMapper.fpnKeyboardLayout.find(item => item.fpnToken === fpnToken)
        fpnRoot.fpnSelectedToken = fpnToken
        fpnRoot.fpnSelectedNote = fpnCurrent === undefined || fpnCurrent.fpnNote < 0 ? 60 : fpnCurrent.fpnNote

        const fpnMaxX = Math.max(0, fpnRoot.width - fpnRoot.fpnEditorWidth)
        const fpnMaxY = Math.max(0, fpnRoot.height - fpnRoot.fpnEditorHeight)
        const fpnPreferredY = fpnKeyY + fpnKeyHeightValue + FpnTheme.fpnSpaceS
        const fpnFlippedY = fpnKeyY - fpnRoot.fpnEditorHeight - FpnTheme.fpnSpaceS

        fpnRoot.fpnEditorX = Math.max(0, Math.min(fpnKeyX + fpnKeyWidth / 2 - fpnRoot.fpnEditorWidth / 2, fpnMaxX))
        fpnRoot.fpnEditorY = Math.max(0, Math.min(fpnPreferredY + fpnRoot.fpnEditorHeight <= fpnRoot.height ? fpnPreferredY : fpnFlippedY, fpnMaxY))
    }

    Repeater {
        model: fpnRoot.fpnKeys

        Rectangle {
            id: fpnKeyRect

            required property var modelData

            readonly property real fpnColumnSpan: modelData.fpnColumnSpan === undefined ? 1 : modelData.fpnColumnSpan
            readonly property real fpnRowSpan: modelData.fpnRowSpan === undefined ? 1 : modelData.fpnRowSpan
            readonly property string fpnEditToken: fpnRoot.fpnKeyToken(modelData)
            readonly property string fpnBindingLabel: fpnRoot.fpnKeyboardMapper.fpnKeyboardLayout.length >= 0
                    ? fpnRoot.fpnKeyboardMapper.fpnBindingLabelForToken(fpnEditToken) : ""
            readonly property bool fpnPressed: fpnRoot.fpnPressedAny(modelData.fpnKey, modelData.fpnNumpad, fpnEditToken, modelData.fpnTokens)
                    || fpnRoot.fpnActiveQtKeys[fpnEditToken] === true
            readonly property bool fpnSelected: fpnRoot.fpnSelectedToken === fpnEditToken

            x: modelData.fpnColumn * (fpnRoot.fpnUnitWidth + fpnRoot.fpnKeySpacing)
            y: modelData.fpnRow * (fpnRoot.fpnKeyHeight + fpnRoot.fpnKeySpacing)
            width: fpnRoot.fpnUnitWidth * fpnColumnSpan + fpnRoot.fpnKeySpacing * Math.max(0, fpnColumnSpan - 1)
            height: fpnRoot.fpnKeyHeight * fpnRowSpan + fpnRoot.fpnKeySpacing * Math.max(0, fpnRowSpan - 1)
            radius: FpnTheme.fpnRadiusMedium
            color: fpnPressed ? FpnTheme.fpnAccentSoftColor : FpnTheme.fpnPanelColor
            border.color: fpnSelected ? FpnTheme.fpnDangerColor : (fpnPressed ? FpnTheme.fpnAccentColor : FpnTheme.fpnBorderColor)
            border.width: fpnSelected ? 2 : 1

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: mouse => {
                    if (mouse.button !== Qt.RightButton) {
                        return
                    }
                    fpnRoot.fpnOpenEditor(fpnEditToken, fpnKeyRect.x, fpnKeyRect.y, fpnKeyRect.width, fpnKeyRect.height)
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: Math.max(2, FpnTheme.fpnSpaceXs)
                width: Math.max(0, parent.width - FpnTheme.fpnSpaceS)
                text: modelData.fpnLabel
                color: FpnTheme.fpnTextColor
                font.pixelSize: Math.max(FpnTheme.fpnFontPixelSize(10), Math.min(FpnTheme.fpnFontPixelSize(12), parent.width / Math.max(1, text.length) * 1.35))
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: Math.max(2, FpnTheme.fpnSpaceXs)
                width: Math.max(0, parent.width - FpnTheme.fpnSpaceS)
                text: fpnBindingLabel
                color: fpnBindingLabel.length > 0 ? FpnTheme.fpnAccentColor : FpnTheme.fpnMutedTextColor
                font.pixelSize: Math.max(FpnTheme.fpnFontPixelSize(8), Math.min(FpnTheme.fpnFontPixelSize(10), parent.width / Math.max(1, text.length) * 1.25))
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }
        }
    }

    Rectangle {
        id: fpnEditorPanel

        x: fpnRoot.fpnEditorX
        y: fpnRoot.fpnEditorY
        z: 10
        width: fpnRoot.fpnEditorWidth
        height: fpnRoot.fpnEditorHeight
        visible: fpnRoot.fpnSelectedToken.length > 0
        radius: FpnTheme.fpnRadiusMedium
        color: FpnTheme.fpnPanelAltColor
        border.color: FpnTheme.fpnStrongBorderColor

        Row {
            anchors.fill: parent
            anchors.margins: FpnTheme.fpnSpaceM
            spacing: FpnTheme.fpnSpaceS

            Label {
                width: 78
                height: FpnTheme.fpnToolbarControlHeight
                text: fpnRoot.fpnKeyboardMapper.fpnNoteName(fpnRoot.fpnSelectedNote)
                color: FpnTheme.fpnTextColor
                font.pixelSize: FpnTheme.fpnFontPixelSize(13)
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            SpinBox {
                width: 92
                height: FpnTheme.fpnToolbarControlHeight
                from: 0
                to: 127
                value: fpnRoot.fpnSelectedNote
                onValueModified: fpnRoot.fpnSelectedNote = value
            }

            Button {
                width: 72
                height: FpnTheme.fpnToolbarControlHeight
                text: qsTr("Set")
                font.pixelSize: FpnTheme.fpnFontPixelSize(12)
                onClicked: {
                    fpnRoot.fpnKeyboardMapper.fpnSetKeyNoteByToken(fpnRoot.fpnSelectedToken, fpnRoot.fpnSelectedNote)
                    fpnRoot.fpnSelectedToken = ""
                }
            }

            Button {
                width: 72
                height: FpnTheme.fpnToolbarControlHeight
                text: qsTr("Clear")
                font.pixelSize: FpnTheme.fpnFontPixelSize(12)
                onClicked: {
                    fpnRoot.fpnKeyboardMapper.fpnClearKeyBindingByToken(fpnRoot.fpnSelectedToken)
                    fpnRoot.fpnSelectedToken = ""
                }
            }
        }
    }
}
