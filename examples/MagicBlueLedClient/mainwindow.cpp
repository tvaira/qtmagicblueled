#include "mainwindow.h"
#include <QDebug>
#include "qmagicblueled.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    magicBlueLedClient = new QMagicBlueLedClient(this);
    connect(magicBlueLedClient, SIGNAL(connecteUpdated()), this, SLOT(connecte()));
    connect(magicBlueLedClient, SIGNAL(rechercheUpdated()), this, SLOT(recherche()));
    connect(magicBlueLedClient, SIGNAL(serviceCommandeUpdated()), this, SLOT(serviceCommande()));
    connect(magicBlueLedClient, SIGNAL(serviceEtatUpdated()), this, SLOT(serviceEtat()));
    connect(magicBlueLedClient, SIGNAL(erreurUpdated()), this, SLOT(erreur()));
    connect(magicBlueLedClient, SIGNAL(detecteUpdated()), this, SLOT(detecte()));
    connect(magicBlueLedClient, SIGNAL(magicBlueLedUpdated()), this, SLOT(magicBlueLedUpdated()));
    connect(magicBlueLedClient, SIGNAL(etatChanged(const QByteArray &)), this, SLOT(etatChanged(const QByteArray &)));

    magicBlueLedClient->rechercher();
}

MainWindow::~MainWindow()
{
    if(magicBlueLedClient->estConnecte())
        magicBlueLedClient->eteindre();
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::connecte()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->etatConnexion() << magicBlueLedClient->estConnecte();
}

void MainWindow::recherche()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->etatRecherche();
}

void MainWindow::serviceCommande()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->etatServiceCommande();
    if(magicBlueLedClient->etatServiceCommande())
    {
        magicBlueLedClient->allumer();
        // Exemples :
        magicBlueLedClient->commanderRGB(127, 20, 80); // en couleur
        //magicBlueLedClient->commander(20); // en blanc
    }
}

void MainWindow::serviceEtat()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->etatServiceEtat();
    if(magicBlueLedClient->etatServiceEtat())
        magicBlueLedClient->lireEtat();
}

void MainWindow::erreur()
{
    if(magicBlueLedClient->connexionErreur())
        qDebug() << Q_FUNC_INFO;
}

void MainWindow::detecte()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->magicBlueLedDetecte();
}

void MainWindow::magicBlueLedUpdated()
{
    //qDebug() << Q_FUNC_INFO;
    if(magicBlueLedClient->estDetecte())
    {
        QList<MagicBlueLed*> liste = magicBlueLedClient->getMagicBlueLed();
        for (int i = 0; i < liste.size(); ++i)
        {
            qDebug() << Q_FUNC_INFO << liste[i]->nom() << liste[i]->adresseMAC();
            magicBlueLedClient->connecter(liste[i]->adresseMAC());
        }
    }
}

void MainWindow::etatChanged(const QByteArray &datas)
{
    //qDebug() << Q_FUNC_INFO;
    /*for(int i = 0; i < datas.length(); ++i)
    {
        qDebug("%02X", (unsigned char)datas[i]);
    }*/
    if(datas[0] == (char)0x66 && datas[11] == (char)0x99)
    {
        // DeviceType
        if(datas[QMagicBlueLedClient::DeviceType] == QMagicBlueLedClient::RBGW_Bulb_New)
            qDebug() << Q_FUNC_INFO << "RBGW_Bulb_New";
        else if(datas[QMagicBlueLedClient::DeviceType] == QMagicBlueLedClient::RGB)
            qDebug() << Q_FUNC_INFO << "RGB";
        else if(datas[QMagicBlueLedClient::DeviceType] == QMagicBlueLedClient::RBGW_UFO)
            qDebug() << Q_FUNC_INFO << "RBGW_UFO";
        else
            qDebug() << Q_FUNC_INFO << "Type inconnu !";
        if(datas[QMagicBlueLedClient::On_Off] == QMagicBlueLedClient::ON)
            qDebug() << Q_FUNC_INFO << "Allumee";
        else
            qDebug() << Q_FUNC_INFO << "Eteinte";
        /*if(datas[QMagicBlueLedClient::Running] == 0x21)
            qDebug() << Q_FUNC_INFO << "Etat : isRunning";
        else
            qDebug() << Q_FUNC_INFO << "Etat : notRunning";*/
        qDebug() << Q_FUNC_INFO << "LedVersionNum" << (unsigned int)datas[QMagicBlueLedClient::LedVersionNum];
        qDebug() << Q_FUNC_INFO << "Red" << (unsigned int)datas[QMagicBlueLedClient::Red];
        qDebug() << Q_FUNC_INFO << "Green" << (unsigned int)datas[QMagicBlueLedClient::Green];
        qDebug() << Q_FUNC_INFO << "Blue" << (unsigned int)datas[QMagicBlueLedClient::Blue];
        qDebug() << Q_FUNC_INFO << "White" << (unsigned int)datas[QMagicBlueLedClient::White];
    }
    /*
        0  66 START ?
        1  15 TYPE
        2  23 ON/OFF
        3  4B ???
        4  41 isRunning ?
        5  02 ???
        6  00 RR
        7  00 GG
        8  00 BB
        9  14 WW
        10 0A LedVersionNum
        11 99 END ?

        DeviceType :
            TYPE_RBGW_Bulb_New : red, green, blue, (byte) 0, (byte) 0xf0
            TYPE_RBG : red, green, blue
            TYPE_RBGW_UFO : red, green, blue, (byte) 0, (byte) 0xf0
    */
}
