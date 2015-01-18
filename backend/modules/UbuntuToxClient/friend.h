#ifndef FRIEND_H
#define FRIEND_H

#include <QObject>

class Friend
{

public:
    explicit Friend();
    Friend(const Friend&);

    void setFriendId(int friendId) { this->friendId = friendId; }
    void setName(QString name) { this->name = name; }
    void setStatusMessage(QString statusMessage) { this->statusMessage = statusMessage; }

    int getFriendId() { return friendId; }
    QString getName() { return name; } //TODO: Default name (tox id?) if name is empty
    QString getStatusMessage() { return statusMessage; }

private:
    int friendId;
    QString name;
    QString statusMessage;

};

#endif // FRIEND_H
