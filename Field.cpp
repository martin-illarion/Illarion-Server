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


#include "Field.hpp"

#include "data/TilesModificatorTable.hpp"
#include "data/CommonObjectTable.hpp"
#include "data/ContainerObjectTable.hpp"
#include "data/TilesTable.hpp"
#include "globals.hpp"

//#define Field_DEBUG

Field::Field() {

    tile = 0;
    music = 0;
    clientflags = 0;
    extraflags = 0;
    warptarget = new position(0,0,0);
}

void Field::setTileId(unsigned short int id) {
    tile = id;
}

unsigned short int Field::getTileCode() {
    return tile;
}

unsigned short int Field::getTileId() {
    if (((tile & 0xFC00) >> 10) > 0) { // shape exists
        return tile & 0x001F;
    } else {
        return tile;
    }
}

void Field::setMusicId(unsigned short int id) {
    music = id;
}

unsigned short int Field::getMusicId() {
    return music;
}

ScriptItem Field::getStackItem(uint8_t spos) {
    ScriptItem retItem;

    if (items.empty()) {
        return retItem;
    } else {
        ITEMVECTOR::iterator theIterator;
        uint8_t counter = 0;

        for (theIterator = items.begin(); theIterator < items.end(); ++theIterator) {
            if (counter >= spos) {
                retItem = (*theIterator);
                retItem.type = ScriptItem::it_field;
                retItem.itempos = counter;
                return retItem;
            }

            ++counter;
        }

        return retItem;
    }
}

Field::Field(const Field &source) {

    tile = source.tile;
    clientflags = source.clientflags;
    extraflags = source.extraflags;
    items = source.items;
    warptarget = new position(*(source.warptarget));
}


Field &Field:: operator =(const Field &source) {

    if (this != &source) {
        tile = source.tile;
        clientflags = source.clientflags;
        extraflags = source.extraflags;
        items = source.items;
        warptarget->x = source.warptarget->x;
        warptarget->y = source.warptarget->y;
        warptarget->z = source.warptarget->z;
    }

    return *this;

}


Field::~Field() {
    delete warptarget;
}


bool Field::addTopItem(const Item it) {

    if (IsPassable()) {
        if (items.size() < MAXITEMS) {
            items.push_back(it);
            TilesModificatorStruct temp;

            if (TilesModItems->find(it.id, temp)) {
                clientflags = clientflags | (temp.Modificator & (FLAG_GROUNDLEVEL));
                extraflags = extraflags | (temp.Modificator & (FLAG_SPECIALITEM + FLAG_PENETRATEABLE + FLAG_TRANSPARENT + FLAG_PASSABLE + FLAG_MAKEPASSABLE));
            }

            return true;
        }
    }

    return false;

}


bool Field::PutGroundItem(const Item it) {

    if (items.size() < MAXITEMS) {
        if (items.empty()) {
            items.push_back(it);
        } else {
            ITEMVECTOR::iterator iterat = items.begin();
            items.insert(iterat, it);
        }

        TilesModificatorStruct temp;

        if (TilesModItems->find(it.id, temp)) {
            clientflags   = clientflags | (temp.Modificator & (FLAG_GROUNDLEVEL));
            extraflags   = extraflags | (temp.Modificator & (FLAG_SPECIALITEM + FLAG_PENETRATEABLE + FLAG_TRANSPARENT + FLAG_PASSABLE + FLAG_MAKEPASSABLE));
        }

        return true;
    }

    return false;

}


bool Field::PutTopItem(const Item it) {

    if (items.size() < MAXITEMS) {
        items.push_back(it);
        TilesModificatorStruct temp;

        if (TilesModItems->find(it.id, temp)) {
            clientflags   = clientflags | (temp.Modificator & (FLAG_GROUNDLEVEL));
            extraflags   = extraflags | (temp.Modificator & (FLAG_SPECIALITEM + FLAG_PENETRATEABLE + FLAG_TRANSPARENT + FLAG_PASSABLE + FLAG_MAKEPASSABLE));
        }

        return true;
    }

    return false;

}


bool Field::TakeTopItem(Item &it) {

    if (items.empty()) {
        return false;
    }

    it = items.back();
    items.pop_back();
    updateFlags();

    return true;

}

