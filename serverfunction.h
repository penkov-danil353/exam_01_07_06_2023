#ifndef SERVERFUNCTION_H
#define SERVERFUNCTION_H
#include <QtNetwork>
#include <QString>
#include <sstream>
#include <vector>
#include <QSqlQuery>

QByteArray parse(QByteArray request, QTcpSocket* socket);
QString queue_enter(QString login, QString roomName, QTcpSocket* socket);
QString queue_leave(QString login);
QString queue_stat(QString roomName);
QString rooms_list();
QString room_create(QString roomName);


struct Participant {
    QString login;
    QTcpSocket* socket;
};
extern QMap <QString, QList<Participant>> queue;


#endif // SERVERFUNCTION_H
