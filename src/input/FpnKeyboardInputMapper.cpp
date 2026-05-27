#include "input/FpnKeyboardInputMapper.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>
#include <QVariantMap>
#include <Qt>

namespace {

struct FpnDefaultBinding {
    const char *fpnKeyName;
    int fpnNote;
};

struct FpnDefaultAction {
    const char *fpnKeyName;
    const char *fpnLine;
    const char *fpnLabel;
};

struct FpnKeyName {
    int fpnLegacyCode;
    int fpnQtKey;
    const char *fpnName;
    const char *fpnLabel;
};

constexpr FpnKeyName fpnKeyNames[] = {
    {1, Qt::Key_Escape, "esc", "Esc"},
    {59, Qt::Key_F1, "f1", "F1"},
    {60, Qt::Key_F2, "f2", "F2"},
    {61, Qt::Key_F3, "f3", "F3"},
    {62, Qt::Key_F4, "f4", "F4"},
    {63, Qt::Key_F5, "f5", "F5"},
    {64, Qt::Key_F6, "f6", "F6"},
    {65, Qt::Key_F7, "f7", "F7"},
    {66, Qt::Key_F8, "f8", "F8"},
    {67, Qt::Key_F9, "f9", "F9"},
    {68, Qt::Key_F10, "f10", "F10"},
    {87, Qt::Key_F11, "f11", "F11"},
    {88, Qt::Key_F12, "f12", "F12"},
    {41, Qt::Key_QuoteLeft, "`", "`"},
    {2, Qt::Key_1, "1", "1"},
    {3, Qt::Key_2, "2", "2"},
    {4, Qt::Key_3, "3", "3"},
    {5, Qt::Key_4, "4", "4"},
    {6, Qt::Key_5, "5", "5"},
    {7, Qt::Key_6, "6", "6"},
    {8, Qt::Key_7, "7", "7"},
    {9, Qt::Key_8, "8", "8"},
    {10, Qt::Key_9, "9", "9"},
    {11, Qt::Key_0, "0", "0"},
    {12, Qt::Key_Minus, "-", "-"},
    {13, Qt::Key_Equal, "=", "="},
    {14, Qt::Key_Backspace, "back", "Backspace"},
    {15, Qt::Key_Tab, "tab", "Tab"},
    {16, Qt::Key_Q, "q", "Q"},
    {17, Qt::Key_W, "w", "W"},
    {18, Qt::Key_E, "e", "E"},
    {19, Qt::Key_R, "r", "R"},
    {20, Qt::Key_T, "t", "T"},
    {21, Qt::Key_Y, "y", "Y"},
    {22, Qt::Key_U, "u", "U"},
    {23, Qt::Key_I, "i", "I"},
    {24, Qt::Key_O, "o", "O"},
    {25, Qt::Key_P, "p", "P"},
    {26, Qt::Key_BracketLeft, "[", "["},
    {27, Qt::Key_BracketRight, "]", "]"},
    {43, Qt::Key_Backslash, "\\", "\\"},
    {58, Qt::Key_CapsLock, "caps", "Caps"},
    {30, Qt::Key_A, "a", "A"},
    {31, Qt::Key_S, "s", "S"},
    {32, Qt::Key_D, "d", "D"},
    {33, Qt::Key_F, "f", "F"},
    {34, Qt::Key_G, "g", "G"},
    {35, Qt::Key_H, "h", "H"},
    {36, Qt::Key_J, "j", "J"},
    {37, Qt::Key_K, "k", "K"},
    {38, Qt::Key_L, "l", "L"},
    {39, Qt::Key_Semicolon, ";", ";"},
    {40, Qt::Key_Apostrophe, "'", "'"},
    {28, Qt::Key_Return, "enter", "Enter"},
    {42, Qt::Key_Shift, "shift", "L Shift"},
    {44, Qt::Key_Z, "z", "Z"},
    {45, Qt::Key_X, "x", "X"},
    {46, Qt::Key_C, "c", "C"},
    {47, Qt::Key_V, "v", "V"},
    {48, Qt::Key_B, "b", "B"},
    {49, Qt::Key_N, "n", "N"},
    {50, Qt::Key_M, "m", "M"},
    {51, Qt::Key_Comma, ",", ","},
    {52, Qt::Key_Period, ".", "."},
    {53, Qt::Key_Slash, "/", "/"},
    {54, Qt::Key_Shift, "rshift", "R Shift"},
    {29, Qt::Key_Control, "ctrl", "L Ctrl"},
    {219, Qt::Key_Meta, "win", "Win"},
    {56, Qt::Key_Alt, "alt", "L Alt"},
    {57, Qt::Key_Space, "space", "Space"},
    {184, Qt::Key_Alt, "ralt", "R Alt"},
    {221, Qt::Key_Menu, "apps", "Menu"},
    {157, Qt::Key_Control, "rctrl", "R Ctrl"},
    {210, Qt::Key_Insert, "insert", "Ins"},
    {199, Qt::Key_Home, "home", "Home"},
    {201, Qt::Key_PageUp, "pgup", "PgUp"},
    {211, Qt::Key_Delete, "delete", "Del"},
    {207, Qt::Key_End, "end", "End"},
    {209, Qt::Key_PageDown, "pgdn", "PgDn"},
    {200, Qt::Key_Up, "up", "Up"},
    {203, Qt::Key_Left, "left", "Left"},
    {208, Qt::Key_Down, "down", "Down"},
    {205, Qt::Key_Right, "right", "Right"},
    {69, Qt::Key_NumLock, "numlock", "Num"},
    {181, Qt::Key_Slash, "num/", "/"},
    {55, Qt::Key_Asterisk, "num*", "*"},
    {74, Qt::Key_Minus, "num-", "-"},
    {71, Qt::Key_7, "num7", "7"},
    {72, Qt::Key_8, "num8", "8"},
    {73, Qt::Key_9, "num9", "9"},
    {78, Qt::Key_Plus, "num+", "+"},
    {75, Qt::Key_4, "num4", "4"},
    {76, Qt::Key_5, "num5", "5"},
    {77, Qt::Key_6, "num6", "6"},
    {79, Qt::Key_1, "num1", "1"},
    {80, Qt::Key_2, "num2", "2"},
    {81, Qt::Key_3, "num3", "3"},
    {156, Qt::Key_Enter, "numenter", "Enter"},
    {82, Qt::Key_0, "num0", "0"},
    {83, Qt::Key_Period, "num.", "."},
    {183, Qt::Key_Print, "printscreen", "PrintScreen"},
    {70, Qt::Key_ScrollLock, "scrolllock", "ScrollLock"},
    {197, Qt::Key_Pause, "pause", "Pause"},
};

constexpr FpnDefaultBinding fpnDefaultBindings[] = {
    {"left", 48}, {"down", 50}, {"right", 52}, {"up", 53},
    {"num0", 55}, {"num.", 57}, {"numenter", 59}, {"num1", 60},
    {"num2", 62}, {"num3", 64}, {"num4", 65}, {"num5", 67},
    {"num6", 69}, {"num7", 71}, {"num8", 72}, {"num9", 74},
    {"num+", 76}, {"numlock", 77}, {"num/", 79}, {"num*", 81},
    {"num-", 83}, {"delete", 84}, {"end", 86}, {"pgdn", 88},
    {"insert", 89}, {"home", 91}, {"pgup", 93},
    {"shift", 35}, {"z", 36}, {"x", 38}, {"c", 40},
    {"v", 41}, {"b", 43}, {"n", 45}, {"m", 47},
    {",", 48}, {".", 50}, {"/", 52}, {"rshift", 53},
    {"caps", 47}, {"a", 48}, {"s", 50}, {"d", 52},
    {"f", 53}, {"g", 55}, {"h", 57}, {"j", 59},
    {"k", 60}, {"l", 62}, {";", 64}, {"'", 65},
    {"enter", 67}, {"tab", 59}, {"q", 60}, {"w", 62},
    {"e", 64}, {"r", 65}, {"t", 67}, {"y", 69},
    {"u", 71}, {"i", 72}, {"o", 74}, {"p", 76},
    {"[", 77}, {"]", 79}, {"\\", 81}, {"`", 71},
    {"1", 72}, {"2", 74}, {"3", 76}, {"4", 77},
    {"5", 79}, {"6", 81}, {"7", 83}, {"8", 84},
    {"9", 86}, {"0", 88}, {"-", 89}, {"=", 91},
    {"back", 93},
};

constexpr const char *fpnDefaultGlobalLines[] = {
    "sustain\tin_0\tset\t127",
    "velocity\tin_0\tset\t80",
    "velocity\tin_1\tset\t100",
};

constexpr FpnDefaultAction fpnDefaultActions[] = {
    {"f1", "keydown\tF1\tGroup\t\tInc\t1", "Group +1"},
    {"f2", "keydown\tF2\tGroup\t\tDec\t1", "Group -1"},
    {"f3", "keydown\tF3\tKeySignature\tInc\t1", "Key +1"},
    {"f4", "keydown\tF4\tKeySignature\tDec\t1", "Key -1"},
    {"f5", "keydown\tF5\tOctave\tin_0\tInc\t1", "Octave L +1"},
    {"f6", "keydown\tF6\tOctave\tin_0\tDec\t1", "Octave L -1"},
    {"f7", "keydown\tF7\tOctave\tin_1\tInc\t1", "Octave R +1"},
    {"f8", "keydown\tF8\tOctave\tin_1\tDec\t1", "Octave R -1"},
    {"f9", "keydown\tF9\tVelocity\tin_0\tInc\t1", "Velocity L +1"},
    {"f10", "keydown\tF10\tVelocity\tin_0\tDec\t1", "Velocity L -1"},
    {"f11", "keydown\tF11\tVelocity\tin_1\tInc\t1", "Velocity R +1"},
    {"f12", "keydown\tF12\tVelocity\tin_1\tDec\t1", "Velocity R -1"},
    {"scrolllock", "keydown\tScrollLock\tRecord", "Record"},
    {"printscreen", "keydown\tPrintScreen\tPlay", "Play"},
    {"pause", "keydown\tPause\tStop", "Stop"},
};

QString fpnTrimmedLower(QString fpnValue)
{
    return fpnValue.trimmed().toLower();
}

QString fpnCanonicalLine(const QStringList &fpnParts)
{
    return fpnParts.join(QLatin1Char('\t'));
}

} // namespace

