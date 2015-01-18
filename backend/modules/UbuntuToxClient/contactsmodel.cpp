#include "contactsmodel.h"

#include <QDebug>

#include "cstring.h"

ContactsModel::ContactsModel(QObject *parent) :
    QAbstractListModel(parent),
    friendList(QList<Friend>())
{
    connect(this, &ContactsModel::backendChanged, this, &ContactsModel::init);
}

int ContactsModel::rowCount(const QModelIndex &parent) const {
    if (tox == nullptr)
        return 0;

    return friendList.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (tox == nullptr)
        return QVariant();

    if (!index.isValid())
        return QVariant();

    if (index.row() >= tox_count_friendlist(tox))
        return QVariant();

    if (role == Qt::DisplayRole) {
        Friend f = friendList.at(index.row());
        return f.getName();
    }
    else
        return QVariant();
}

void ContactsModel::init() {
    if (backend == nullptr)
        return;

    qDebug() << "Got backend";
    tox = backend->getToxObject();

    qDebug() << "Getting friends...";

    //Clear friend list
    friendList.clear();

    //Get friends

    const uint32_t friendCount = tox_count_friendlist(tox);
    if (friendCount > 0) {
        //Get the list of friend ids
        int32_t *ids = new int32_t[friendCount];
        tox_get_friendlist(tox, ids, friendCount);

        //Create new friend objects for all retrieved friends
        for (int32_t i = 0; i < static_cast<int32_t>(friendCount); ++i) {
            addFriendToModel(ids[i], ""); //TODO: User-ID
        }
        delete[] ids;
    }

    qDebug() << "ContactsModel init() finished";

    connect(this, &ContactsModel::friendNameChanged, this, &ContactsModel::setFriendName);
    connect(this, &ContactsModel::friendStatusMessageChanged, this, &ContactsModel::setFriendStatusMessage);

    connect(backend, &ToxBackend::friendAdded, this, &ContactsModel::addFriendToModel);

    tox_callback_friend_action(tox, onFriendAction, this);
    tox_callback_name_change(tox, onFriendNameChange, this);
    tox_callback_status_message(tox, onFriendStatusMessageChanged, this);
}

void ContactsModel::setFriendName(int id, QString name) {
    //Get friend with given id from friend list
    Friend f; int i;
    for (i = 0; i < friendList.size(); i++) {
        f = friendList[i];
        if (f.getFriendId() == id) {
            break;
        }
    }

    if (i >= friendList.size())
        return;

    //Set name and replace in list
    f.setName(name);
    friendList[i] = f;

    //Notify ListView of changes
    QModelIndex updateIndex = index(i, 0, QModelIndex());
    emit dataChanged(updateIndex, updateIndex);
}

void ContactsModel::setFriendStatusMessage(int id, QString message) {
    //Get friend with given id from friend list
    Friend f; int i;
    for (i = 0; i < friendList.size(); i++) {
        f = friendList[i];
        if (f.getFriendId() == id) {
            break;
        }
    }

    if (i >= friendList.size())
        return;

    //Set name and replace in list
    f.setStatusMessage(message);
    friendList[i] = f;

    //Notify ListView of changes
    QModelIndex updateIndex = index(i, 0, QModelIndex());
    emit dataChanged(updateIndex, updateIndex);
}

void ContactsModel::onFriendAction(Tox*/* tox*/, int friendId, const uint8_t *cMessage, uint16_t cMessageSize, void *model) {
    emit static_cast<ContactsModel*>(model)->friendMessageReceived(friendId, CString::toString(cMessage, cMessageSize), true);
}

void ContactsModel::onFriendNameChange(Tox*/* tox*/, int friendId, const uint8_t* cName, uint16_t cNameSize, void* model) {
    emit static_cast<ContactsModel*>(model)->friendNameChanged(friendId, CString::toString(cName, cNameSize));
}

void ContactsModel::onFriendStatusMessageChanged(Tox*/* tox*/, int friendId, const uint8_t* cMessage, uint16_t cMessageSize, void* model) {
    emit static_cast<ContactsModel*>(model)->friendStatusMessageChanged(friendId, CString::toString(cMessage, cMessageSize));
}

void ContactsModel::addFriendToModel(int friendId, const QString &userId) {
    uint8_t clientId[TOX_CLIENT_ID_SIZE];

    if (tox_get_client_id(tox, friendId, clientId) == 0) {
        Friend f;
        f.setFriendId(friendId);

        //Get the friend's name
        const int nameSize = tox_get_name_size(tox, friendId);
        if (nameSize > 0) {
            uint8_t *name = new uint8_t[nameSize];
            if (tox_get_name(tox, friendId, name) == nameSize) {
                f.setName(CString::toString(name, nameSize));
            }
            delete[] name;
        }

        //Get the friend's status message
        const int statusMessageSize = tox_get_status_message_size(tox, friendId);
        if (statusMessageSize > 0) {
            uint8_t *statusMessage = new uint8_t[statusMessageSize];
            if (tox_get_status_message(tox, friendId, statusMessage, statusMessageSize) == statusMessageSize) {
                f.setStatusMessage(CString::toString(statusMessage, statusMessageSize));
            }
            delete[] statusMessage;
        }

        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        //Add friend to friend list
        friendList << f;

        endInsertRows();

        qDebug() << "Added friend";
    }
}
