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


#ifndef CNPC_HPP
#define CNPC_HPP

#include "CCharacter.hpp"
#include "script/CLuaNPCScript.hpp"
#include <boost/shared_ptr.hpp>

/**
* struct to take a text
*/
struct NPCTalk 
{
    /**
    * german version of the spoken text
    */
	std::string SpeechText_ger;
    
    /**
    * englisch version of the spoken text
    */
	std::string SpeechText_eng;
};

class CWorld;

/**
* defines a npc
*/
class CNPC : public CCharacter 
{

	public:

		/**
        * constructor which creates the npc
        * @param id of the npc
        * @param name of the npc
        * @param type race of the npc
        * @param pos position on the map of the npc
        * @param dir in which direction the npc is watching
        * @param ishealer if true the npc ressurects death players
        * @param sex the sex of the npc
        */
		CNPC(TYPE_OF_CHARACTER_ID id, std::string name, CCharacter::race_type type, position pos, CCharacter::face_to dir, bool ishealer, CCharacter::sex_type sex,
             uint8_t hair, uint8_t beard, uint8_t hairred, uint8_t hairgreen, uint8_t hairblue, uint8_t skinred, uint8_t skingreen, uint8_t skinblue);

		/**
        * the destructor 
        */
		virtual ~CNPC();

		/**
        * gets the healer state of this npc
        * @return if the npc is a healer or not
        */
		bool getHealer() {
			return _ishealer;
		}
        
        /**
        * gets the initial position of the npc
        * @return the starting position of this npc
        */
		position getStartPos() {
			return _startpos;
		}
        
        /**
        * gets a pointer to the script for this npc
        * @return the pointer for the script
        */
		boost::shared_ptr<CLuaNPCScript> getScript() {
			return _script;
		}
        
        /**
        * adds a script to this npac
        * @param script a pointer to a lua script for this npc
        */
		void setScript(boost::shared_ptr<CLuaNPCScript> script) {
			_script = script;
		}

		/**
        * npc receives a spoken text from a character
        * triggeres a script entry
        * @param tt how loudly the text is spoken
        * @param message the text which is spoken
        * @param cc the character who has spokenthe text
        */
		virtual void receiveText(talk_type tt, std::string message, CCharacter* cc);

	protected:

		/**
        * stores if the character is a healer or not
        */
		bool _ishealer;
        
        //CCharacter::sex_type _sex;
        
        /**
        * stores the starting position
        */
		position _startpos;
        
        /**
        * stores the pointer to the script for this npc
        */
		boost::shared_ptr<CLuaNPCScript> _script;
};

#endif // CNPC_HPP