FpnKeyboardInputMapper::FpnKeyboardInputMapper(QObject *parent)
    : QObject(parent)
{
    fpnLoadDefaultDefinitions();
    fpnResetDefaultKeyboardLayout();
}

int FpnKeyboardInputMapper::fpnNoteForKey(int qtKey) const
{
    const int fpnLegacyCode = fpnLegacyCodeFromQtKey(qtKey);
    if (fpnLegacyCode < 0) {
        return -1;
    }
    return m_fpnBindings.value(fpnLegacyCode).fpnNote;
}

int FpnKeyboardInputMapper::fpnNoteForTokens(const QStringList &fpnTokens) const
{
    for (const QString &fpnToken : fpnTokens) {
        const int fpnLegacyCode = fpnLegacyCodeFromToken(fpnToken);
        if (fpnLegacyCode < 0) {
            continue;
        }

        const int fpnNote = m_fpnBindings.value(fpnLegacyCode).fpnNote;
        if (fpnNote >= 0) {
            return fpnNote;
        }
    }
    return -1;
}

bool FpnKeyboardInputMapper::fpnIsMapped(int qtKey) const
{
    const int fpnLegacyCode = fpnLegacyCodeFromQtKey(qtKey);
    return fpnLegacyCode >= 0 && m_fpnBindings.value(fpnLegacyCode).fpnNote >= 0;
}

