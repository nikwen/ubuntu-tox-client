/*
 * ubuntu-tox-client
 *
 * Copyright (c) 2015 Niklas Wenzel <nikwen.developer@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOXBACKEND_H
#define TOXBACKEND_H

#include <QObject>
#include <QTimer>
#include <QSocketNotifier>

#include "tox/tox.h"

class ToxBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString toxId READ getToxId NOTIFY toxIdChanged)
    Q_PROPERTY(QString userName READ getOwnUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString statusMessage READ getOwnStatusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit ToxBackend(QObject *parent = 0);
    ~ToxBackend();

    //SIGTERM handling
    static int setUpUnixSignalHandlers();
    static void termSignalHandler(int unused);

public slots:
    QString getToxId() { return m_toxId; }
    QString getOwnUserName();
    QString getOwnStatusMessage();
    bool isConnected() { return m_connected; }

    void setUserName(const QString& name);
    void setStatusMessage(const QString& message);

    void acceptFriendRequest(const QString& userId);

    qint16 getMaximumUserNameLength() { return TOX_MAX_NAME_LENGTH; }
    qint16 getMaximumStatusMessageLength() { return TOX_MAX_STATUSMESSAGE_LENGTH; }

    //SIGTERM handling
    void handleSigTerm();

    //Can also be called from the QML code (necessary for non-Unity8 desktop environments)
    void cleanUpOnClose();

    //The following should only be used by ContactsModel!!!
    Tox *getToxObject() { return tox; }

signals:
    void toxIdChanged();
    void connectedChanged();
    void userNameChanged();
    void statusMessageChanged();

    void friendRequestReceived(const QString& userId, const QString& message);

    void friendAdded(int friendId, const QString& userId);
    void failedToAddFriend(const QString& userId, const QString& errorInfo = QString());

private:
    Tox *tox;
    QTimer *toxTimer;

    QString m_toxId;
    bool m_connected;

    void tick();
    void bootstrap();
    void setToxId(QString id);
    void setConnected(bool connected);
    bool isConnectedCheck();

    QString getConfigurationFilePath();
    void saveTox();
    bool loadTox();

    static void onFriendRequest(Tox* tox, const uint8_t* cUserId, const uint8_t* cMessage, uint16_t cMessageSize, void* backend);

    //SIGTERM handling
    static int sigtermFd[2];
    QSocketNotifier *termSocketNotifier;
};

#endif // TOXBACKEND_H

