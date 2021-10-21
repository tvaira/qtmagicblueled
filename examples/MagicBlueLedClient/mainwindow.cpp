#include "mainwindow.h"
#include <QDebug>
#include "qmagicblueled.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    magicBlueLedClient = new QMagicBlueLedClient(this);
    connect(magicBlueLedClient, SIGNAL(connecte()), this, SLOT(connecte()));
    connect(magicBlueLedClient, SIGNAL(recherche()), this, SLOT(recherche()));
    connect(magicBlueLedClient, SIGNAL(erreur()), this, SLOT(erreur()));
    connect(magicBlueLedClient, SIGNAL(detecte()), this, SLOT(detecte()));
    connect(magicBlueLedClient, SIGNAL(magicBlueLedUpdated()), this, SLOT(magicBlueLedUpdated()));

    magicBlueLedClient->rechercher();
}

MainWindow::~MainWindow()
{
    magicBlueLedClient->write(false);
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::connecte()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->etatConnexion();
    if(magicBlueLedClient->etatConnexion())
    {
        magicBlueLedClient->write(true);
    }
    else
    {
        magicBlueLedClient->write(false);
    }
}

void MainWindow::recherche()
{
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::erreur()
{
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::detecte()
{
    qDebug() << Q_FUNC_INFO << magicBlueLedClient->magicBlueLedDetecte();
}

void MainWindow::magicBlueLedUpdated()
{
    qDebug() << Q_FUNC_INFO;
    if(magicBlueLedClient->magicBlueLedDetecte())
    {
        QList<MagicBlueLed*> liste = magicBlueLedClient->getMagicBlueLed();
        for (int i = 0; i < liste.size(); ++i)
        {
            qDebug() << Q_FUNC_INFO << liste[i]->nom() << liste[i]->adresseMAC();
            magicBlueLedClient->start(liste[i]->adresseMAC());
        }
    }
}