bool FpnKeyboardInputMapper::fpnLoadKeyboardLayout(const QString &fpnPath)
{
    QFile fpnFile(fpnPath);
    if (!fpnFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fpnSetStatus(tr("Could not open keyboard layout"));
        return false;
    }

    const QString fpnText = QString::fromUtf8(fpnFile.readAll());
    if (QFileInfo(fpnPath).suffix().compare(QStringLiteral("cfg"), Qt::CaseInsensitive) == 0) {
        return fpnApplyLegacyConfigText(fpnText, QFileInfo(fpnPath).absolutePath());
    }

    if (!fpnApplyMapText(fpnText, true)) {
        fpnSetStatus(tr("Keyboard layout did not contain compatible note bindings"));
        return false;
    }

    const QString fpnCleanPath = QFileInfo(fpnPath).absoluteFilePath();
    if (m_fpnKeyboardLayoutPath != fpnCleanPath) {
        m_fpnKeyboardLayoutPath = fpnCleanPath;
        emit fpnKeyboardLayoutPathChanged();
    }
    fpnSetStatus(tr("Loaded keyboard layout"));
    emit fpnKeyboardLayoutChanged();
    return true;
}

bool FpnKeyboardInputMapper::fpnSaveKeyboardLayout(const QString &fpnPath)
{
    QString fpnTargetPath = fpnPath.trimmed();
    if (fpnTargetPath.isEmpty()) {
        fpnTargetPath = m_fpnKeyboardLayoutPath;
    }
    if (fpnTargetPath.isEmpty()) {
        fpnTargetPath = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("freepiano-next.map"));
    }
    if (!fpnTargetPath.endsWith(QStringLiteral(".map"), Qt::CaseInsensitive)) {
        fpnTargetPath += QStringLiteral(".map");
    }

    QFile fpnFile(fpnTargetPath);
    if (!fpnFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        fpnSetStatus(tr("Could not save keyboard layout"));
        return false;
    }

    QTextStream fpnStream(&fpnFile);
    fpnStream << fpnToMapText();
    const QString fpnCleanPath = QFileInfo(fpnTargetPath).absoluteFilePath();
    if (m_fpnKeyboardLayoutPath != fpnCleanPath) {
        m_fpnKeyboardLayoutPath = fpnCleanPath;
        emit fpnKeyboardLayoutPathChanged();
    }
    fpnSetStatus(tr("Saved keyboard layout"));
    return true;
}

