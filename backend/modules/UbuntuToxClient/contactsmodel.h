#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>

#include "tox/tox.h"
#include "toxbackend.h"
#include "friend.h"

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ToxBackend *toxBackend MEMBER backend NOTIFY backendChanged)

public:
    explicit ContactsModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void backendChanged();

    void friendMessageReceived(int friendId, const QString& message, bool isAction);
    void friendNameChanged(int friendId, const QString& name);
    void friendStatusMessageChanged(int friendId, const QString& message);

public slots:
    void init();

    void setFriendName(int id, QString name);
    void setFriendStatusMessage(int id, QString message);

    void addFriendToModel(int friendId, const QString& userId);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    ToxBackend *backend;
    Tox *tox = nullptr;

    QList<Friend> friendList;

    static void onFriendAction(Tox*/* tox*/, int friendId, const uint8_t *cMessage, uint16_t cMessageSize, void *model);
    static void onFriendNameChange(Tox* tox, int friendId, const uint8_t* cName, uint16_t cNameSize, void* model);
    static void onFriendStatusMessageChanged(Tox* tox, int friendId, const uint8_t* cMessage, uint16_t cMessageSize, void* model);

    enum Roles {
        StatusMessageRole = Qt::UserRole+100,
    };


};

#endif // CONTACTSMODEL_H
