# Qt Quick Responsive UI Architecture

## Directory Structure

```text
qml/
  Main.qml                    ApplicationWindow, focus and global keyboard handling
  FpnTheme.qml                shared spacing, color, breakpoint, and font scale tokens
  FpnControlBar.qml           header toolbar with Qt Quick Controls 2 controls
  FpnResponsiveWorkspace.qml  adaptive desktop/compact layouts with LayoutItemProxy
  FpnSidebar.qml              desktop sidebar and compact navigation bar
  FpnMainContent.qml          main piano and computer-keyboard content area
  FpnPianoKeyboard.qml        responsive piano surface
  FpnPianoKey.qml             piano key delegate
  FpnComputerKeyboard.qml     GridLayout based computer-keyboard visualization
```

## Layout Rules

- Use `ApplicationWindow` as the window root and set `minimumWidth` / `minimumHeight`.
- Use `ColumnLayout`, `RowLayout`, and `GridLayout` for resize-responsive regions.
- Use `Layout.fillWidth`, `Layout.fillHeight`, `Layout.preferredWidth`, and `Layout.preferredHeight` for items whose direct parent is a layout.
- Use `anchors.fill: parent` only inside non-layout containers.
- Do not apply `Scale`, `transform`, or non-uniform x/y scaling to the window or root UI.
- Keep the sidebar width fixed on desktop and move it to a compact navigation row below the breakpoint.
- Let the main content use `Layout.fillWidth` and `Layout.fillHeight` so it consumes remaining space.

## DPI And Font Strategy

Qt 6 enables High DPI scaling by default. The C++ entry point sets:

```cpp
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
```

QML sizes remain device-independent pixels. Fonts use `FpnTheme.fpnFontPixelSize(baseSize)`, which applies one uniform font scale and never stretches text horizontally or vertically.

## Adaptive Layout

`FpnResponsiveWorkspace.qml` keeps the content components stable and switches only the layout hierarchy:

- Wide window: `RowLayout` with fixed sidebar and fill-size main content.
- Compact window: `ColumnLayout` with compact navigation and fill-size main content.

The switch uses `LayoutItemProxy` from `QtQuick.Layouts`, so the same content items can participate in different layout structures without duplicating UI state.