void FpnKeyboardInputMapper::fpnResetDefaultKeyboardLayout()
{
    m_fpnBindings.clear();
    m_fpnGlobalLayoutLines.clear();
    for (const char *fpnLine : fpnDefaultGlobalLines) {
        m_fpnGlobalLayoutLines.append(QString::fromLatin1(fpnLine));
    }
    for (const FpnDefaultBinding &fpnDefaultBinding : fpnDefaultBindings) {
        const int fpnLegacyCode = m_fpnNameToLegacyCode.value(QString::fromLatin1(fpnDefaultBinding.fpnKeyName), -1);
        if (fpnLegacyCode >= 0) {
            m_fpnBindings.insert(fpnLegacyCode, {fpnDefaultBinding.fpnNote, QString()});
        }
    }
    const int fpnSpaceCode = m_fpnNameToLegacyCode.value(QStringLiteral("space"), -1);
    if (fpnSpaceCode >= 0) {
        m_fpnBindings.insert(fpnSpaceCode, {-1, QStringLiteral("FreePiano")});
    }
    for (const FpnDefaultAction &fpnDefaultAction : fpnDefaultActions) {
        const int fpnLegacyCode = m_fpnNameToLegacyCode.value(QString::fromLatin1(fpnDefaultAction.fpnKeyName), -1);
        if (fpnLegacyCode >= 0) {
            FpnBinding fpnBinding = m_fpnBindings.value(fpnLegacyCode);
            fpnBinding.fpnActionLines.append(QString::fromLatin1(fpnDefaultAction.fpnLine));
            fpnBinding.fpnActionLabel = QString::fromLatin1(fpnDefaultAction.fpnLabel);
            m_fpnBindings.insert(fpnLegacyCode, fpnBinding);
        }
    }

    fpnSetStatus(tr("Using default keyboard layout"));
    emit fpnKeyboardLayoutChanged();
}

void FpnKeyboardInputMapper::fpnSetKeyNoteByToken(const QString &fpnToken, int fpnNote)
{
    const int fpnLegacyCode = fpnLegacyCodeFromToken(fpnToken);
    if (fpnLegacyCode < 0) {
        return;
    }

    fpnNote = qBound(0, fpnNote, 127);
    FpnBinding fpnBinding = m_fpnBindings.value(fpnLegacyCode);
    if (fpnBinding.fpnNote == fpnNote) {
        return;
    }

    fpnBinding.fpnNote = fpnNote;
    m_fpnBindings.insert(fpnLegacyCode, fpnBinding);
    fpnSetStatus(tr("Keyboard layout changed"));
    emit fpnKeyboardLayoutChanged();
}

void FpnKeyboardInputMapper::fpnClearKeyBindingByToken(const QString &fpnToken)
{
    const int fpnLegacyCode = fpnLegacyCodeFromToken(fpnToken);
    if (fpnLegacyCode < 0 || !m_fpnBindings.contains(fpnLegacyCode)) {
        return;
    }

    FpnBinding fpnBinding = m_fpnBindings.value(fpnLegacyCode);
    fpnBinding.fpnNote = -1;
    if (fpnBinding.fpnLabel.isEmpty()) {
        m_fpnBindings.remove(fpnLegacyCode);
    } else {
        m_fpnBindings.insert(fpnLegacyCode, fpnBinding);
    }
    fpnSetStatus(tr("Keyboard layout changed"));
    emit fpnKeyboardLayoutChanged();
}

