#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMagicBlueLedClient>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QMagicBlueLedClient *magicBlueLedClient;

public slots:
    void connecte();
    void recherche();
    void serviceCommande();
    void serviceEtat();
    void erreur();
    void detecte();
    void magicBlueLedUpdated();
    void etatChanged(const QByteArray &datas);
};

#endif // MAINWINDOW_H
