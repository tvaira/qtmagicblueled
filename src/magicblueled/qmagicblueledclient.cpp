#include "qmagicblueledclient.h"
#include "qmagicblueled.h"

#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtCore/QtEndian>

QT_BEGIN_NAMESPACE

QMagicBlueLedClient::QMagicBlueLedClient(QObject *parent) : QObject(parent), m_discoveryAgent(nullptr), m_controller(nullptr), m_service(nullptr), m_serviceConfiguration(nullptr), m_etatConnexion(false), m_etatRecherche(false), m_connexionErreur(false), m_magicBlueLedDetecte(false), m_etatServiceCommande(false), m_etatServiceEtat(false)
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    m_discoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    // Slot pour la recherche d'appareils BLE
    connect(m_discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(ajouterMagicBlueLed(QBluetoothDeviceInfo)));
    connect(m_discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(rechercheTerminee()));
}

QMagicBlueLedClient::~QMagicBlueLedClient()
{
    if(m_controller)
        m_controller->disconnectFromDevice();
    delete m_controller;
    qDeleteAll(m_devices);
}

void QMagicBlueLedClient::rechercher(QString nom, QString adresseMAC)
{
    if(!nom.isEmpty())
        m_nomRecherche = nom;
    if(!adresseMAC.isEmpty())
        m_adresseMACRecherche = adresseMAC;
    qDeleteAll(m_devices);
    m_devices.clear();
    m_magicBlueLedDetecte = false;
    emit detecteUpdated();
    emit magicBlueLedUpdated();

    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    if(m_discoveryAgent->isActive())
    {
        m_etatRecherche = true;
        emit rechercheUpdated();
    }
}

void QMagicBlueLedClient::arreter()
{
    if(m_etatRecherche && m_discoveryAgent->isActive())
    {
        m_discoveryAgent->stop();
    }
}

void QMagicBlueLedClient::connecter(QString adresse)
{
    connecterMagicBlueLed(adresse);
}

void QMagicBlueLedClient::deconnecter()
{
    if(m_controller)
        m_controller->disconnectFromDevice();
}

void QMagicBlueLedClient::allumer()
{
    this->write(true);
}

void QMagicBlueLedClient::eteindre()
{
    this->write(false);
}

void QMagicBlueLedClient::commanderRGB(int rouge, int vert, int bleu)
{
    QByteArray datas(7, 0);

    datas[0] = 0x56;
    datas[1] = static_cast<char>(rouge); // RR
    datas[2] = static_cast<char>(vert);  // GG
    datas[3] = static_cast<char>(bleu);  // BB
    datas[4] = 0x00; // WW
    datas[5] = 0xf0; // mode
    datas[6] = 0xaa;

    this->write(datas);
}

void QMagicBlueLedClient::commander(int blanc)
{
    QByteArray datas(7, 0);

    datas[0] = 0x56;
    datas[1] = 0x00; // RR
    datas[2] = 0x00; // GG
    datas[3] = 0x00; // BB
    datas[4] = static_cast<char>(blanc); // WW
    datas[5] = 0x0f; // mode
    datas[6] = 0xaa;

    this->write(datas);
}

void QMagicBlueLedClient::commanderModeIntegre(int modeIntegre, int vitesse)
{
    /*
        button_gradual : 0x25, 28 // graduelle
        button_flash : 0x30, 25
        button_saltus : 0x38, 28 // sans transition
    */
    QByteArray datas(4, 0);

    vitesse = 31 - vitesse;
    datas[0] = 0xBB;
    datas[1] = static_cast<char>(modeIntegre);
    datas[2] = static_cast<char>(vitesse);
    datas[3] = 0x44;

    this->write(datas);
}

void QMagicBlueLedClient::lireEtat()
{
    QByteArray datas(3, 0);

    datas[0] = 0xEF;
    datas[1] = 0x01;
    datas[2] = 0x77;

    this->write(datas, true);
}

QList<MagicBlueLed*> QMagicBlueLedClient::getMagicBlueLed()
{
    return m_devices;
}

bool QMagicBlueLedClient::etatConnexion() const
{
    return m_etatConnexion;
}

bool QMagicBlueLedClient::etatRecherche() const
{
    return m_etatRecherche;
}

