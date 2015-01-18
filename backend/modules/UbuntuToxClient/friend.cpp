#include "friend.h"

Friend::Friend() :
    name(""),
    statusMessage("")
{
}

Friend::Friend(const Friend& f)
{
    setFriendId(f.friendId);
    setName(f.name);
    setStatusMessage(f.statusMessage);
}
