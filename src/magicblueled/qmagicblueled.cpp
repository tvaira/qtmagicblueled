#include "qmagicblueled.h"

QT_BEGIN_NAMESPACE

MagicBlueLed::MagicBlueLed(QString nom, QString adresseMAC, QObject *parent) : QObject(parent), _nom(nom), _adresseMAC(adresseMAC)
{
}

MagicBlueLed::MagicBlueLed(const MagicBlueLed& magicBlueLed) : QObject(magicBlueLed.parent()), _nom(magicBlueLed._nom), _adresseMAC(magicBlueLed._adresseMAC)
{

}

MagicBlueLed& MagicBlueLed::operator=(const MagicBlueLed& magicBlueLed)
{
    if(this != &magicBlueLed)
    {
        _nom = magicBlueLed._nom;
        _adresseMAC = magicBlueLed._adresseMAC;
    }
    return *this;
}

QString MagicBlueLed::nom() const
{
    return _nom;
}

QString MagicBlueLed::adresseMAC() const
{
    return _adresseMAC;
}

QT_END_NAMESPACE
