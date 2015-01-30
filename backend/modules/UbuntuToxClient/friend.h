#ifndef FRIEND_H
#define FRIEND_H

#include <QObject>
#include <QStringList>

#include "message.h"
#include "typeidbaseclass.h"

class Friend : public TypeIdBaseClass
{

public:
    explicit Friend();
    Friend(const Friend&);

    bool operator==(const Friend &other) const;

    void setFriendId(int friendId) { this->friendId = friendId; }
    void setClientId(QString clientId) { this->clientId = clientId; }
    void setName(QString name) { this->name = name; }
    void setStatusMessage(QString statusMessage) { this->statusMessage = statusMessage; }

    int getFriendId() const { return friendId; }
    const QString& getClientId() const { return clientId; }
    const QString& getName() const { return name; }
    const QString& getStatusMessage() const { return statusMessage; }

    int getMessageCount() const { return messageList.length(); }
    const Message& getMessage(int pos) const { return messageList.at(pos); }

private:
    //NOTE: When adding private attributes, make sure to adjust the copy constructor!
    int friendId;
    QString clientId;
    QString name;
    QString statusMessage;

    QList<Message> messageList;

};

#endif // FRIEND_H