QString FpnKeyboardInputMapper::fpnBindingLabelForToken(const QString &fpnToken) const
{
    const int fpnLegacyCode = fpnLegacyCodeFromToken(fpnToken);
    if (fpnLegacyCode < 0) {
        return QString();
    }

    const FpnBinding fpnBinding = m_fpnBindings.value(fpnLegacyCode);
    if (!fpnBinding.fpnLabel.isEmpty()) {
        return fpnBinding.fpnLabel;
    }
    if (fpnBinding.fpnNote >= 0) {
        return fpnNoteName(fpnBinding.fpnNote);
    }
    return fpnBinding.fpnActionLabel;
}

QString FpnKeyboardInputMapper::fpnKeyTokenForTokens(const QStringList &fpnTokens) const
{
    for (const QString &fpnToken : fpnTokens) {
        const int fpnLegacyCode = fpnLegacyCodeFromToken(fpnToken);
        if (fpnLegacyCode >= 0 && m_fpnDefinitions.contains(fpnLegacyCode)) {
            return QStringLiteral("dik:%1").arg(fpnLegacyCode);
        }
    }
    return QString();
}

QString FpnKeyboardInputMapper::fpnNoteName(int fpnNote) const
{
    static const char *fpnNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    if (fpnNote < 0 || fpnNote > 127) {
        return QString();
    }
    return QStringLiteral("%1%2").arg(QString::fromLatin1(fpnNames[fpnNote % 12])).arg(fpnNote / 12 - 1);
}

QVariantList FpnKeyboardInputMapper::fpnKeyboardLayout() const
{
    QVariantList fpnLayout;
    for (const FpnKeyName &fpnKeyName : fpnKeyNames) {
        QVariantMap fpnItem;
        const FpnBinding fpnBinding = m_fpnBindings.value(fpnKeyName.fpnLegacyCode);
        fpnItem.insert(QStringLiteral("fpnToken"), QStringLiteral("dik:%1").arg(fpnKeyName.fpnLegacyCode));
        fpnItem.insert(QStringLiteral("fpnKeyName"), QString::fromLatin1(fpnKeyName.fpnName));
        fpnItem.insert(QStringLiteral("fpnDisplayLabel"), QString::fromLatin1(fpnKeyName.fpnLabel));
        fpnItem.insert(QStringLiteral("fpnNote"), fpnBinding.fpnNote);
        fpnItem.insert(QStringLiteral("fpnBindingLabel"), fpnBinding.fpnLabel.isEmpty() ? fpnBinding.fpnActionLabel : fpnBinding.fpnLabel);
        fpnItem.insert(QStringLiteral("fpnActionLabel"), fpnBinding.fpnActionLabel);
        fpnLayout.append(fpnItem);
    }
    return fpnLayout;
}

QString FpnKeyboardInputMapper::fpnKeyboardLayoutPath() const
{
    return m_fpnKeyboardLayoutPath;
}

QString FpnKeyboardInputMapper::fpnKeyboardLayoutStatus() const
{
    return m_fpnKeyboardLayoutStatus;
}

void FpnKeyboardInputMapper::fpnLoadDefaultDefinitions()
{
    for (const FpnKeyName &fpnKeyName : fpnKeyNames) {
        const QString fpnName = QString::fromLatin1(fpnKeyName.fpnName);
        m_fpnDefinitions.insert(fpnKeyName.fpnLegacyCode, {
            fpnKeyName.fpnLegacyCode,
            fpnKeyName.fpnQtKey,
            fpnName,
            QString::fromLatin1(fpnKeyName.fpnLabel),
        });
        if (!m_fpnQtKeyToLegacyCode.contains(fpnKeyName.fpnQtKey)) {
            m_fpnQtKeyToLegacyCode.insert(fpnKeyName.fpnQtKey, fpnKeyName.fpnLegacyCode);
        }
        m_fpnNameToLegacyCode.insert(fpnName, fpnKeyName.fpnLegacyCode);
    }

    m_fpnNameToLegacyCode.insert(QStringLiteral("~"), 41);
    m_fpnNameToLegacyCode.insert(QStringLiteral("backspace"), 14);
    m_fpnNameToLegacyCode.insert(QStringLiteral("capslock"), 58);
    m_fpnNameToLegacyCode.insert(QStringLiteral("rwin"), 220);
    m_fpnNameToLegacyCode.insert(QStringLiteral("pageup"), 201);
    m_fpnNameToLegacyCode.insert(QStringLiteral("pgdown"), 209);
    m_fpnNameToLegacyCode.insert(QStringLiteral("numenter"), 156);
}

