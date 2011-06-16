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


#ifndef CNPCTABLE_H
#define CNPCTABLE_H

#include <string>
#include <list>

#include "globals.h"
#include "CWorld.hpp"
#include "CNPC.hpp"
#include "CField.hpp"

class CWorld;

struct NPCStruct {

	position NPCPos; //Pos des NPC
	std::string Name; //Name des NPC
	CCharacter::race_type type; //type des NPC's
	CCharacter::face_to faceto; //richtung des Chars 0 Nord, 2 Ost, 4 S�d, 6 West
	std::vector<struct NPCTalk> speechTexts; //Texte welche der NPC sagen kann;
	unsigned short int walk_range; //Bereich in dem sich der NPC bewegen darf. 0 f�r keine Bewegung.
	bool ishealer;
};


class CNPCTable {
	public:

		CNPCTable(); //Constructor
		~CNPCTable(); //Destructor

		bool LoadData(); //Function welche die Daten aus der Datenbank l�dt liefert false wenn das laden nicht klappt

	private:
        CWorld * _world;
		std::list<struct NPCStruct> NPCList; //List zum aufnehmen aller Werte aus der DB
};

#endif