bool QMagicBlueLedClient::connexionErreur() const
{
    return m_connexionErreur;
}

bool QMagicBlueLedClient::magicBlueLedDetecte() const
{
    return m_magicBlueLedDetecte;
}

bool QMagicBlueLedClient::etatServiceCommande() const
{
    return m_etatServiceCommande;
}

bool QMagicBlueLedClient::etatServiceEtat() const
{
    return m_etatServiceEtat;
}

bool QMagicBlueLedClient::estConnecte() const
{
    return m_etatConnexion;
}

bool QMagicBlueLedClient::estDetecte() const
{
    return m_magicBlueLedDetecte;
}

void QMagicBlueLedClient::ajouterMagicBlueLed(const QBluetoothDeviceInfo &info)
{
    // Bluetooth Low Energy ?
    if(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        //qDebug() << Q_FUNC_INFO << info.name() << info.address().toString();
        // Magic Blue ?
        if( (!m_nomRecherche.isEmpty() && info.name().startsWith(m_nomRecherche)) || (!m_adresseMACRecherche.isEmpty() && info.address().toString() == m_adresseMACRecherche) || (!m_nomRecherche.isEmpty() && info.name().startsWith(QString::fromUtf8("LEDBlue"))) )
        {
            MagicBlueLed *magicBlueLed = new MagicBlueLed(info.name(), info.address().toString(), this);
            m_devices.append(magicBlueLed);
            m_magicBlueLedDetecte = true;
        }
    }
}

void QMagicBlueLedClient::rechercheTerminee()
{
    m_etatRecherche = false;
    emit rechercheUpdated();
    emit detecteUpdated();
    emit magicBlueLedUpdated();
}

void QMagicBlueLedClient::rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error erreur)
{
    Q_UNUSED(erreur)
    /*if(erreur == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
    {
        qDebug() << Q_FUNC_INFO << "bluetooth non activé !";
    }
    else if(erreur == QBluetoothDeviceDiscoveryAgent::UnknownError)
    {
        qDebug() << Q_FUNC_INFO << "erreur inconnue !";
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "erreur scan !";
    }*/
    m_etatRecherche = false;
    emit rechercheUpdated();
    emit detecteUpdated();
    emit magicBlueLedUpdated();
}

void QMagicBlueLedClient::connecterMagicBlueLed(const QString &adresseServeur)
{
    m_controller =  new QLowEnergyController(QBluetoothAddress(adresseServeur), this);

    // Slot pour la récupération des services
    connect(m_controller, SIGNAL(serviceDiscovered(QBluetoothUuid)), this, SLOT(ajouterService(QBluetoothUuid)));
    connect(m_controller, SIGNAL(connected()), this, SLOT(magicBlueLedConnecte()));
    connect(m_controller, SIGNAL(disconnected()), this, SLOT(magicBlueLedDeconnecte()));
    connect(m_controller, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(connecteErreur(QLowEnergyController::Error)));
    connect(m_controller, SIGNAL(discoveryFinished()), this, SLOT(decouverteServicesTerminee()));

    m_connexionErreur = false;
    emit erreurUpdated();
    m_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
    m_controller->connectToDevice();
}

void QMagicBlueLedClient::connecterService(QLowEnergyService *service)
{
    if(service->state() == QLowEnergyService::DiscoveryRequired)
    {
        // Slot pour la récupération des caractéristiques
        connect(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(serviceDetailsDiscovered(QLowEnergyService::ServiceState)));

        m_etatServiceCommande = false;
        m_etatServiceEtat = false;
        service->discoverDetails();
    }
}

void QMagicBlueLedClient::ajouterService(QBluetoothUuid serviceUuid)
{
    //qDebug() << Q_FUNC_INFO << serviceUuid.toString();
    QLowEnergyService *service = m_controller->createServiceObject(serviceUuid);
    connecterService(service);
}

void QMagicBlueLedClient::decouverteServicesTerminee()
{
    //qDebug() << Q_FUNC_INFO;
}

void QMagicBlueLedClient::serviceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    //qDebug() << Q_FUNC_INFO << c.uuid().toString() << value;
    if(c.uuid().toString() == Q_MAGICBLUELED_CHARACTERISTIC_CONFIGURATION)
        emit etatChanged(value);
}

