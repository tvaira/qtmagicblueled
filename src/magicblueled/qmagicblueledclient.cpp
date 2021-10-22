#include "qmagicblueledclient.h"
#include "qmagicblueled.h"

#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtCore/QtEndian>

QT_BEGIN_NAMESPACE

QMagicBlueLedClient::QMagicBlueLedClient(QObject *parent) : QObject(parent), m_discoveryAgent(nullptr), m_controller(nullptr), m_service(nullptr), m_etatConnexion(false), m_etatRecherche(false), m_connexionErreur(false), m_magicBlueLedDetecte(false)
{
    //qDebug() << Q_FUNC_INFO;
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
    //qDebug() << Q_FUNC_INFO;
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
    emit detecte();
    emit magicBlueLedUpdated();

    //qDebug() << Q_FUNC_INFO << "recherche appareil ble";
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    if(m_discoveryAgent->isActive())
    {
        m_etatRecherche = true;
        emit recherche();
    }
}

void QMagicBlueLedClient::arreter()
{
    if(m_etatRecherche && m_discoveryAgent->isActive())
    {
        //qDebug() << Q_FUNC_INFO << "arret recherche appareil ble";
        m_discoveryAgent->stop();
    }
}

void QMagicBlueLedClient::connecter(QString adresse)
{
    //qDebug() << Q_FUNC_INFO << adresse;
    connecterMagicBlueLed(adresse);
}

void QMagicBlueLedClient::deconnecter()
{
    //qDebug() << Q_FUNC_INFO;
    if(m_controller)
        m_controller->disconnectFromDevice();
}

void QMagicBlueLedClient::read()
{

}

void QMagicBlueLedClient::allumer()
{
    this->write(true);
}

void QMagicBlueLedClient::eteindre()
{
    this->write(false);
}

void QMagicBlueLedClient::write(const QByteArray &data)
{
    if(m_service && m_characteristic.isValid())
    {
        //qDebug() << Q_FUNC_INFO << m_service->serviceUuid().toString();
        if(m_characteristic.properties() & QLowEnergyCharacteristic::Write)
        {
            if(data.length() <= Q_MAGICBLUELED_MAX_SIZE)
            {
                /*qDebug() << Q_FUNC_INFO << m_characteristic.uuid().toString() << data << data.length();
                if(data.length() == 3)
                    qDebug("%02X %02X %02X", (unsigned char)data[0], data[1], data[2]);
                else if(data.length() == 7)
                    qDebug("%02X %02X %02X %02X %02X %02X %02X", (unsigned char)data[0], (unsigned char)data[1], (unsigned char)data[2], (unsigned char)data[3], (unsigned char)data[4], (unsigned char)data[5], (unsigned char)data[6]);*/
                m_service->writeCharacteristic(m_characteristic, data, QLowEnergyService::WriteWithoutResponse);
            }
        }
    }
}

void QMagicBlueLedClient::write(int rouge, int vert, int bleu, int white/*=0*/)
{
    Q_UNUSED(white)
    QByteArray datas(7, 0);

    datas[0] = 0x56;
    datas[1] = static_cast<char>(rouge); //RR
    datas[2] = static_cast<char>(vert); // GG
    datas[3] = static_cast<char>(bleu); // BB
    datas[4] = 0x00; // WW
    datas[5] = 0xf0;
    datas[6] = 0xaa;

    write(datas);
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

void QMagicBlueLedClient::gererNotification(bool notification)
{
    if(m_service && m_characteristic.isValid())
    {
        if(m_characteristic.properties() & QLowEnergyCharacteristic::Notify)
        {
            QLowEnergyDescriptor descripteurNotification = m_characteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if(descripteurNotification.isValid())
            {
                // active la notification : 0100 ou désactive 0000
                //qDebug() << Q_FUNC_INFO << "modification notification" << m_characteristic.uuid().toString() << notification;
                if(notification)
                    m_service->writeDescriptor(descripteurNotification, QByteArray::fromHex("0100"));
                else
                    m_service->writeDescriptor(descripteurNotification, QByteArray::fromHex("0000"));
            }
        }
    }
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
        if( (!m_nomRecherche.isEmpty() && info.name().startsWith(m_nomRecherche)) || (!m_adresseMACRecherche.isEmpty() && info.address().toString() == m_adresseMACRecherche) )
        {
            //qDebug() << Q_FUNC_INFO << "magic blue ble" << info.name() << info.address().toString();
            MagicBlueLed *magicBlueLed = new MagicBlueLed(info.name(), info.address().toString(), this);
            m_devices.append(magicBlueLed);
            m_magicBlueLedDetecte = true;
        }
    }
}

void QMagicBlueLedClient::rechercheTerminee()
{
    m_etatRecherche = false;
    emit recherche();
    emit detecte();
    emit magicBlueLedUpdated();
    /*if (m_devices.isEmpty())
        qDebug() << Q_FUNC_INFO << "recherche terminee : aucun appareil ble trouve !";
    else
        qDebug() << Q_FUNC_INFO << "recherche terminee";*/
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
    emit recherche();
    emit detecte();
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

    //qDebug() << Q_FUNC_INFO << "demande de connexion";
    m_connexionErreur = false;
    emit erreurUpdated();
    m_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
    m_controller->connectToDevice();
}

void QMagicBlueLedClient::connecterService(QLowEnergyService *service)
{
    m_service = service;

    if(m_service->state() == QLowEnergyService::DiscoveryRequired)
    {
        // Slot pour le changement d'une caractéristique
        connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(serviceCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));
        // Slot pour la récupération des caractéristiques
        connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(serviceDetailsDiscovered(QLowEnergyService::ServiceState)));

        //qDebug() << Q_FUNC_INFO << "découverte des détails des services";
        m_service->discoverDetails();
    }
}

void QMagicBlueLedClient::ajouterService(QBluetoothUuid serviceUuid)
{
    //qDebug() << Q_FUNC_INFO << serviceUuid.toString();
    QLowEnergyService *service = m_controller->createServiceObject(serviceUuid);
    connecterService(service);
}

void QMagicBlueLedClient::serviceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    Q_UNUSED(c)
    Q_UNUSED(value)
    //qDebug() << Q_FUNC_INFO << c.uuid().toString() << value;
}

void QMagicBlueLedClient::serviceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    //Q_UNUSED(newState)

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
            //qDebug() << Q_FUNC_INFO << "characteristic" << c.uuid().toString();
            if(c.uuid().toString() == Q_MAGICBLUELED_CHARACTERISTIC_UUID)
            {
                //qDebug() << Q_FUNC_INFO << "my characteristic" << c.uuid().toString() << (c.properties() & QLowEnergyCharacteristic::Write);
                m_characteristic = c;
                m_service = service;
            }

            m_etatConnexion = true;
            emit connecte();
        }
    }
}

void QMagicBlueLedClient::magicBlueLedConnecte()
{
    //qDebug() << Q_FUNC_INFO;
    m_controller->discoverServices();
}

void QMagicBlueLedClient::magicBlueLedDeconnecte()
{
    //qDebug() << Q_FUNC_INFO;
    m_etatConnexion = false;
    emit connecte();
}

void QMagicBlueLedClient::connecteErreur(QLowEnergyController::Error error)
{
    //Q_UNUSED(error)
    qDebug() << Q_FUNC_INFO << error;
    m_etatConnexion = false;
    m_connexionErreur = true;
    emit connecte();
    emit erreurUpdated();
}

QT_END_NAMESPACE
