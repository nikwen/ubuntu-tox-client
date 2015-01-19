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

#include <QDebug>
#include <QSaveFile>
#include <QStandardPaths>
#include <QDir>

#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>

#include "toxbackend.h"
#include "cstring.h"
#include "cdata.h"
#include "bootstrapnodes.h"

//Static members from header file:
int ToxBackend::sigtermFd[2];

ToxBackend::ToxBackend(QObject *parent) :
    QObject(parent),
    m_toxId(""),
    m_connected(false)
{
    //Setup SIGTERM handlers to get notified when the application is closed

    int setUpHandlersResult = ToxBackend::setUpUnixSignalHandlers();

    if (setUpHandlersResult > 0) {
        qFatal("sigaction(SIGTERM) exited with return code > 0");
    } else {
        if (::socketpair(AF_UNIX, SOCK_STREAM, 0, ToxBackend::sigtermFd)) {
            qFatal("Couldn't create TERM socketpair");
        }
        termSocketNotifier = new QSocketNotifier(ToxBackend::sigtermFd[1], QSocketNotifier::Read, this);
        connect(termSocketNotifier, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
    }

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

    if (tox == NULL) {
        qCritical() << "Tox core failed to start";
        return;
    }

    //Try to load saved tox status

    if (!loadTox()) {
        //Set a default username and status message

        setUserName(QString("Ubuntu User"));
        setStatusMessage(QString("Toxing from my Ubuntu phone"));
    }

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

    //Start main loop
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

void ToxBackend::setToxId(QString id) { //TODO: Better solution
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
        saveTox();
        emit friendAdded(friendId, userId);
    }
}

void ToxBackend::sendFriendRequest(const QString &address, const QString &message) {
    const QString userId = address.mid(0, TOX_CLIENT_ID_SIZE * 2);

    if (hasFriendWithAddress(address)) { //TODO: Think about removing this as it is already handled by tox_add_friend
        emit failedToAddFriend(userId, TOX_FAERR_ALREADYSENT);
    } else {
        CString cMessage(message);
        int friendId = tox_add_friend(tox, CFriendAddress(address).data(), cMessage.data(), cMessage.size());
        if (friendId < 0) {
            emit failedToAddFriend(userId, friendId);
        } else {
            emit friendAdded(friendId, userId);
        }
    }

    saveTox();
}

bool ToxBackend::hasFriendWithPublicKey(const QString &publicKey) const {
    //Return if public key is too short
    if (publicKey.length() != (TOX_CLIENT_ID_SIZE * 2)) {
        return false;
    }

    const uint32_t friendCount = tox_count_friendlist(tox);

    if (friendCount > 0) {
        int32_t *ids = new int32_t[friendCount];
        tox_get_friendlist(tox, ids, friendCount);

        for (int32_t i = 0; i < static_cast<int32_t>(friendCount); ++i) {
            QString friendUserId = getFriendUserId(ids[i]);
            if (friendUserId.toUpper().startsWith(publicKey.toUpper())) {
                delete[] ids;
                return true;
            }
        }
        delete[] ids;
    }
    return false;
}

QString ToxBackend::getFriendUserId(int friendNumber) const {
    uint8_t rawid[TOX_CLIENT_ID_SIZE];
    tox_get_client_id(tox, friendNumber, rawid);
    QByteArray data((char*) rawid, TOX_CLIENT_ID_SIZE);
    QString id = data.toHex().toUpper();

    return id;
}

bool ToxBackend::hasFriendWithAddress(const QString &address) const {
    //Return if address is too short
    if (address.length() != (TOX_FRIEND_ADDRESS_SIZE * 2)) {
        return false;
    }

    QString publicKey = address.left(TOX_CLIENT_ID_SIZE * 2);
    return hasFriendWithPublicKey(publicKey);
}

void ToxBackend::setUserName(const QString& name) {
    CString cName(name);
    tox_set_name(tox, cName.data(), std::min((int) cName.size(), TOX_MAX_NAME_LENGTH));
    saveTox();
    emit userNameChanged();
}

void ToxBackend::setStatusMessage(const QString& message) {
    CString cMessage(message);
    tox_set_status_message(tox, cMessage.data(), std::min((int) cMessage.size(), TOX_MAX_STATUSMESSAGE_LENGTH));
    saveTox();
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

QString ToxBackend::getConfigurationFilePath() {
    //Create app data directory if it does not exist

    QDir dataDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (!dataDir.exists() && !dataDir.mkpath(dataDir.absolutePath())) {
        qCritical() << "Failed to create directory:" << dataDir;
        return nullptr;
    }

    //Return path for file "save.txt" in dataDir

    QString path = dataDir.absoluteFilePath("save.tox"); //TODO: Change file name?

    return path;
}

void ToxBackend::saveTox() {
    if (!tox) {
        qWarning() << "Tox not started, will not save tox status!";
        return;
    }

    qDebug() << "Saving tox status";

    //Open file to save tox status to

    QString path = getConfigurationFilePath();
    if (path == nullptr) {
        qCritical() << "Failed to receive configuration file path";
        return;
    }

    QSaveFile configurationFile(path);
    if (!configurationFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open file:" << path;
        return;
    }

    qDebug() << "Saving tox status to" << path;

    //Get tox size and save tox data

    uint32_t fileSize = tox_size(tox);

    if (fileSize > 0 && fileSize <= INT32_MAX) {
        uint8_t *data = new uint8_t[fileSize];
        tox_save(tox, data);
        configurationFile.write(reinterpret_cast<char *>(data), fileSize);
        configurationFile.commit();
        delete[] data;

        qDebug() << "Saved tox status";
    } else {
        qCritical() << "Invalid fileSize for tox status";
    }
}

/*
 * Returns whether loading was successful
 */
bool ToxBackend::loadTox() {
    qDebug() << "Loading tox status";

    //Open file to load the tox status from

    QString path = getConfigurationFilePath();
    if (path == nullptr) {
        qCritical() << "Failed to receive configuration file path";
        return false;
    }

    QFile configurationFile(path);

    if (!configurationFile.exists()) {
        qWarning() << "File does not exist:" << path;
        return false;
    }

    if (!configurationFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open file:" << path;
        return false;
    }

    //Get file size and load tox data

    bool returnValue;

    qint64 fileSize = configurationFile.size();
    if (fileSize > 0) {
        QByteArray data = configurationFile.readAll();
        int error = tox_load(tox, reinterpret_cast<uint8_t *>(data.data()), data.size());

        if (error != 0) {
            qWarning() << "tox_load failed with error" << error;
            returnValue = false;
        } else {
            qDebug() << "Loaded tox status";
            returnValue = true;
        }
    }
    configurationFile.close();

    return returnValue;
}

void ToxBackend::cleanUpOnClose() {
    saveTox();

    qDebug() << "Killing tox...";

    if (tox) {
        tox_kill(tox);
        tox = nullptr;
    }
}

//SIGTERM handling

int ToxBackend::setUpUnixSignalHandlers() {
    struct sigaction term;

    term.sa_handler = ToxBackend::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0) {
        return 2;
    }

    return 0;
}

void ToxBackend::termSignalHandler(int) {
    char a = 1;
    ::write(ToxBackend::sigtermFd[0], &a, sizeof(a));
}

void ToxBackend::handleSigTerm() {
    termSocketNotifier->setEnabled(false);
    char tmp;
    ::read(ToxBackend::sigtermFd[1], &tmp, sizeof(tmp));

    qDebug() << "SIGTERM received";

    cleanUpOnClose();

    termSocketNotifier->setEnabled(true);
}
