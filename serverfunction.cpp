#include "serverfunction.h"
#include "QtCore/qdebug.h"

QMap <QString, QList<Participant>> queue;

QByteArray parse(QByteArray request, QTcpSocket* socket) {
    QStringList splitted_request = QString(request).trimmed().split("&");
    qDebug() << splitted_request;
    QString response;

    QString method = splitted_request[0];
    if (method == QString("start")) {
        QString login = splitted_request[1];
        QString roomName = splitted_request[2];
        response = queue_enter(login, roomName, socket);
    }
    else if (method == QString("break")) {
        QString login = splitted_request[1];
        response = queue_leave(login);
    }
    else if (method == QString("stats")) {
        QString login = splitted_request[1];
        response = queue_stat(login);
    }
    else if (method == QString("rooms")) {
        response = rooms_list();
    }
    else if (method == QString("newroom")) {
        QString roomName = splitted_request[1];
        response = room_create(roomName);
    }
    else {
        response = QString("Function is not defined");
    }
    qDebug() << response;
    return QByteArray((response + "\r\n").toUtf8());
}


QString queue_enter(QString login, QString roomName, QTcpSocket* socket) {
    if (!queue.contains(roomName)) {
        return QString("fail&roomNotExist");
    }

    for (const Participant& p : queue.value(roomName)) {
        if (p.login == login) {
        return QString("fail&alreadyInQueue");
        }
    }

    queue[roomName].append({login, socket});
    int position = queue[roomName].size(); // номер человека в очереди

    if (position >= 7) {
        for (const Participant& p : queue.value(roomName)) {
            p.socket->close();
        }
    }
    else {
        return QString("success&%1").arg(position);
    }
}

QString rooms_list() {
    QString response;
    for (auto it = queue.cbegin(); it != queue.cend(); it++) {
        QString roomName = it.key();
        int peopleCount = it.value().size();
        if (peopleCount < 0) peopleCount = 0;
        response.append(QString("%1-%2&").arg(roomName).arg(peopleCount));
    }
    return response;
}

QString room_create(QString roomName) {
    if (!queue.contains(roomName)) { // если комната с таким названием уже существует
        queue.insert(roomName, QList<Participant>());
        return QString("success");
    }
    else {
        return QString("fail");
    }
}

QString queue_leave(QString login) {
    bool userFound = false;
    for (auto& participants : queue) {
        for (int i = 0; i < participants.size(); ++i) {
            if (participants[i].login == login) {
                participants.removeAt(i);
                userFound = true;
                break;
            }
        }
    }
    if (userFound) {
        return QString("success");
    } else {
        return QString("fail&userNotExist");
    }
}

QString queue_stat(QString roomName) {
    QString response;
    for (const Participant& p : queue.value(roomName)) {
        response.append(p.login + "&");
    }
    return response;
}
