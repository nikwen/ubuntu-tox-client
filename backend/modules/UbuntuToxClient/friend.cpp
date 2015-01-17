#include "friend.h"

Friend::Friend() :
    name(""),
    statusMessage("")
{
}

Friend::Friend(const Friend& f)
{
    this->name = f.name;
    this->statusMessage = f.statusMessage;
}