void FpnKeyboardInputMapper::fpnSetStatus(const QString &fpnStatus)
{
    if (m_fpnKeyboardLayoutStatus == fpnStatus) {
        return;
    }
    m_fpnKeyboardLayoutStatus = fpnStatus;
    emit fpnKeyboardLayoutStatusChanged();
}

bool FpnKeyboardInputMapper::fpnApplyMapText(const QString &fpnText, bool fpnClearFirst)
{
    QHash<int, FpnBinding> fpnNewBindings = fpnClearFirst ? QHash<int, FpnBinding>() : m_fpnBindings;
    QStringList fpnNewGlobalLines = fpnClearFirst ? QStringList() : m_fpnGlobalLayoutLines;
    bool fpnParsedAny = false;
    quint32 fpnMapVersion = 0;
    const QRegularExpression fpnWhitespace(QStringLiteral("\\s+"));
    const QStringList fpnLines = fpnText.split(QRegularExpression(QStringLiteral("\\r?\\n")));

    for (QString fpnLine : fpnLines) {
        const int fpnCommentIndex = fpnLine.indexOf(QLatin1Char('#'));
        if (fpnCommentIndex >= 0) {
            fpnLine.truncate(fpnCommentIndex);
        }
        fpnLine = fpnLine.trimmed();
        if (fpnLine.isEmpty()) {
            continue;
        }

        QStringList fpnParts = fpnLine.split(fpnWhitespace, Qt::SkipEmptyParts);
        if (fpnParts.isEmpty()) {
            continue;
        }

        if (fpnParts.at(0).compare(QStringLiteral("FreePiano"), Qt::CaseInsensitive) == 0 && fpnParts.size() > 1) {
            const QStringList fpnVersionParts = fpnParts.at(1).split(QLatin1Char('.'));
            for (int i = 0; i < fpnVersionParts.size() && i < 4; ++i) {
                fpnMapVersion |= (fpnVersionParts.at(i).toUInt() & 0xff) << (24 - i * 8);
            }
            continue;
        }

        const QString fpnCommand = fpnTrimmedLower(fpnParts.takeFirst());
        if (fpnCommand != QStringLiteral("keydown")
            && fpnCommand != QStringLiteral("keyup")
            && fpnCommand != QStringLiteral("key")
            && fpnCommand != QStringLiteral("label")
            && fpnCommand != QStringLiteral("color")) {
            fpnNewGlobalLines.append(fpnLine);
            continue;
        }
        if (fpnParts.isEmpty()) {
            continue;
        }

        const int fpnLegacyCode = m_fpnNameToLegacyCode.value(fpnTrimmedLower(fpnParts.takeFirst()), -1);
        if (fpnLegacyCode < 0) {
            continue;
        }

        if (fpnCommand == QStringLiteral("keyup") || fpnCommand == QStringLiteral("color")) {
            FpnBinding fpnBinding = fpnNewBindings.value(fpnLegacyCode);
            fpnBinding.fpnActionLines.append(fpnCanonicalLine(QStringList {fpnCommand, fpnCanonicalKeyName(fpnLegacyCode)} + fpnParts));
            if (fpnBinding.fpnActionLabel.isEmpty()) {
                fpnBinding.fpnActionLabel = fpnCommand;
            }
            fpnNewBindings.insert(fpnLegacyCode, fpnBinding);
            fpnParsedAny = true;
            continue;
        }

        if (fpnCommand == QStringLiteral("label")) {
            FpnBinding fpnBinding = fpnNewBindings.value(fpnLegacyCode);
            fpnBinding.fpnLabel = fpnParts.join(QLatin1Char(' ')).trimmed();
            fpnNewBindings.insert(fpnLegacyCode, fpnBinding);
            fpnParsedAny = true;
            continue;
        }

        if (fpnParts.size() < 3 || fpnParts.at(0).compare(QStringLiteral("NoteOn"), Qt::CaseInsensitive) != 0) {
            FpnBinding fpnBinding = fpnNewBindings.value(fpnLegacyCode);
            const QStringList fpnEventParts = QStringList {fpnCommand, fpnCanonicalKeyName(fpnLegacyCode)} + fpnParts;
            fpnBinding.fpnActionLines.append(fpnCanonicalLine(fpnEventParts));
            const QString fpnActionLabel = fpnActionLabelFromParts(fpnParts);
            if (!fpnActionLabel.isEmpty()) {
                fpnBinding.fpnActionLabel = fpnActionLabel;
            }
            fpnNewBindings.insert(fpnLegacyCode, fpnBinding);
            fpnParsedAny = true;
            continue;
        }

        int fpnNote = -1;
        const QString fpnNoteToken = fpnTrimmedLower(fpnParts.at(2));
        static const QHash<QString, int> fpnPitchOffsets = {
            {QStringLiteral("c"), 0}, {QStringLiteral("c#"), 1}, {QStringLiteral("d"), 2}, {QStringLiteral("d#"), 3},
            {QStringLiteral("e"), 4}, {QStringLiteral("f"), 5}, {QStringLiteral("f#"), 6}, {QStringLiteral("g"), 7},
            {QStringLiteral("g#"), 8}, {QStringLiteral("a"), 9}, {QStringLiteral("a#"), 10}, {QStringLiteral("b"), 11},
        };
        const QRegularExpressionMatch fpnMatch = QRegularExpression(QStringLiteral("^([a-g]#?)(-?\\d+)$")).match(fpnNoteToken);
        if (fpnMatch.hasMatch()) {
            fpnNote = (fpnMatch.captured(2).toInt() + 1) * 12 + fpnPitchOffsets.value(fpnMatch.captured(1), 0);
            if (fpnMapVersion != 0 && fpnMapVersion < 0x01070000) {
                fpnNote -= 12;
            }
        } else {
            bool fpnOk = false;
            fpnNote = fpnNoteToken.toInt(&fpnOk);
            if (!fpnOk) {
                fpnNote = -1;
            }
        }

        if (fpnNote >= 0 && fpnNote <= 127) {
            FpnBinding fpnBinding = fpnNewBindings.value(fpnLegacyCode);
            fpnBinding.fpnNote = fpnNote;
            fpnNewBindings.insert(fpnLegacyCode, fpnBinding);
            fpnParsedAny = true;
        }
    }

    if (!fpnParsedAny) {
        return false;
    }
    m_fpnBindings = fpnNewBindings;
    m_fpnGlobalLayoutLines = fpnNewGlobalLines;
    return true;
}

