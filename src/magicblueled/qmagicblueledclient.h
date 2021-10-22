#ifndef QTMAGICBLUELEDCLIENT_H
#define QTMAGICBLUELEDCLIENT_H

#include <QtMagicBlueLed/qmagicblueledglobal.h>
#include <QtMagicBlueLed/qmagicblueled.h>

#include <QtCore/QIODevice>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyController>

#define Q_MAGICBLUELED_SERVICE_UUID         QString::fromUtf8("{0000ffe5-0000-1000-8000-00805f9b34fb}")
#define Q_MAGICBLUELED_CHARACTERISTIC_UUID  QString::fromUtf8("{0000ffe9-0000-1000-8000-00805f9b34fb}")
#define Q_MAGICBLUELED_MAX_SIZE             20 // 20 octets de données max

QT_BEGIN_NAMESPACE

class Q_MAGICBLUELED_EXPORT QMagicBlueLedClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool magicBlueLedDetecte MEMBER m_magicBlueLedDetecte NOTIFY detecte)
    Q_PROPERTY(bool etatRecherche MEMBER m_etatRecherche NOTIFY recherche)
    Q_PROPERTY(bool etatConnexion MEMBER m_etatConnexion NOTIFY connecte)
    Q_PROPERTY(bool connexionErreur MEMBER m_connexionErreur NOTIFY erreurUpdated)
    Q_PROPERTY(QList<MagicBlueLed*> listeMagicBlueLed READ getMagicBlueLed NOTIFY magicBlueLedUpdated)

public:
    explicit QMagicBlueLedClient(QObject *parent = nullptr);
    ~QMagicBlueLedClient();

    Q_INVOKABLE void rechercher(QString nom=QString::fromUtf8("LEDBLE"), QString adresseMAC=QString::fromUtf8(""));
    Q_INVOKABLE void arreter();
    Q_INVOKABLE void connecter(QString adresse);
    Q_INVOKABLE void deconnecter();
    Q_INVOKABLE void read();
    Q_INVOKABLE void allumer();
    Q_INVOKABLE void eteindre();
    Q_INVOKABLE void write(const QByteArray &data);
    Q_INVOKABLE void write(int rouge, int vert, int bleu, int white=0);
    Q_INVOKABLE void write(bool etat);
    Q_INVOKABLE void gererNotification(bool notification);
    QList<MagicBlueLed*> getMagicBlueLed();
    bool etatConnexion() const;
    bool etatRecherche() const;
    bool connexionErreur() const;
    bool magicBlueLedDetecte() const;
    bool estConnecte() const;
    bool estDetecte() const;

Q_SIGNALS:
    void connecte();
    void recherche();
    void erreurUpdated();
    void detecte();
    void magicBlueLedUpdated();

public Q_SLOTS:
    void ajouterMagicBlueLed(const QBluetoothDeviceInfo&);
    void rechercheTerminee();
    void rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error);
    void connecterMagicBlueLed(const QString &adresseServeur);
    void connecterService(QLowEnergyService *service);
    void ajouterService(QBluetoothUuid serviceUuid);
    void serviceDetailsDiscovered(QLowEnergyService::ServiceState newState);
    void serviceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void magicBlueLedConnecte();
    void magicBlueLedDeconnecte();
    void connecteErreur(QLowEnergyController::Error);

private:
    QList<MagicBlueLed*>             m_devices;
    QBluetoothDeviceDiscoveryAgent  *m_discoveryAgent;
    QLowEnergyController            *m_controller;
    QLowEnergyService               *m_service;
    QLowEnergyCharacteristic         m_characteristic;
    bool                             m_etatConnexion;
    bool                             m_etatRecherche;
    bool                             m_connexionErreur;
    bool                             m_magicBlueLedDetecte;
    QString                          m_nomRecherche;
    QString                          m_adresseMACRecherche;
};

QT_END_NAMESPACE

#endif // #ifndef QTMAGICBLUELEDCLIENT_H
