#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

#include "typeidbaseclass.h"

class Friend;

class Message : public TypeIdBaseClass
{

public:
    explicit Message(QString messageText, Friend *parentFriend);
    Message(const Message&);

    const QString& getMessageText() const { return messageText; }
    Friend* getParentFriend() const { return parentFriend; }

private:
    QString messageText;
    Friend *parentFriend = nullptr;

};

#endif // MESSAGE_H
