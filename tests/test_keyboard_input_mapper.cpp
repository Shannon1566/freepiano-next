#include <QtTest/QtTest>

#include "input/FpnKeyboardInputMapper.h"

class FpnKeyboardInputMapperTests : public QObject
{
    Q_OBJECT

private slots:
    void fpnDefaultLayoutMatchesFreePianoNotesAndActions();
    void fpnCanModifyAndSaveLayout();
    void fpnLoadsLegacyMapText();
    void fpnLoadsLegacyConfigMapReference();
};

void FpnKeyboardInputMapperTests::fpnDefaultLayoutMatchesFreePianoNotesAndActions()
{
    FpnKeyboardInputMapper fpnMapper;

    QCOMPARE(fpnMapper.fpnNoteForTokens({QStringLiteral("name:a")}), 48);
    QCOMPARE(fpnMapper.fpnNoteForTokens({QStringLiteral("name:q")}), 60);
    QCOMPARE(fpnMapper.fpnNoteForTokens({QStringLiteral("name:num1")}), 60);
    QCOMPARE(fpnMapper.fpnBindingLabelForToken(QStringLiteral("name:f1")), QStringLiteral("Group +1"));
    QCOMPARE(fpnMapper.fpnBindingLabelForToken(QStringLiteral("name:space")), QStringLiteral("FreePiano"));
}

void FpnKeyboardInputMapperTests::fpnCanModifyAndSaveLayout()
{
    QTemporaryDir fpnDir;
    QVERIFY(fpnDir.isValid());

    FpnKeyboardInputMapper fpnMapper;
    fpnMapper.fpnSetKeyNoteByToken(QStringLiteral("name:a"), 61);
    QCOMPARE(fpnMapper.fpnNoteForTokens({QStringLiteral("name:a")}), 61);

    const QString fpnMapPath = fpnDir.filePath(QStringLiteral("edited.map"));
    QVERIFY(fpnMapper.fpnSaveKeyboardLayout(fpnMapPath));

    QFile fpnSavedFile(fpnMapPath);
    QVERIFY(fpnSavedFile.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString fpnSavedText = QString::fromUtf8(fpnSavedFile.readAll());

    QVERIFY(fpnSavedText.contains(QStringLiteral("FreePiano 1.8")));
    QVERIFY(fpnSavedText.contains(QStringLiteral("velocity\tin_1\tset\t100")));
    QVERIFY(fpnSavedText.contains(QStringLiteral("keydown\ta\tNoteOn\tin_0\tc#4")));
    QVERIFY(fpnSavedText.contains(QStringLiteral("keydown\tF1\tGroup\t\tInc\t1")));
    QVERIFY(fpnSavedText.contains(QStringLiteral("label\tspace\tFreePiano")));
}

void FpnKeyboardInputMapperTests::fpnLoadsLegacyMapText()
{
    QTemporaryDir fpnDir;
    QVERIFY(fpnDir.isValid());

    const QString fpnMapPath = fpnDir.filePath(QStringLiteral("legacy.map"));
    QFile fpnMapFile(fpnMapPath);
    QVERIFY(fpnMapFile.open(QIODevice::WriteOnly | QIODevice::Text));
    fpnMapFile.write("FreePiano 1.8\r\n");
    fpnMapFile.write("velocity in_1 set 100\r\n");
    fpnMapFile.write("keydown a NoteOn in_0 c3\r\n");
    fpnMapFile.write("keydown F1 Group  Inc 1\r\n");
    fpnMapFile.write("label space FreePiano\r\n");
    fpnMapFile.close();

    FpnKeyboardInputMapper fpnMapper;
    QVERIFY(fpnMapper.fpnLoadKeyboardLayout(fpnMapPath));
    QCOMPARE(fpnMapper.fpnNoteForTokens({QStringLiteral("name:a")}), 48);
    QCOMPARE(fpnMapper.fpnBindingLabelForToken(QStringLiteral("name:f1")), QStringLiteral("Group +1"));
    QCOMPARE(fpnMapper.fpnBindingLabelForToken(QStringLiteral("name:space")), QStringLiteral("FreePiano"));
}

void FpnKeyboardInputMapperTests::fpnLoadsLegacyConfigMapReference()
{
    QTemporaryDir fpnDir;
    QVERIFY(fpnDir.isValid());

    QFile fpnMapFile(fpnDir.filePath(QStringLiteral("custom.map")));
    QVERIFY(fpnMapFile.open(QIODevice::WriteOnly | QIODevice::Text));
    fpnMapFile.write("FreePiano 1.8\r\n");
    fpnMapFile.write("keydown q NoteOn in_0 d4\r\n");
    fpnMapFile.close();

    QFile fpnConfigFile(fpnDir.filePath(QStringLiteral("freepiano.cfg")));
    QVERIFY(fpnConfigFile.open(QIODevice::WriteOnly | QIODevice::Text));
    fpnConfigFile.write("keyboard map custom.map\r\n");
    fpnConfigFile.close();

    FpnKeyboardInputMapper fpnMapper;
    QVERIFY(fpnMapper.fpnLoadKeyboardLayout(fpnConfigFile.fileName()));
    QCOMPARE(fpnMapper.fpnNoteForTokens({QStringLiteral("name:q")}), 62);
}

QTEST_MAIN(FpnKeyboardInputMapperTests)

#include "test_keyboard_input_mapper.moc"
