#include "contactsmodel.h"

#include <QDebug>

#include "cstring.h"

ContactsModel::ContactsModel(QObject *parent) :
    QAbstractListModel(parent),
    friendMap(QMap<int, Friend>())
{
    connect(this, &ContactsModel::backendChanged, this, &ContactsModel::init);
}

int ContactsModel::rowCount(const QModelIndex &parent) const {
    if (tox == nullptr)
        return 0;

    return tox_count_friendlist(tox);
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
        Friend f = friendMap.values().at(index.row());
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

    //Clear friend map
    friendMap.clear();

    //Get friends

    const uint32_t friendCount = tox_count_friendlist(tox);
    if (friendCount > 0) {
        //Get the list of friend ids
        int32_t *ids = new int32_t[friendCount];
        tox_get_friendlist(tox, ids, friendCount);
        uint8_t clientId[TOX_CLIENT_ID_SIZE];

        //Create new friend objects for all retrieved friends
        for (int32_t i = 0; i < static_cast<int32_t>(friendCount); ++i) {
            if (tox_get_client_id(tox, ids[i], clientId) == 0) {
                Friend f;

                //Get the friend's name
                const int nameSize = tox_get_name_size(tox, ids[i]);
                if (nameSize > 0) {
                    uint8_t *name = new uint8_t[nameSize];
                    if (tox_get_name(tox, ids[i], name) == nameSize) {
                        f.setName(CString::toString(name, nameSize));
                    }
                    delete[] name;
                }

                //Get the friend's status message
                const int statusMessageSize = tox_get_status_message_size(tox, ids[i]);
                if (statusMessageSize > 0) {
                    uint8_t *statusMessage = new uint8_t[statusMessageSize];
                    if (tox_get_status_message(tox, ids[i], statusMessage, statusMessageSize) == statusMessageSize) {
                        f.setStatusMessage(CString::toString(statusMessage, statusMessageSize));
                    }
                    delete[] statusMessage;
                }

                //Add friend to friend map
                friendMap[ids[i]] = f;
            }
        }
        delete[] ids;
    }

    qDebug() << "ContactsModel init() finished";

    connect(this, &ContactsModel::friendNameChanged, this, &ContactsModel::setFriendName);
    connect(this, &ContactsModel::friendStatusMessageChanged, this, &ContactsModel::setFriendStatusMessage);

    tox_callback_friend_action(tox, onFriendAction, this);
    tox_callback_name_change(tox, onFriendNameChange, this);
    tox_callback_status_message(tox, onFriendStatusMessageChanged, this);
}

void ContactsModel::setFriendName(int id, QString name) {
    Friend f = friendMap[id];
    f.setName(name);
    friendMap[id] = f;
    QModelIndex updateIndex = index(0, 0, QModelIndex()); //TODO: Currently only works with one contact, find out real index!!!
    emit dataChanged(updateIndex, updateIndex);
}

void ContactsModel::setFriendStatusMessage(int id, QString message) {
    Friend f = friendMap[id];
    f.setStatusMessage(message);
    friendMap[id] = f;
    QModelIndex updateIndex = index(0, 0, QModelIndex()); //TODO: Currently only works with one contact, find out real index!!!
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