bool Field::changeQualityOfTopItem(short int amount) {
    Item it;
    short int tmpQuality;

    if (TakeTopItem(it)) {
        // don't increase the class of the item, but allow decrease of the item class
        tmpQuality = ((amount+it.quality%100)<100) ? (amount + it.quality) : (it.quality-it.quality%100 + 99);

        if (tmpQuality%100 > 1) {
            it.quality = tmpQuality;
            PutTopItem(it);
            return false;
        } else {
            return true;
        }
    }

    return false;
}


int Field::increaseTopItem(int count, bool &erased) {

    Item it;
    int temp = count;
#ifdef Field_DEBUG
    std::cout << "increaseTopItem " << count << "\n";
#endif

    if (TakeTopItem(it)) {
        temp = count + it.number;
#ifdef Field_DEBUG
        std::cout << "temp " << temp << "\n";
#endif
#ifdef Field_DEBUG
        std::cout << "it.number:"<<it.number<<std::endl;
#endif

        if (temp > MAXITEMS) {
            it.number = MAXITEMS;
            temp = temp - MAXITEMS;
            PutTopItem(it);
            erased = false;
        } else if (temp <= 0) {
            temp = count + it.number;
            erased = true;
        } else {
            it.number = temp;
            temp = 0;
            PutTopItem(it);
            erased = false;
        }
    }

    return temp;

}


bool Field::swapTopItem(TYPE_OF_ITEM_ID newid, uint16_t newQuality) {

    std::cout << "swapTopItem " << std::endl;
    Item temp;

    if (TakeTopItem(temp)) {
        std::cout << "old ID " << (int)temp.id << " old wear " << (int)temp.wear << std::endl;
        temp.id = newid;

        if (newQuality > 0) {
            temp.quality = newQuality;
        }

        if (CommonItems->find(newid, tempCommon)) {
            temp.wear=tempCommon.AgeingSpeed;
            std::cout << "new ID " << (int)temp.id << " new wear " << (int)temp.wear << std::endl;
        } else {
            std::cout << "new ID " << (int)temp.id << " new wear not found" << std::endl;
        }

        PutTopItem(temp);
        return true;
    }

    std::cout << "no item found to swap" << std::endl;

    return false;

}


bool Field::ViewTopItem(Item &it) {

    if (items.empty()) {
        return false;
    }

    it = items.back();

    return true;

}


MAXCOUNTTYPE Field::NumberOfItems() {

    return items.size();

}


void Field::Save(std::ostream *mapt, std::ostream *obj, std::ostream *warp) {

    mapt->write((char *) & tile, sizeof(tile));
    mapt->write((char *) & music, sizeof(music));
    mapt->write((char *) & clientflags, sizeof(clientflags));
    mapt->write((char *) & extraflags, sizeof(extraflags));

    ITEMVECTOR::iterator theIterator;

    unsigned char size = items.size();
    obj->write((char *) & size, sizeof(size));

    for (theIterator = items.begin(); theIterator < items.end(); ++theIterator) {
        theIterator->save(obj);
    }

    if (IsWarpField()) {
        char b = 1;
        warp->write((char *) & b, sizeof(b));
        warp->write((char *) & warptarget->x, sizeof(warptarget->x));
        warp->write((char *) & warptarget->y, sizeof(warptarget->x));
        warp->write((char *) & warptarget->z, sizeof(warptarget->x));
    } else {
        char b = 0;
        warp->write((char *) & b, sizeof(b));
    }
}


void Field::giveNonPassableItems(ITEMVECTOR &nonpassitems) {

    ITEMVECTOR::iterator theIterator;

    for (theIterator = items.begin(); theIterator < items.end(); ++theIterator) {
        if (TilesModItems->nonPassable(theIterator->id)) {
            nonpassitems.push_back(*theIterator);
        }
    }

}


void Field::giveExportItems(ITEMVECTOR &nonmoveitems) {

    ITEMVECTOR::iterator theIterator;

    for (theIterator = items.begin(); theIterator < items.end(); ++theIterator) {
        if (theIterator->wear == 255) {
            nonmoveitems.push_back(*theIterator);
        } else {
            if (CommonItems->find(theIterator->id, tempCommon) && tempCommon.AfterInfiniteRot > 0) {
                Item rottedItem = *theIterator;
                rottedItem.id = tempCommon.AfterInfiniteRot;
                rottedItem.wear = 255;
                nonmoveitems.push_back(rottedItem);
            }
        }
    }
}


