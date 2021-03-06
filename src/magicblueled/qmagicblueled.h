#ifndef QMAGICBLUELED_H
#define QMAGICBLUELED_H

#include <QtMagicBlueLed/qmagicblueledglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class Q_MAGICBLUELED_EXPORT MagicBlueLed : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nom READ nom NOTIFY magicBlueLedChanged)
    Q_PROPERTY(QString adresseMAC READ adresseMAC NOTIFY magicBlueLedChanged)

public:
    explicit MagicBlueLed(QString nom, QString adresseMAC, QObject *parent = nullptr);
    MagicBlueLed(const MagicBlueLed& magicBlueLed);
    MagicBlueLed& operator=(const MagicBlueLed& magicBlueLed);
    QString nom() const;
    QString adresseMAC() const;

private:
    QString _nom;
    QString _adresseMAC;

signals:
    void magicBlueLedChanged();

public slots:
};

QT_END_NAMESPACE

#endif // QMAGICBLUELED_H
