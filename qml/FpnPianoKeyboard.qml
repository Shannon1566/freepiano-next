import QtQuick
import FreePiano

Item {
    id: fpnRoot

    property var fpnActiveNotes: ({})
    readonly property real fpnPreferredWhiteKeyWidth: 36
    readonly property real fpnWhiteKeyWidth: Math.max(24, Math.min(fpnPreferredWhiteKeyWidth, width / Math.max(1, fpnWhiteKeyCount)))

    signal fpnKeyPressed(int fpnNote)
    signal fpnKeyReleased(int fpnNote)

    readonly property var fpnWhiteNotes: fpnBuildKeys(false)
    readonly property var fpnBlackNotes: fpnBuildKeys(true)
    readonly property int fpnWhiteKeyCount: fpnWhiteNotes.length
    readonly property real fpnBlackKeyWidth: fpnWhiteKeyWidth * 0.62
    implicitWidth: fpnWhiteKeyCount * fpnPreferredWhiteKeyWidth
    implicitHeight: 260

    function fpnIsBlackNote(fpnNote) {
        const fpnPitch = fpnNote % 12
        return fpnPitch === 1 || fpnPitch === 3 || fpnPitch === 6 || fpnPitch === 8 || fpnPitch === 10
    }

    function fpnNoteName(fpnNote) {
        const fpnNames = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
        return fpnNames[fpnNote % 12] + Math.floor(fpnNote / 12 - 1)
    }

    function fpnBuildKeys(fpnBlack) {
        const fpnKeys = []
        let fpnWhiteSlot = 0
        for (let fpnNote = 21; fpnNote <= 108; ++fpnNote) {
            const fpnIsBlack = fpnIsBlackNote(fpnNote)
            if (fpnIsBlack) {
                if (fpnBlack) {
                    fpnKeys.push({
                        "fpnNote": fpnNote,
                        "fpnLabel": fpnNoteName(fpnNote),
                        "fpnSlot": fpnWhiteSlot - 1
                    })
                }
            } else {
                if (!fpnBlack) {
                    fpnKeys.push({
                        "fpnNote": fpnNote,
                        "fpnLabel": fpnNoteName(fpnNote),
                        "fpnSlot": fpnWhiteSlot
                    })
                }
                ++fpnWhiteSlot
            }
        }
        return fpnKeys
    }

    Flickable {
        id: fpnFlickable

        anchors.fill: parent
        contentWidth: fpnRoot.fpnWhiteKeyCount * fpnRoot.fpnWhiteKeyWidth
        contentHeight: height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Row {
            height: fpnFlickable.height
            spacing: 0

            Repeater {
                model: fpnRoot.fpnWhiteNotes

                FpnPianoKey {
                    required property var modelData

                    width: fpnRoot.fpnWhiteKeyWidth
                    height: fpnFlickable.height
                    fpnNote: modelData.fpnNote
                    fpnLabel: modelData.fpnLabel
                    fpnBlackKey: false
                    fpnKeyboardPressed: fpnRoot.fpnActiveNotes[modelData.fpnNote] === true
                    onFpnKeyPressed: fpnNote => fpnRoot.fpnKeyPressed(fpnNote)
                    onFpnKeyReleased: fpnNote => fpnRoot.fpnKeyReleased(fpnNote)
                }
            }
        }

        Repeater {
            model: fpnRoot.fpnBlackNotes

            FpnPianoKey {
                required property var modelData

                x: (modelData.fpnSlot + 0.68) * fpnRoot.fpnWhiteKeyWidth
                y: 0
                width: fpnRoot.fpnBlackKeyWidth
                height: fpnFlickable.height * 0.62
                z: 2
                fpnNote: modelData.fpnNote
                fpnLabel: modelData.fpnLabel
                fpnBlackKey: true
                fpnKeyboardPressed: fpnRoot.fpnActiveNotes[modelData.fpnNote] === true
                onFpnKeyPressed: fpnNote => fpnRoot.fpnKeyPressed(fpnNote)
                onFpnKeyReleased: fpnNote => fpnRoot.fpnKeyReleased(fpnNote)
            }
        }
    }
}
