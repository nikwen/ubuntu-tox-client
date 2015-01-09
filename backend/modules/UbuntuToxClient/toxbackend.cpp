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

#include "toxbackend.h"
#include "QDebug"
#include "cstring.h"
#include "cdata.h"

#include "bootstrapnodes.h"

ToxBackend::ToxBackend(QObject *parent) :
    QObject(parent),
    m_toxId(""),
    m_connected(false)
{
    //Create timer for tox "main loop"

    toxTimer = new QTimer(this);
    toxTimer->setSingleShot(true);
    connect(toxTimer, &QTimer::timeout, this, &ToxBackend::tick);

    //Create tox object

    Tox_Options toxOptions;
    toxOptions.proxy_type = TOX_PROXY_NONE;
    toxOptions.proxy_address[0] = 0;
    toxOptions.proxy_port = 0;

    tox = tox_new(&toxOptions);

    if (tox == NULL)
    {
        qCritical() << "Tox core failed to start";
        return;
    }

    //Set the username

    setUserName(QString("Ubuntu User"));

    //Set the status message

    setStatusMessage(QString("Toxing from my Ubuntu phone"));

    //Set the user status

    tox_set_user_status(tox, TOX_USERSTATUS_NONE);

    //Set handlers for events
    tox_callback_friend_request(tox, onFriendRequest, this);

    //Get own id:

    uint8_t friendAddress[TOX_FRIEND_ADDRESS_SIZE];
    tox_get_address(tox, friendAddress);
    QString addressString = CFriendAddress::toString(friendAddress);

    qDebug() << "Tox-ID:" << addressString;
//    qDebug() << "Public Key:" << addressString.left(64);
//    qDebug() << "Nospam:" << addressString.mid(64, 8);
//    qDebug() << "Checksum:" << addressString.mid(64 + 8, 4);

    setToxId(addressString);

    //Start loop
    tick();
}

ToxBackend::~ToxBackend() {

}

bool ToxBackend::isConnectedCheck() {
    setConnected(tox_isconnected(tox));
    return m_connected;
}

void ToxBackend::setConnected(bool connected) {
    if (m_connected != connected) {
        m_connected = connected;
        emit connectedChanged();
    }
}

void ToxBackend::setToxId(QString id) {
    m_toxId = id;
    emit toxIdChanged();
}

#define CORE_DISCONNECT_TOLERANCE 30

/*
 * The body of the tox "main loop"
 * Executes tox_do()
 */
void ToxBackend::tick() {
    static int tolerance = CORE_DISCONNECT_TOLERANCE;

    tox_do(tox);

    if (isConnectedCheck())
        tolerance = CORE_DISCONNECT_TOLERANCE;
    else if (!(--tolerance)) {
        qDebug() << "Bootstrapping...";
        bootstrap();
    }

    toxTimer->start(tox_do_interval(tox));
}

/*
 * Bootstrap to the DHT using the set of bootstrap nodes in bootstrapnodes.h
 */
void ToxBackend::bootstrap() {
    int nodeCount = sizeof(bootstrapNodes)/sizeof(bootstrapNodes[0]);

    int i = 0;
    static unsigned int j = qrand() % nodeCount;

    while (i < 4) {
        struct BootstrapNode *d = &bootstrapNodes[j % nodeCount];
        tox_bootstrap_from_address(tox, d->address, d->port, d->key);
        i++;
        j++;
    }
}

void ToxBackend::onFriendRequest(Tox*, const uint8_t* cUserId, const uint8_t* cMessage, uint16_t cMessageSize, void* backend) {
    emit static_cast<ToxBackend*>(backend)->friendRequestReceived(CUserId::toString(cUserId), CString::toString(cMessage, cMessageSize));
}

void ToxBackend::acceptFriendRequest(const QString& userId) {
    int friendId = tox_add_friend_norequest(tox, CUserId(userId).data());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
    }
}

void ToxBackend::setUserName(const QString& name) {
    CString cName(name);
    tox_set_name(tox, cName.data(), std::min((int) cName.size(), TOX_MAX_NAME_LENGTH));
    emit userNameChanged();
}

void ToxBackend::setStatusMessage(const QString& message) {
    CString cMessage(message);
    tox_set_status_message(tox, cMessage.data(), std::min((int) cMessage.size(), TOX_MAX_STATUSMESSAGE_LENGTH));
    emit statusMessageChanged();
}

QString ToxBackend::getOwnUserName() {
    QString name;

    int size = tox_get_self_name_size(tox);
    uint8_t* nameData = new uint8_t[size];

    if (tox_get_self_name(tox, nameData) == size) {
        name = CString::toString(nameData, size);
    }
    delete[] nameData;

    return name;
}

QString ToxBackend::getOwnStatusMessage() {
    QString message;

    int size = tox_get_self_status_message_size(tox);
    uint8_t* messageData = new uint8_t[size];

    if (tox_get_self_status_message(tox, messageData, size) == size) {
        message = CString::toString(messageData, size);
    }
    delete[] messageData;

    return message;
}