void Field::Load(std::istream *mapt, std::istream *obj, std::istream *warp) {

    mapt->read((char *) & tile, sizeof(tile));
    mapt->read((char *) & music, sizeof(music));
    mapt->read((char *) & clientflags, sizeof(clientflags));
    mapt->read((char *) & extraflags, sizeof(extraflags));

    unsigned char ftemp = 255 - FLAG_NPCONFIELD - FLAG_MONSTERONFIELD - FLAG_PLAYERONFIELD;

    clientflags = clientflags & ftemp;

    MAXCOUNTTYPE size;
    obj->read((char *) & size, sizeof(size));

    items.clear();
    Item temp;

    for (int i = 0; i < size; ++i) {
        temp.load(obj);
        items.push_back(temp);
    }

    char iswarp = 0;
    warp->read((char *) & iswarp, sizeof(iswarp));

    if (iswarp == 1) {
        short int x, y, z;
        warp->read((char *) & x, sizeof(warptarget->x));
        warp->read((char *) & y, sizeof(warptarget->y));
        warp->read((char *) & z, sizeof(warptarget->z));
        SetWarpField(position(x, y, z));
    }

}

int8_t Field::DoAgeItems(ITEM_FUNCT funct) {

    int8_t ret = 0;

    if (! items.empty()) {
        ITEMVECTOR::iterator theIterator = items.begin();

        while (theIterator < items.end()) {
            if (! funct(&(*theIterator))) {
                if (!CommonItems->find(theIterator->id, tempCommon)) {
                    tempCommon.ObjectAfterRot = theIterator->id;
                }

                if (theIterator->id != tempCommon.ObjectAfterRot) {
#ifdef Field_DEBUG
                    std::cout << "FIELD:Ein Item wird umgewandelt von: " << theIterator->id << "  nach: " << tempCommon.ObjectAfterRot << "!\n";
#endif

                    //only set ret to 1 if it wasn't -1 because -1 has the highest priority (forces update of the field and rots container)
                    if (theIterator->id != tempCommon.ObjectAfterRot && ret != -1) {
                        ret = 1;
                    }

                    theIterator->id = tempCommon.ObjectAfterRot;

                    if (CommonItems->find(tempCommon.ObjectAfterRot, tempCommon)) {
                        theIterator->wear = tempCommon.AgeingSpeed;
                    }

                    theIterator++;
                } else {
#ifdef Field_DEBUG
                    std::cout << "FIELD:Ein Item wird gel�cht,ID:" << theIterator->id << "!\n";
#endif

                    if (ContainerItems->find(theIterator->id)) {
                        std::cout << "Ein Container auf einem Feld soll gel�cht werden!" << std::endl;
                        erasedcontainers->push_back(theIterator->number);
                        ret = -1;
                    }

                    theIterator = items.erase(theIterator);

                    if (ret != -1) {
                        ret = 1;
                    }

                }
            } else {
                ++theIterator;
            }
        }
    }

    updateFlags();

    return ret;

}

void Field::updateFlags() {

    // alle durch Items und Tiles modifizierte Flags l�chen
    clientflags = clientflags & (255 - (FLAG_GROUNDLEVEL));
    extraflags = extraflags & (255 - (FLAG_SPECIALITEM + FLAG_PENETRATEABLE + FLAG_TRANSPARENT + FLAG_PASSABLE + FLAG_MAKEPASSABLE));

    TilesStruct tt;

    if (Tiles->find(tile, tt)) {
        clientflags = clientflags | (tt.flags & (FLAG_GROUNDLEVEL));
        extraflags = extraflags | (tt.flags & (FLAG_PENETRATEABLE + FLAG_TRANSPARENT + FLAG_PASSABLE + FLAG_SPECIALTILE + FLAG_MAKEPASSABLE));
    }

    TilesModificatorStruct tmod;
    ITEMVECTOR::iterator theIterator;

    for (theIterator = items.begin(); theIterator < items.end(); ++theIterator) {
        if (TilesModItems->find(theIterator->id, tmod)) {
            clientflags   = clientflags | (tmod.Modificator & (FLAG_GROUNDLEVEL));
            extraflags   = extraflags | (tmod.Modificator & (FLAG_SPECIALITEM + FLAG_PENETRATEABLE + FLAG_TRANSPARENT + FLAG_PASSABLE + FLAG_MAKEPASSABLE));
        }
    }

}


