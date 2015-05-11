/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  zoltanp <email>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef DCOP_STUB_H
#define DCOP_STUB_H

class QString;

/*
 * Header to make DCOP specific code compile
 */

class DCOPRef
{
};

class DCOPObject {
public:
    void setObjId(const QString &);
};

#define K_DCOP

#define k_dcop public

#endif // DCOP_STUB_H
