import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import FreePiano

Rectangle {
    id: fpnRoot

    property bool fpnCompact: false

    color: FpnTheme.fpnPanelAltColor
    border.color: FpnTheme.fpnBorderColor
    implicitWidth: FpnTheme.fpnSidebarWidth
    implicitHeight: fpnCompact ? FpnTheme.fpnCompactSidebarHeight : 360

    readonly property var fpnSections: [
        { "fpnText": qsTr("Play"), "fpnChecked": true },
        { "fpnText": qsTr("Mixer"), "fpnChecked": false },
        { "fpnText": qsTr("Input"), "fpnChecked": false },
        { "fpnText": qsTr("Plug-ins"), "fpnChecked": false }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: FpnTheme.fpnSpaceM
        spacing: FpnTheme.fpnSpaceS
        visible: !fpnRoot.fpnCompact

        Label {
            Layout.fillWidth: true
            text: qsTr("FreePiano")
            color: FpnTheme.fpnTextColor
            font.pixelSize: FpnTheme.fpnFontPixelSize(18)
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Next")
            color: FpnTheme.fpnMutedTextColor
            font.pixelSize: FpnTheme.fpnFontPixelSize(12)
            elide: Text.ElideRight
        }

        Repeater {
            model: fpnRoot.fpnSections

            Button {
                required property var modelData

                Layout.fillWidth: true
                Layout.preferredHeight: 38
                checkable: true
                checked: modelData.fpnChecked
                text: modelData.fpnText
                font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("WASAPI shared")
            color: FpnTheme.fpnMutedTextColor
            font.pixelSize: FpnTheme.fpnFontPixelSize(12)
            elide: Text.ElideRight
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: FpnTheme.fpnSpaceS
        spacing: FpnTheme.fpnSpaceS
        visible: fpnRoot.fpnCompact

        Repeater {
            model: fpnRoot.fpnSections

            Button {
                required property var modelData

                Layout.fillWidth: true
                Layout.preferredHeight: 40
                checkable: true
                checked: modelData.fpnChecked
                text: modelData.fpnText
                font.pixelSize: FpnTheme.fpnFontPixelSize(12)
            }
        }
    }
}
