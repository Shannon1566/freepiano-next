pragma Singleton

import QtQuick

QtObject {
    id: fpnRoot

    property real fpnUserFontScale: 1.0

    readonly property int fpnWindowMinimumWidth: 760
    readonly property int fpnWindowMinimumHeight: 500
    readonly property int fpnBreakpointCompact: 860
    readonly property int fpnBreakpointWide: 1120

    readonly property int fpnSidebarWidth: 220
    readonly property int fpnCompactSidebarHeight: 56
    readonly property int fpnHeaderHeight: 64
    readonly property int fpnToolbarControlHeight: 36

    readonly property int fpnRadiusSmall: 4
    readonly property int fpnRadiusMedium: 6

    readonly property int fpnSpaceXs: 4
    readonly property int fpnSpaceS: 8
    readonly property int fpnSpaceM: 12
    readonly property int fpnSpaceL: 16
    readonly property int fpnSpaceXl: 24

    readonly property color fpnWindowColor: "#eef2f7"
    readonly property color fpnPanelColor: "#ffffff"
    readonly property color fpnPanelAltColor: "#f8fafc"
    readonly property color fpnBorderColor: "#cbd5e1"
    readonly property color fpnStrongBorderColor: "#94a3b8"
    readonly property color fpnTextColor: "#0f172a"
    readonly property color fpnMutedTextColor: "#475569"
    readonly property color fpnAccentColor: "#2563eb"
    readonly property color fpnAccentSoftColor: "#dbeafe"
    readonly property color fpnDangerColor: "#b91c1c"

    function fpnFontPixelSize(fpnBasePixelSize) {
        return Math.round(fpnBasePixelSize * fpnRoot.fpnUserFontScale)
    }
}
