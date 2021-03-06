/*
 * ubuntu-tox-client
 *
 * Copyright (c) 2015 Niklas Wenzel <nikwen.developer@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>

    This file is part of Tox Qt GUI.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the COPYING file for more details.
*/

#ifndef CDATA_H
#define CDATA_H

#include <cstdint>

class QString;
class CData
{
public:
    uint8_t* data();
    uint16_t size();

protected:
    explicit CData(const QString& data, uint16_t byteSize);
    virtual ~CData();

    static QString toString(const uint8_t* cData, const uint16_t cDataSize);

private:
    uint8_t* cData;
    uint16_t cDataSize;

    static uint16_t fromString(const QString& userId, uint8_t* cData);
};

class CUserId : public CData
{
public:
    explicit CUserId(const QString& userId);

    static QString toString(const uint8_t *cUserId);

private:
    static const uint16_t SIZE;

};

class CFriendAddress : public CData
{
public:
    explicit CFriendAddress(const QString& friendAddress);

    static QString toString(const uint8_t* cFriendAddress);

private:
    static const uint16_t SIZE;

};

#endif // CDATA_H
