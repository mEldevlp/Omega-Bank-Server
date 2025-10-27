#pragma once

#include <QDialog>
#include <QList>
#include <QPair>
#include <QString>

class QComboBox;
class QLineEdit;
class QDateEdit;
class QLabel;

class AddCardDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddCardDialog(const QList<QPair<QString, QString>>& accounts, QWidget* parent = nullptr);

private slots:
    void onCreate();

private:
    QComboBox* accountCombo = nullptr;
    QComboBox* typeCombo = nullptr;
    QLineEdit* panEdit = nullptr;
    QDateEdit* expiryEdit = nullptr;
    QComboBox* statusCombo = nullptr;
    QLabel* statusLabel = nullptr;

    static bool luhn_check(const QString& s);
    QString sanitizePan(const QString& pan) const;
    bool validatePan(const QString& pan) const;
};