bool FpnKeyboardInputMapper::fpnApplyLegacyConfigText(const QString &fpnText, const QString &fpnBasePath)
{
    const QStringList fpnLines = fpnText.split(QRegularExpression(QStringLiteral("\\r?\\n")));
    for (QString fpnLine : fpnLines) {
        const int fpnCommentIndex = fpnLine.indexOf(QLatin1Char('#'));
        if (fpnCommentIndex >= 0) {
            fpnLine.truncate(fpnCommentIndex);
        }
        fpnLine = fpnLine.trimmed();
        if (fpnLine.isEmpty()) {
            continue;
        }

        const QStringList fpnParts = fpnLine.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
        if (fpnParts.size() < 3
            || fpnParts.at(0).compare(QStringLiteral("keyboard"), Qt::CaseInsensitive) != 0
            || fpnParts.at(1).compare(QStringLiteral("map"), Qt::CaseInsensitive) != 0) {
            continue;
        }

        QString fpnMapPath = fpnLine.section(QRegularExpression(QStringLiteral("\\s+")), 2).trimmed();
        if ((fpnMapPath.startsWith(QLatin1Char('"')) && fpnMapPath.endsWith(QLatin1Char('"')))
            || (fpnMapPath.startsWith(QLatin1Char('\'')) && fpnMapPath.endsWith(QLatin1Char('\'')))) {
            fpnMapPath = fpnMapPath.mid(1, fpnMapPath.size() - 2);
        }
        QFileInfo fpnMapInfo(fpnMapPath);
        if (fpnMapInfo.isRelative()) {
            fpnMapInfo = QFileInfo(QDir(fpnBasePath).filePath(fpnMapPath));
        }
        return fpnLoadKeyboardLayout(fpnMapInfo.absoluteFilePath());
    }

    fpnSetStatus(tr("Legacy config does not reference a keyboard map"));
    return false;
}

