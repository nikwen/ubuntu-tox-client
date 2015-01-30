#include "friend.h"

Friend::Friend() :
    name(""),
    statusMessage(""),
    clientId(""),
    friendId(-1)
{
    Message m("Test", this);
    messageList << m;
}

Friend::Friend(const Friend& f)
{
    setFriendId(f.friendId);
    setClientId(f.clientId);
    setName(f.name);
    setStatusMessage(f.statusMessage);
    messageList << f.messageList;
}

bool Friend::operator ==(const Friend &other) const {
    return (this->getClientId() == other.getClientId()) && (this->getFriendId() == other.getFriendId());
}
