#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class FpnKeyboardInputMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList fpnKeyboardLayout READ fpnKeyboardLayout NOTIFY fpnKeyboardLayoutChanged)
    Q_PROPERTY(QString fpnKeyboardLayoutPath READ fpnKeyboardLayoutPath NOTIFY fpnKeyboardLayoutPathChanged)
    Q_PROPERTY(QString fpnKeyboardLayoutStatus READ fpnKeyboardLayoutStatus NOTIFY fpnKeyboardLayoutStatusChanged)

public:
    explicit FpnKeyboardInputMapper(QObject *parent = nullptr);

    Q_INVOKABLE int fpnNoteForKey(int qtKey) const;
    Q_INVOKABLE int fpnNoteForTokens(const QStringList &fpnTokens) const;
    Q_INVOKABLE bool fpnIsMapped(int qtKey) const;
    Q_INVOKABLE bool fpnLoadKeyboardLayout(const QString &fpnPath);
    Q_INVOKABLE bool fpnSaveKeyboardLayout(const QString &fpnPath = QString());
    Q_INVOKABLE void fpnResetDefaultKeyboardLayout();
    Q_INVOKABLE void fpnSetKeyNoteByToken(const QString &fpnToken, int fpnNote);
    Q_INVOKABLE void fpnClearKeyBindingByToken(const QString &fpnToken);
    Q_INVOKABLE QString fpnBindingLabelForToken(const QString &fpnToken) const;
    Q_INVOKABLE QString fpnKeyTokenForTokens(const QStringList &fpnTokens) const;
    Q_INVOKABLE QString fpnNoteName(int fpnNote) const;

    QVariantList fpnKeyboardLayout() const;
    QString fpnKeyboardLayoutPath() const;
    QString fpnKeyboardLayoutStatus() const;

signals:
    void fpnKeyboardLayoutChanged();
    void fpnKeyboardLayoutPathChanged();
    void fpnKeyboardLayoutStatusChanged();

private:
    struct FpnKeyDefinition {
        int fpnLegacyCode = 0;
        int fpnQtKey = 0;
        QString fpnName;
        QString fpnDisplayLabel;
    };

    struct FpnBinding {
        int fpnNote = -1;
        QString fpnLabel;
        QStringList fpnActionLines;
        QString fpnActionLabel;
    };

    void fpnLoadDefaultDefinitions();
    void fpnSetStatus(const QString &fpnStatus);
    bool fpnApplyMapText(const QString &fpnText, bool fpnClearFirst);
    bool fpnApplyLegacyConfigText(const QString &fpnText, const QString &fpnBasePath);
    QString fpnToMapText() const;
    QString fpnActionLabelFromParts(const QStringList &fpnParts) const;
    int fpnLegacyCodeFromToken(const QString &fpnToken) const;
    int fpnLegacyCodeFromQtKey(int fpnQtKey) const;
    QString fpnCanonicalKeyName(int fpnLegacyCode) const;

    QHash<int, FpnBinding> m_fpnBindings;
    QHash<int, FpnKeyDefinition> m_fpnDefinitions;
    QHash<int, int> m_fpnQtKeyToLegacyCode;
    QHash<QString, int> m_fpnNameToLegacyCode;
    QStringList m_fpnGlobalLayoutLines;
    QString m_fpnKeyboardLayoutPath;
    QString m_fpnKeyboardLayoutStatus;
};
