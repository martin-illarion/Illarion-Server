//  illarionserver - server for the game Illarion
//  Copyright 2011 Illarion e.V.
//
//  This file is part of illarionserver.
//
//  illarionserver is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  illarionserver is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with illarionserver.  If not, see <http://www.gnu.org/licenses/>.


#ifndef _CLUALEARNSCRIPT_HPP
#define _CLUALEARNSCRIPT_HPP

#include "LuaScript.hpp"

class World;
class Character;

class LuaLearnScript : public LuaScript {
public:
    LuaLearnScript(std::string filename) throw(ScriptException);
    virtual ~LuaLearnScript() throw();

    void learn(Character *cc, std::string skill, uint8_t skillGroup, uint32_t actionPoints, uint8_t opponent, uint8_t leadAttrib);
    void reduceMC(Character *cc);

private:

    LuaLearnScript(const LuaLearnScript &);
    LuaLearnScript &operator=(const LuaLearnScript &);
};
#endif
