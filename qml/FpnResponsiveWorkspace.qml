import QtQuick
import QtQuick.Layouts
import FreePiano

Item {
    id: fpnRoot

    required property var fpnActiveKeyboardNotes
    required property var fpnActiveQtKeys
    required property var fpnKeyboardMapper

    signal fpnPianoKeyPressed(int fpnNote)
    signal fpnPianoKeyReleased(int fpnNote)

    readonly property bool fpnCompact: width < FpnTheme.fpnBreakpointCompact

    FpnSidebar {
        id: fpnSidebar

        fpnCompact: fpnRoot.fpnCompact
    }

    FpnMainContent {
        id: fpnMainContent

        fpnCompact: fpnRoot.fpnCompact
        fpnActiveKeyboardNotes: fpnRoot.fpnActiveKeyboardNotes
        fpnActiveQtKeys: fpnRoot.fpnActiveQtKeys
        fpnKeyboardMapper: fpnRoot.fpnKeyboardMapper
        onFpnPianoKeyPressed: fpnNote => fpnRoot.fpnPianoKeyPressed(fpnNote)
        onFpnPianoKeyReleased: fpnNote => fpnRoot.fpnPianoKeyReleased(fpnNote)
    }

    RowLayout {
        anchors.fill: parent
        spacing: FpnTheme.fpnSpaceL
        visible: !fpnRoot.fpnCompact

        LayoutItemProxy {
            Layout.fillHeight: true
            Layout.preferredWidth: FpnTheme.fpnSidebarWidth
            Layout.minimumWidth: FpnTheme.fpnSidebarWidth
            Layout.maximumWidth: FpnTheme.fpnSidebarWidth
            target: fpnSidebar
        }

        LayoutItemProxy {
            Layout.fillWidth: true
            Layout.fillHeight: true
            target: fpnMainContent
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: FpnTheme.fpnSpaceM
        visible: fpnRoot.fpnCompact

        LayoutItemProxy {
            Layout.fillWidth: true
            Layout.preferredHeight: FpnTheme.fpnCompactSidebarHeight
            Layout.minimumHeight: FpnTheme.fpnCompactSidebarHeight
            Layout.maximumHeight: FpnTheme.fpnCompactSidebarHeight
            target: fpnSidebar
        }

        LayoutItemProxy {
            Layout.fillWidth: true
            Layout.fillHeight: true
            target: fpnMainContent
        }
    }
}
