#ifndef QMAGICBLUELED_H
#define QMAGICBLUELED_H

#include <QtMagicBlueLed/qmagicblueledglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class Q_MAGICBLUELED_EXPORT MagicBlueLed : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nom READ getNom NOTIFY magicBlueLedChanged)
    Q_PROPERTY(QString adresseMAC READ getAdresseMAC NOTIFY magicBlueLedChanged)

public:
    explicit MagicBlueLed(QString nom, QString adresseMAC, QObject *parent = nullptr);
    QString getNom() const;
    QString getAdresseMAC() const;

private:
    QString nom;
    QString adresseMAC;

signals:
    void magicBlueLedChanged();

public slots:
};

QT_END_NAMESPACE

#endif // QMAGICBLUELED_H
