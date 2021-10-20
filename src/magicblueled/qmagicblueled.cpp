#include "qmagicblueled.h"

QT_BEGIN_NAMESPACE

MagicBlueLed::MagicBlueLed(QString nom, QString adresseMAC, QObject *parent) : QObject(parent), nom(nom), adresseMAC(adresseMAC)
{
}

QString MagicBlueLed::getNom() const
{
    return nom;
}

QString MagicBlueLed::getAdresseMAC() const
{
    return adresseMAC;
}

QT_END_NAMESPACE