QString FpnKeyboardInputMapper::fpnToMapText() const
{
    QString fpnText;
    QTextStream fpnStream(&fpnText);
    fpnStream << "FreePiano 1.8\r\n\r\n";
    for (const QString &fpnLine : m_fpnGlobalLayoutLines) {
        fpnStream << fpnLine << "\r\n";
    }
    fpnStream << "\r\n";
    fpnStream << "#\tkeyname\taction\tchannel\tnote\r\n";

    for (const FpnKeyName &fpnKeyName : fpnKeyNames) {
        const FpnBinding fpnBinding = m_fpnBindings.value(fpnKeyName.fpnLegacyCode);
        if (fpnBinding.fpnNote >= 0) {
            fpnStream << "keydown\t" << fpnCanonicalKeyName(fpnKeyName.fpnLegacyCode) << "\tNoteOn\tin_0\t"
                      << fpnNoteName(fpnBinding.fpnNote).toLower() << "\r\n";
        }
        if (!fpnBinding.fpnLabel.isEmpty()) {
            fpnStream << "label\t" << fpnCanonicalKeyName(fpnKeyName.fpnLegacyCode) << "\t" << fpnBinding.fpnLabel << "\r\n";
        }
        for (const QString &fpnActionLine : fpnBinding.fpnActionLines) {
            fpnStream << fpnActionLine << "\r\n";
        }
    }
    return fpnText;
}

QString FpnKeyboardInputMapper::fpnActionLabelFromParts(const QStringList &fpnParts) const
{
    if (fpnParts.isEmpty()) {
        return QString();
    }

    const QString fpnAction = fpnParts.at(0);
    if (fpnAction.compare(QStringLiteral("Group"), Qt::CaseInsensitive) == 0 && fpnParts.size() >= 3) {
        return QStringLiteral("Group %1%2")
            .arg(fpnParts.at(1).compare(QStringLiteral("Inc"), Qt::CaseInsensitive) == 0 ? QStringLiteral("+") : QStringLiteral("-"),
                 fpnParts.at(2));
    }
    if (fpnAction.compare(QStringLiteral("KeySignature"), Qt::CaseInsensitive) == 0 && fpnParts.size() >= 3) {
        return QStringLiteral("Key %1%2")
            .arg(fpnParts.at(1).compare(QStringLiteral("Inc"), Qt::CaseInsensitive) == 0 ? QStringLiteral("+") : QStringLiteral("-"),
                 fpnParts.at(2));
    }
    if ((fpnAction.compare(QStringLiteral("Octave"), Qt::CaseInsensitive) == 0
         || fpnAction.compare(QStringLiteral("Velocity"), Qt::CaseInsensitive) == 0)
        && fpnParts.size() >= 4) {
        return QStringLiteral("%1 %2 %3%4")
            .arg(fpnAction,
                 fpnParts.at(1),
                 fpnParts.at(2).compare(QStringLiteral("Inc"), Qt::CaseInsensitive) == 0 ? QStringLiteral("+") : QStringLiteral("-"),
                 fpnParts.at(3));
    }
    if (fpnAction.compare(QStringLiteral("Play"), Qt::CaseInsensitive) == 0
        || fpnAction.compare(QStringLiteral("Record"), Qt::CaseInsensitive) == 0
        || fpnAction.compare(QStringLiteral("Stop"), Qt::CaseInsensitive) == 0) {
        return fpnAction;
    }
    return fpnAction;
}

int FpnKeyboardInputMapper::fpnLegacyCodeFromToken(const QString &fpnToken) const
{
    if (fpnToken.startsWith(QStringLiteral("dik:"))) {
        bool fpnOk = false;
        const int fpnLegacyCode = fpnToken.mid(4).toInt(&fpnOk);
        return fpnOk ? fpnLegacyCode : -1;
    }

    if (fpnToken.startsWith(QStringLiteral("name:"))) {
        return m_fpnNameToLegacyCode.value(fpnTrimmedLower(fpnToken.mid(5)), -1);
    }

    if (fpnToken.startsWith(QStringLiteral("scan:"))) {
        bool fpnOk = false;
        int fpnScan = fpnToken.mid(5).toInt(&fpnOk);
        if (!fpnOk) {
            return -1;
        }
        if (fpnScan > 255) {
            if ((fpnScan & 0xff00) == 0xe000) {
                fpnScan = (fpnScan & 0xff) | 0x80;
            } else if ((fpnScan & 0x100) == 0x100) {
                fpnScan = (fpnScan & 0xff) | 0x80;
            }
        }
        return fpnScan & 0xff;
    }

    return -1;
}

int FpnKeyboardInputMapper::fpnLegacyCodeFromQtKey(int fpnQtKey) const
{
    return m_fpnQtKeyToLegacyCode.value(fpnQtKey, -1);
}

QString FpnKeyboardInputMapper::fpnCanonicalKeyName(int fpnLegacyCode) const
{
    return m_fpnDefinitions.value(fpnLegacyCode).fpnName;
}