void QMagicBlueLedClient::serviceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    // découverte ?
    if(newState != QLowEnergyService::ServiceDiscovered)
    {
        return;
    }

    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    //qDebug() << Q_FUNC_INFO << "service" << service->serviceUuid().toString();

    if(service->serviceUuid().toString() == Q_MAGICBLUELED_SERVICE_UUID)
    {
        foreach(QLowEnergyCharacteristic c, service->characteristics())
        {
            //qDebug() << Q_FUNC_INFO << "characteristic" << c.uuid().toString() << "write" << (c.properties() & QLowEnergyCharacteristic::Write);
            if(c.uuid().toString() == Q_MAGICBLUELED_CHARACTERISTIC_UUID)
            {
                m_characteristic = c;
                m_service = service;
                m_etatServiceCommande = true;
                emit serviceCommandeUpdated();
            }
        }
    }
    else if(service->serviceUuid().toString() == Q_MAGICBLUELED_SERVICE_UUID_CONFIGURATION)
    {
        foreach(QLowEnergyCharacteristic c, service->characteristics())
        {
            //qDebug() << Q_FUNC_INFO << "characteristic" << c.uuid().toString() << c.properties();
            if(c.uuid().toString() == Q_MAGICBLUELED_CHARACTERISTIC_CONFIGURATION)
            {
                m_characteristicConfiguration = c;
                m_serviceConfiguration = service;
                // Slot pour le changement d'une caractéristique
                connect(m_serviceConfiguration, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(serviceCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));
                gererNotification(true);
                m_etatServiceEtat = true;
                emit serviceEtatUpdated();
            }
        }
    }
}

void QMagicBlueLedClient::magicBlueLedConnecte()
{
    m_etatConnexion = true;
    emit connecteUpdated();
    m_controller->discoverServices();
}

void QMagicBlueLedClient::magicBlueLedDeconnecte()
{
    m_etatConnexion = false;
    emit connecteUpdated();
}

void QMagicBlueLedClient::connecteErreur(QLowEnergyController::Error error)
{
    //Q_UNUSED(error)
    qDebug() << Q_FUNC_INFO << error;
    m_etatConnexion = false;
    m_connexionErreur = true;
    emit connecteUpdated();
    emit erreurUpdated();
}

void QMagicBlueLedClient::gererNotification(bool notification)
{
    if(m_serviceConfiguration && m_characteristicConfiguration.isValid())
    {
        //qDebug() << Q_FUNC_INFO << m_characteristicConfiguration.uuid().toString() << m_characteristicConfiguration.properties();
        if(m_characteristicConfiguration.properties() & QLowEnergyCharacteristic::Notify)
        {
            QLowEnergyDescriptor descripteurNotification = m_characteristicConfiguration.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if(descripteurNotification.isValid())
            {
                // active la notification : 0100 ou désactive 0000
                if(notification)
                    m_serviceConfiguration->writeDescriptor(descripteurNotification, QByteArray::fromHex("0100"));
                else
                    m_serviceConfiguration->writeDescriptor(descripteurNotification, QByteArray::fromHex("0000"));
            }
        }
    }
}

void QMagicBlueLedClient::write(const QByteArray &data, bool reponse)
{
    if(m_service && m_characteristic.isValid())
    {
        if(m_characteristic.properties() & QLowEnergyCharacteristic::Write)
        {
            if(data.length() <= Q_MAGICBLUELED_MAX_SIZE)
            {
                //qDebug() << Q_FUNC_INFO << m_service->serviceUuid().toString() << m_characteristic.uuid().toString() << data << data.length();
                if(!reponse)
                    m_service->writeCharacteristic(m_characteristic, data, QLowEnergyService::WriteWithoutResponse);
                else
                    m_service->writeCharacteristic(m_characteristic, data, QLowEnergyService::WriteWithResponse);
            }
        }
    }
}

void QMagicBlueLedClient::write(bool etat)
{
    QByteArray datas(3, 0);

    datas[0] = 0xcc;
    if(etat)
        datas[1] = 0x23;
    else
        datas[1] = 0x24;
    datas[2] = 0x33;

    write(datas);
}

QT_END_NAMESPACE
