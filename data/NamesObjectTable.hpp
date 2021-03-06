/*
 * Illarionserver - server for the game Illarion
 * Copyright 2011 Illarion e.V.
 *
 * This file is part of Illarionserver.
 *
 * Illarionserver  is  free  software:  you can redistribute it and/or modify it
 * under the terms of the  GNU  General  Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * Illarionserver is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY;  without  even  the  implied  warranty  of  MERCHANTABILITY  or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU  General Public License along with
 * Illarionserver. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NAMESOBJECTTABLE_H
#define NAMESOBJECTTABLE_H

#include <string>

#if __GNUC__ < 3
#include <hash_map>
#else
#include <ext/hash_map>

#if (__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
using __gnu_cxx::hash_map;
#endif

#if (__GNUC__ == 3 && __GNUC_MINOR__ < 1)
using std::hash_map;
#endif

#endif

#include "TableStructs.hpp"

//! eine Tabelle f�r allgemeine Item-Eigenschaften
class NamesObjectTable {

public:

    bool find(TYPE_OF_ITEM_ID Id, NamesStruct &ret);

    NamesObjectTable();

    ~NamesObjectTable();

    void reload();

    inline bool dataOK() {
        return m_dataOK;
    }

protected:

    //! der Datentyp der die Tabelle aufnimmt
    typedef hash_map < TYPE_OF_ITEM_ID, NamesStruct > TABLE;

    //! die Tabelle mit den eingelesenen Werten
    TABLE m_table;

    void clearOldTable();

    bool m_dataOK;

};

#endif
