#include "friend.h"

Friend::Friend() :
    name(""),
    statusMessage("")
{
}

Friend::Friend(const Friend& f)
{
    setFriendId(f.friendId);
    setClientId(f.clientId);
    setName(f.name);
    setStatusMessage(f.statusMessage);
}
