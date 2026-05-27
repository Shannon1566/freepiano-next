import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Layouts
import FreePiano

Rectangle {
    id: fpnRoot

    required property var fpnController
    property bool fpnCompact: false

    signal fpnActionFinished()
    signal fpnPanicRequested()

    color: FpnTheme.fpnPanelColor
    border.color: FpnTheme.fpnBorderColor
    implicitHeight: FpnTheme.fpnHeaderHeight

    FileDialog {
        id: fpnLoadMapDialog

        title: qsTr("Load Keyboard Map")
        nameFilters: [qsTr("FreePiano maps (*.map *.cfg)"), qsTr("All files (*)")]
        onAccepted: {
            fpnRoot.fpnController.fpnKeyboardMapper.fpnLoadKeyboardLayout(selectedFile)
            fpnRoot.fpnActionFinished()
        }
    }

    FileDialog {
        id: fpnSaveMapDialog

        title: qsTr("Save Keyboard Map")
        fileMode: FileDialog.SaveFile
        defaultSuffix: "map"
        nameFilters: [qsTr("FreePiano map (*.map)"), qsTr("All files (*)")]
        onAccepted: {
            fpnRoot.fpnController.fpnKeyboardMapper.fpnSaveKeyboardLayout(selectedFile)
            fpnRoot.fpnActionFinished()
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: FpnTheme.fpnSpaceL
        anchors.rightMargin: FpnTheme.fpnSpaceL
        anchors.topMargin: FpnTheme.fpnSpaceM
        anchors.bottomMargin: FpnTheme.fpnSpaceM
        spacing: FpnTheme.fpnSpaceM

        Label {
            Layout.fillWidth: true
            Layout.minimumWidth: 120
            text: fpnRoot.fpnController.fpnStatusText
            elide: Text.ElideRight
            color: FpnTheme.fpnTextColor
            font.pixelSize: FpnTheme.fpnFontPixelSize(14)
            verticalAlignment: Text.AlignVCenter
        }

        ComboBox {
            id: fpnInstrumentBox

            Layout.fillWidth: fpnRoot.fpnCompact
            Layout.preferredWidth: fpnRoot.fpnCompact ? 180 : 260
            Layout.minimumWidth: 150
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            model: fpnRoot.fpnController.fpnAvailableInstruments
            currentIndex: fpnRoot.fpnController.fpnCurrentInstrumentIndex
            textRole: ""
            enabled: count > 0
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)

            onActivated: index => {
                fpnRoot.fpnController.fpnLoadInstrument(index)
                fpnRoot.fpnActionFinished()
            }

            Connections {
                target: fpnRoot.fpnController
                function onFpnCurrentInstrumentChanged() {
                    fpnInstrumentBox.currentIndex = fpnRoot.fpnController.fpnCurrentInstrumentIndex
                }
                function onFpnAvailableInstrumentsChanged() {
                    fpnInstrumentBox.currentIndex = fpnRoot.fpnController.fpnCurrentInstrumentIndex
                }
            }
        }

        Button {
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            text: fpnRoot.fpnCompact ? qsTr("Refresh") : qsTr("Refresh")
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            onClicked: {
                fpnRoot.fpnController.fpnRefreshInstruments()
                fpnRoot.fpnActionFinished()
            }
        }

        Button {
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            text: qsTr("Load")
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            visible: !fpnRoot.fpnCompact
            onClicked: {
                fpnRoot.fpnController.fpnLoadDefaultInstrument()
                fpnRoot.fpnActionFinished()
            }
        }

        Button {
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            text: qsTr("Panic")
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            onClicked: fpnRoot.fpnPanicRequested()
        }

        Button {
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            text: qsTr("Load Map")
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            visible: !fpnRoot.fpnCompact
            onClicked: {
                fpnLoadMapDialog.open()
                fpnRoot.fpnActionFinished()
            }
        }

        Button {
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            text: qsTr("Save Map")
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            visible: !fpnRoot.fpnCompact
            onClicked: {
                fpnSaveMapDialog.open()
                fpnRoot.fpnActionFinished()
            }
        }

        Button {
            Layout.preferredHeight: FpnTheme.fpnToolbarControlHeight
            text: qsTr("Reset Map")
            font.pixelSize: FpnTheme.fpnFontPixelSize(13)
            visible: !fpnRoot.fpnCompact
            onClicked: {
                fpnRoot.fpnController.fpnKeyboardMapper.fpnResetDefaultKeyboardLayout()
                fpnRoot.fpnActionFinished()
            }
        }
    }
}
