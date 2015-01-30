#include "message.h"

Message::Message(QString messageText, Friend *parentFriend)
{
    this->messageText = messageText;
    this->parentFriend = parentFriend;
}

Message::Message(const Message &m)
{
    this->messageText = m.getMessageText();
    this->parentFriend = m.getParentFriend();
}