void Field::DeleteAllItems() {

    items.clear();
    updateFlags();

}


unsigned char Field::GroundLevel() {

    return (clientflags & FLAG_GROUNDLEVEL);

}


bool Field::IsMonsterOnField() {

    return ((clientflags & FLAG_MONSTERONFIELD) != 0);

}


void Field::SetMonsterOnField(bool t) {

    if (t) {
        clientflags = clientflags | FLAG_MONSTERONFIELD;
    } else {
        clientflags = clientflags & (255 - FLAG_MONSTERONFIELD);
    }

}


bool Field::IsNPCOnField() {

    return ((clientflags & FLAG_NPCONFIELD) != 0);

}


void Field::SetNPCOnField(bool t) {

    if (t) {
        clientflags = clientflags | FLAG_NPCONFIELD;
    } else {
        clientflags = clientflags & (255 - FLAG_NPCONFIELD);
    }

}


bool Field::IsPlayerOnField() {

    return ((clientflags & FLAG_PLAYERONFIELD) != 0);

}


void Field::SetPlayerOnField(bool t) {

    if (t) {
        clientflags = clientflags | FLAG_PLAYERONFIELD;
    } else {
        clientflags = clientflags & (255 - FLAG_PLAYERONFIELD);
    }

}



void Field::SetLevel(unsigned char z) {

    z <<= 4;     // bits an die richtige Position bringen
    z &= FLAG_MAPLEVEL;     // andere bits l�chen
    clientflags &= (255 - FLAG_MAPLEVEL);       // maplevel - bits l�chen
    clientflags |= z;     // maplevel - bits setzen

}


bool Field::IsWarpField() {

    return ((extraflags & FLAG_WARPFIELD) != 0);

}



void Field::SetWarpField(const position &pos) {
    warptarget->x = pos.x;
    warptarget->y = pos.y;
    warptarget->z = pos.z;
    extraflags = extraflags | FLAG_WARPFIELD;
}


void Field::UnsetWarpField() {
    extraflags = extraflags & (255 - FLAG_WARPFIELD);
}


void Field::GetWarpField(position &pos) {
    pos.x = warptarget->x;
    pos.y = warptarget->y;
    pos.z = warptarget->z;
}


bool Field::HasSpecialItem() {

    return ((extraflags & FLAG_SPECIALITEM) != 0);

}


void Field::SetSpecialItem(bool t) {

    if (t) {
        extraflags = extraflags | FLAG_SPECIALITEM;
    } else {
        extraflags = extraflags & (255 - FLAG_SPECIALITEM);
    }

}


bool Field::IsSpecialField() {

    return ((extraflags & FLAG_SPECIALTILE) != 0);

}


void Field::SetSpecialField(bool t) {

    if (t) {
        extraflags = extraflags | FLAG_SPECIALTILE;
    } else {
        extraflags = extraflags & (255 - FLAG_SPECIALTILE);
    }

}


bool Field::IsTransparent() {

    return ((extraflags & FLAG_TRANSPARENT) == 0);

}


bool Field::IsPassable() const {

    return (((extraflags & FLAG_PASSABLE) == 0) || ((extraflags & FLAG_MAKEPASSABLE) != 0));

}


bool Field::IsPenetrateable() {

    return ((extraflags & FLAG_PENETRATEABLE) == 0);

}


bool Field::moveToPossible() const {
    return (
               (IsPassable()  &&
                ((clientflags & (FLAG_MONSTERONFIELD | FLAG_NPCONFIELD | FLAG_PLAYERONFIELD)) == 0))
           );
}

void Field::setChar() {
    clientflags |= FLAG_PLAYERONFIELD;
}

void Field::removeChar() {
    clientflags &= ~FLAG_PLAYERONFIELD;
}
