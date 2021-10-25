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

#define Q_MAGICBLUELED_SERVICE_UUID                  QString::fromUtf8("{0000ffe5-0000-1000-8000-00805f9b34fb}")
#define Q_MAGICBLUELED_CHARACTERISTIC_UUID           QString::fromUtf8("{0000ffe9-0000-1000-8000-00805f9b34fb}")
#define Q_MAGICBLUELED_SERVICE_UUID_CONFIGURATION    QString::fromUtf8("{0000ffe0-0000-1000-8000-00805f9b34fb}")
#define Q_MAGICBLUELED_CHARACTERISTIC_CONFIGURATION  QString::fromUtf8("{0000ffe4-0000-1000-8000-00805f9b34fb}")
#define Q_MAGICBLUELED_MAX_SIZE                      20 // 20 octets de données max

QT_BEGIN_NAMESPACE

class Q_MAGICBLUELED_EXPORT QMagicBlueLedClient : public QObject
{
public:
    enum DeviceType
    {
        RGB = 0x03,
        RBGW_Bulb_New = 0x15,
        RBGW_UFO = 0x44
    };
    enum BulbState
    {
        ON = 0x23,
        OFF = 0x24
    };
    enum FieldState
    {
        DeviceType = 1,
        On_Off = 2,
        Running = 4,
        Red = 6,
        Green = 7,
        Blue = 8,
        White = 9,
        LedVersionNum = 10
    };

private:
    Q_OBJECT
    Q_ENUMS(DeviceType)
    Q_ENUMS(BulbState)
    Q_ENUMS(FieldState)
    Q_PROPERTY(bool magicBlueLedDetecte MEMBER m_magicBlueLedDetecte NOTIFY detecteUpdated)
    Q_PROPERTY(bool etatRecherche MEMBER m_etatRecherche NOTIFY rechercheUpdated)
    Q_PROPERTY(bool etatServiceCommande MEMBER m_etatServiceCommande NOTIFY serviceCommandeUpdated)
    Q_PROPERTY(bool etatServiceEtat MEMBER m_etatServiceEtat NOTIFY serviceEtatUpdated)
    Q_PROPERTY(bool etatConnexion MEMBER m_etatConnexion NOTIFY connecteUpdated)
    Q_PROPERTY(bool connexionErreur MEMBER m_connexionErreur NOTIFY erreurUpdated)
    Q_PROPERTY(QList<MagicBlueLed*> listeMagicBlueLed READ getMagicBlueLed NOTIFY magicBlueLedUpdated)

public:
    explicit QMagicBlueLedClient(QObject *parent = nullptr);
    ~QMagicBlueLedClient();
    Q_INVOKABLE void rechercher(QString nom=QString::fromUtf8("LEDBLE"), QString adresseMAC=QString::fromUtf8(""));
    Q_INVOKABLE void arreter();
    Q_INVOKABLE void connecter(QString adresse);
    Q_INVOKABLE void deconnecter();
    Q_INVOKABLE void allumer();
    Q_INVOKABLE void eteindre();
    Q_INVOKABLE void commanderRGB(int rouge, int vert, int bleu);
    Q_INVOKABLE void commander(int blanc);
    Q_INVOKABLE void commanderModeIntegre(int modeIntegre, int vitesse);
    Q_INVOKABLE void lireEtat();
    QList<MagicBlueLed*> getMagicBlueLed();
    bool etatConnexion() const;
    bool etatRecherche() const;
    bool connexionErreur() const;
    bool magicBlueLedDetecte() const;
    bool etatServiceCommande() const;
    bool etatServiceEtat() const;
    bool estConnecte() const;
    bool estDetecte() const;

Q_SIGNALS:
    void connecteUpdated();
    void rechercheUpdated();
    void erreurUpdated();
    void detecteUpdated();
    void magicBlueLedUpdated();
    void serviceCommandeUpdated();
    void serviceEtatUpdated();
    void etatChanged(const QByteArray &value);

public Q_SLOTS:
    void connecterMagicBlueLed(const QString &adresseServeur);

private Q_SLOTS:
    void ajouterMagicBlueLed(const QBluetoothDeviceInfo&);
    void rechercheTerminee();
    void rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error);
    void ajouterService(QBluetoothUuid serviceUuid);
    void magicBlueLedConnecte();
    void magicBlueLedDeconnecte();
    void connecteErreur(QLowEnergyController::Error);
    void serviceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void serviceDetailsDiscovered(QLowEnergyService::ServiceState newState);
    void connecterService(QLowEnergyService *service);
    void decouverteServicesTerminee();

private:
    QList<MagicBlueLed*>             m_devices;
    QBluetoothDeviceDiscoveryAgent  *m_discoveryAgent;
    QLowEnergyController            *m_controller;
    QLowEnergyService               *m_service;
    QLowEnergyCharacteristic         m_characteristic;
    QLowEnergyService               *m_serviceConfiguration;
    QLowEnergyCharacteristic         m_characteristicConfiguration;
    bool                             m_etatConnexion;
    bool                             m_etatRecherche;
    bool                             m_connexionErreur;
    bool                             m_magicBlueLedDetecte;
    bool                             m_etatServiceCommande;
    bool                             m_etatServiceEtat;
    QString                          m_nomRecherche;
    QString                          m_adresseMACRecherche;

    void gererNotification(bool notification);
    void write(const QByteArray &data, bool reponse=false);
    void write(bool etat);
};

QT_END_NAMESPACE

#endif // #ifndef QTMAGICBLUELEDCLIENT_H
