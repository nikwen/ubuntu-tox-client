#ifndef FRIEND_H
#define FRIEND_H

#include <QObject>

class Friend
{

public:
    explicit Friend();
    Friend(const Friend&);

    void setFriendId(int friendId) { this->friendId = friendId; }
    void setClientId(QString clientId) { this->clientId = clientId; }
    void setName(QString name) { this->name = name; }
    void setStatusMessage(QString statusMessage) { this->statusMessage = statusMessage; }

    int getFriendId() { return friendId; }
    QString getClientId() { return clientId; }
    QString getName() { return name; }
    QString getStatusMessage() { return statusMessage; }

private:
    //NOTE: When adding private attributes, make sure to adjust the copy constructor!
    int friendId;
    QString clientId;
    QString name;
    QString statusMessage;

};

#endif // FRIEND_H
