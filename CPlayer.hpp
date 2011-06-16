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


#ifndef CPLAYER_HPP
#define CPLAYER_HPP

//falls nicht auskommentiert, werden mehr Bildschirmausgaben gemacht:
//#define CPlayer_DEBUG

#include <string>
#include <sstream>
#include <set>

#include "CTimer.h"
#include "CCharacter.hpp"
#include "CContainerStack.h"
#include "constants.h"
#include "CLongTimeAction.hpp"
#include <boost/shared_ptr.hpp>
#include "CLanguage.hpp"
#include "netinterface/protocol/ServerCommands.hpp"


class CWorld;
class CLuaScript;
class CNetInterface;
struct WeatherStruct;

enum gm_rights
{
    gmr_allowlogin = 1, //GM is allowed to login if nologin is true
    gmr_basiccommands = 2, //Basic Commands like !who !what !? !fi and !inform
    gmr_warp = 4, //GM is allowed to Warp (includes #jump_to)
    gmr_summon = 8, // GM is allowed to Summon
    gmr_prison = 16, //GM is allowed to put people in prisons
    gmr_settiles = 32, //GM is allowed to change tiles (includes ton and toff command)
    gmr_clipping = 64, //GM is allowed to change his clipping state (Walk trough walls)
    gmr_warpfields = 128, //GM is allowed to manipulate Warpfields
    gmr_import = 256, //GM is allowed to import maps and Warpfields. includes also createArea
    gmr_visible = 512, //GM is allowed to change his visiblity state
    gmr_reload = 1024, //GM is allowed to reload the tabels (With #r or !rd) includes the right to set the spawnstate !setspawn
    gmr_ban = 2048, //GM is allowed to ban players.
    gmr_loginstate = 4096, //GM is allowed to change loginstate
    gmr_save = 8192, //GM is allowed to save players and maps (!playersave and #mapsave)
    gmr_broadcast = 16384, //GM is allowed to broadcast messages
    gmr_forcelogout = 32768, //GM is allowed to force a logout of players
    gmr_getgmcalls = 65536, //Char gets GM messages
    gmr_isnotshownasgm = 131072 //Char is not shown at gm in the list/ has no officially gm state.
};

//! ein Spieler in newmud
class CPlayer : public CCharacter {

    public:

        ////////////////////////////////////////
        // possible Exceptions thrown by CPlayer
        ////////////////////////////////////////
        // exception is thrown if logout of player is only possible result
        class LogoutException {
            public:
                LogoutException(const char& reason) : m_reason(reason) {}
                inline const char& getReason() { return m_reason; }
            private:
                char m_reason;
        };

        enum viewdir 
        {
            upper = 0,
            right = 1,
            lower = 2,
            left = 3
        };

        typedef enum
        {
            upperleft = 0,
            upperright = 1,
            lowerleft = 2,
            lowerright = 3
        } mapreferences;
        
        
        void workoutCommands();
        
        //! definiert eine "Zweiseitige Warteschlange" vom Typ unsigned char
        //typedef deque<unsigned char> BYTEDEQUE;

        //! die letzte IP des Spielers als std::string
        std::string last_ip;

        //! gibt an, ob der Spieler sich gerade den Inhalt eines Containers der auf der Karte liegt anzeigen l�t
        bool mapshowcaseopen;

        //BYTEDEQUE//
        //! alle Langzeiteffekte die auf den Spieler einwirken
        int longTimeEffects;

        //! Passwort
        std::string pw;

        //! E-Mail
        std::string email;

        //! Zeit (in Sekunden) die der Spieler insgesamt online war
        // In den Speicherfunktionen wird die Differenz zwischen aktueller Systemzeit
        // und lastsavetime zu onlinetime addiert.
        // Der Konstruktor, die Speicherfunktionen, die Ladefunktionen und setPlayerData setzen lastsavetime immer
        // auf die jeweils aktuelle Systemzeit.
        unsigned long int onlinetime;

        //! Zeitpunkt zu dem die Klasseninstanz erstellt bzw. das letzte mal gespeichert wurde
        time_t lastsavetime;

        //! Zeitpunkt der letzten Lebensmeldung vom Client
        time_t lastkeepalive;

        //! Zeitpunkt der letzten Aktion des Spielers
        time_t lastaction;

        //! Location
        std::string location;

        //! Realer Name
        std::string realname;
        	
        //! Screen resolution;
        uint8_t screenwidth;
        uint8_t screenheight;

        //! die Verbindung zum Spieler, -- Achtung ! Die Verbindung wird NICHT im Destruktor gel�cht
        // , da sie auch extern erstellt wird und durch das Einfgen in diverse
        // Vektoren oft Destruktoren fr tempor�e CPlayer aufgerufen werden, die noch
        // ben�igte Verbindungen l�chen wrden!
        boost::shared_ptr<CNetInterface> Connection;

        //! alle "Sichtfenster" fr Containerinhalte die der Spieler �fnen kann
        CContainerStack showcases[ MAXSHOWCASES ];

        private:
        // all visible chars
        std::set<uint32_t> visibleChars;

        public:
                // send a char appearance; always or only if char not yet visible
        void sendCharAppearance( TYPE_OF_CHARACTER_ID id, boost::shared_ptr<CBasicServerCommand> appearance, bool always ); 

            // removes a Char from sight
        void sendCharRemove( TYPE_OF_CHARACTER_ID id, boost::shared_ptr<CBasicServerCommand> removechar );


        /**
        *a long time needed action for the player
        */
        CLongTimeAction * ltAction;
        
        virtual void startAction(unsigned short int wait, unsigned short int ani=0, unsigned short int redoani=0, unsigned short int sound=0, unsigned short int redosound=0);
        virtual void abortAction();
        virtual void successAction();
        virtual void actionDisturbed(CCharacter * disturber);

        /**
        *changes the source of the last action of this Player
        *<b>Lua: [:changeSource]</b>
        *@param cc source is a character the pointer to this character
        */
        virtual void changeSource( CCharacter * cc );
        
        /**
        *changes the source of the last action for this player
        *<b>Lua: [:changeSource]</b>
        *@param sI source is a item the new item
        */
        inline virtual void changeSource( ScriptItem sI );
        
        /**
        *changes the Source of the last action for this player.
        *<b>Lua: [:changeSource]</b>
        *@param pos source is a position the new position
        */
        inline virtual void changeSource( position pos ); 
        
        /**
        *changes the Source of the last action to nothing for this player
        *<b>Lua: [:changeSource]</b>
        */
        inline virtual void changeSource();
        
        /**
        *changes the Target of the last action for this player.
        *<b>Lua: [:changeTarget]</b>
        *@param cc target is a character the pointer to this character
        */
        inline virtual void changeTarget( CCharacter * cc );
        
        /**
        *changes the Target of the last action for this player.
        *<b>Lua: [:changeTarget]</b>
        *@param sI target is a item the new item
        */
        inline virtual void changeTarget( ScriptItem sI );
        
        /**
        *changes the target of the last action for this player.
        *<b>Lua: [:changeTarget]</b>
        *@param pos Target is a position the new position
        */
        inline virtual void changeTarget( position pos );
        
        /**
        *changes the Target of the last action to nothing for this player
        *<b>Lua: [:changeTarget]</b>
        */
        inline virtual void changeTarget();
        
        /**
        * returns the number of seconds the player has been idle, not actively issuing commands
        * <b>Lua: [:idleTime]</b>
        * @return number of seconds the player has been idle
        */
        virtual uint32_t idleTime();

        /**
        * send a book ID to the client
        * <b>Lua: [:sendBook]</b>
        * @param bookID id of the book
        */
        virtual void sendBook( uint16_t bookID );

        /**
        * send a character description to the player if the  char is a player
        * <b>Lua: [:sendCharDescription]</b>        
        * @param id of the character from which the description is sended
        * @param desc the current descpription
        */          
        virtual void sendCharDescription( TYPE_OF_CHARACTER_ID id,const std::string& desc);
        
        //! normal constructor
        CPlayer(boost::shared_ptr<CNetInterface> newConnection) throw(LogoutException);
        
        //! check if username/password is ok
        void check_logindata() throw (LogoutException);

        //Checks if a Player has a special GM right
        bool hasGMRight(gm_rights right);

        //! save char to db
        bool save() throw();

        //! load data from db
        // \param no_attributes don't load contents of table "player"
        bool load() throw ();
        
        void login() throw (LogoutException);

        //Loads the GM Flag of the character
        bool loadGMFlags() throw ();

        //! Destruktor
        ~CPlayer();


        /**
        * sends one area relative to the current z coordinate to the player
        * @param zoffs the offset of the z param of the area which should be sended
        */
        void sendRelativeArea(int8_t zoffs);
       
        /**
     * sends all areas
     */
        void sendFullMap();

        /**
        * sends one complete mapstripe ( z-2, z-1, z, z+1, z+2) to the client
        * @param direction the direction from which the whole stripe has to be sent
        * @param extraStripeForDiagonalMove send an additional stripe for diagonal moves
        */
        void sendDirStripe( viewdir direction, bool extraStripeForDiagonalMove );
      
        void sendStepStripes( direction dir );

        /**
        * sends one single mapstripe to the client
        * @param direction of the single stripe 
        * @param zoffs the offset to the z direction which should be sended
        */
        void sendSingleStripe( viewdir direction, int8_t zoffs );
        
        inline bool actionRunning(){ return ltAction->actionRunning(); }

        //�erladene Funktion der Basisklasse ccharacter um den Spieler noch zus�zliche ausgaben seines Zustandes zu machen.
        void increasePoisonValue(short int value);

        //! schlie� alle "Sichtfenster" fr Containerinhalte des Spielers
        void closeAllShowcasesOfMapContainers();
        void closeAllShowcases();

        //�dert die Qualit� eines Items an einer bestimmten Stelle
        void changeQualityAt(unsigned char pos, short int amount);

        //�dert die Qualit� eines Items mit einer bestimmten ID
        void changeQualityItem(TYPE_OF_ITEM_ID id, short int amount);

        //! speichert die Daten des Character in eine Datei
        // \param pfn das Verzeichnis fr die Datendatei
        // \return true falls erfolgreich, false sonst

        //! berprft ein Pa�ort
        // \param chkpw das Pa�ort das berprft werden soll
        // \return true, falls chkpw mit dem Pa�ort des Spielers bereinstimmt
        bool VerifyPassword( std::string chkpw );
        
        inline bool isMonitoringClient(){ return monitoringClient; }

        //Dropt beim Sterben den alles aus Belt und Backpack
        void PlayerDeath();

        //! L�st den Player Magie lernen. Fr standard Charactere keine Funktion in CPlayer berladen
        //\param type Magierichtung die gelernt werden soll (0 Magier, 1 Priester, 2 Barde, 3 Druide)
        //\param flag Magieflags die gelernt werden sollen
        virtual void teachMagic(unsigned char type, unsigned char flag);

        //! Sets the active magic type of a player (druid/mage/...)
        //\param newMagType Magierichtung, die jetzt aktiv ist (0 Magier, 1 Priester, 2 Barde, 3 Druide)
        virtual void set_magic_type(magic_type newMagType) {
            magic.type=newMagType;
            sendMagicFlags(magic.type);
        }

        virtual void setInformChar(bool newInformChar) {
            informCharacter=newInformChar;
        }

        //! Returns the language which the player specified when creating the Character (german/english/french)
        virtual const unsigned short int getPlayerLanguage();

        virtual void setPlayerLanguage(CLanguage::LanguageType mother_tongue);

        //�erladene Funktion um beim Sterben Items zu droppen
        void SetAlive( bool t );

        //! schickt an den Spieler die Daten des Items an
        // einer Positon des K�pers
        // \param cpos die Position des Item am K�per
        void sendCharacterItemAtPos( unsigned char cpos );
        
        /**
        *Sends the weather to the client
        *@weather the weather struct of the weather which is sended
        *@interior ist the player inside outside?
        */
        void sendWeather( WeatherStruct weather );

        //! Fhrt die Alterung der Item im Inventory des Player durch
        // und schickt ggf. ein Update an den Spieler
        // \param funct
        // \see Item.hpp
        void AgeInventory( ITEM_FUNCT funct );

        //! erstellt count Item mit der ID itemid im Inventory des CPlayer
        // und schickt ein Update an den Spieler
        // \param itemid die Id der zu erstellenden Item
        // \param count die Anzahl der zu erstellenden Item
        // \return Anzahl der Item die nicht erstellt werden konnten
        //virtual int createItem( TYPE_OF_ITEM_ID itemid, int count );
        virtual int createItem( TYPE_OF_ITEM_ID itemid, uint8_t count, uint16_t quali, uint32_t data );

        /**
        * try to learn a skill 
        * <b>Lua: [:learn]</b>
        * @param skill name of the skill the user tries to learn
        * @param skillGroup group of the given skill 
        * @param actionPoints cost of learning
        * @param opponent gives information until which skill level learning may be successful
        * @param leadAttrib value of the lead attribute responsible for how successful learning the given skill is
        */
        virtual void learn( std::string skill, uint8_t skillGroup, uint32_t actionPoints, uint8_t opponent, uint8_t leadAttrib );

        //! verändert den Wert eines Skills bzw. legt einen neuen Skill an
        // \param typ der Typ des Skills
        // \param name der Name des Skills
        // \param amount �derung
        // \return der neue Skillwert fr name
        virtual unsigned short int increaseSkill( unsigned char typ, std::string name, short int amount );

        //! löscht alle Skills des CCharacter
        virtual void deleteAllSkills();

        //! löscht count Item mit der ID itemid aus dem Inventory des CPlayer
        // und schickt ein Update an den Spieler
        // \param itemid die Id der zu löschenden Item
        // \param count die Anzahl der zu löschenden Item
        // \return Anzahl der Item die nicht gelöcht werden konnten
        virtual int eraseItem( TYPE_OF_ITEM_ID itemid, int count );

        //! löscht count Items mit der ID itemid aus dem Inventory des CPlayer
        // und schickt ein Update an den Spieler
        // \param itemid die Id der zu löschenden Items
        // \param count die Anzahl der zu löschenden Items
        // \oaram data Datawert der zu löschenden Items
        // \return Anzahl der Items, die nicht gelöscht werden konnten
        virtual int eraseItem( TYPE_OF_ITEM_ID itemid, int count, uint32_t data );

        //! ver�dert die Anzahl des Item an der Position pos um count
        // und schickt ein Update an den Spieler
        // \param pos die Stelle im Inventory die ge�dert werden soll
        // \param count die �derungsanzahl
        // \return der Rest von count, der nicht ge�dert werden konnnte
        int increaseAtPos( unsigned char pos, int count );

        int createAtPos(unsigned char pos, TYPE_OF_ITEM_ID newid, int count);

        //! setzt die Id des Item an der Position pos auf newid
        // und schickt ein Update an den Spieler
        // \param pos die Stelle im Inventory die ge�dert werden soll
        // \param newid die neue Id des Item
        // \return true falls erfolgreich, false sonst
        bool swapAtPos( unsigned char pos, TYPE_OF_ITEM_ID newid, uint16_t newQuality = 0 );

        //! schickt ein Update der Ansicht des Rucksackinhalts an den Client
        void updateBackPackView();

        //! sendet alle Namen der Skills des CPlayer mit den entsprechenden Typen/Werten an den Client
        void sendAllSkills();

        //! sendet Magie-Flags an den Client
        // \param der Magietyp fr den die Flags an den Client gechickt werden sollen
        void sendMagicFlags(int type);

        //! sendet ein Skillupdate an den Client
        // \param name der Name des Skills
        // \param type der Typ des Skills
        // \param value der Wert des Skills
        void sendSkill( std::string name, unsigned char type, unsigned short int major, unsigned short int minor );
        
        unsigned short int setSkill( unsigned char typ, std::string sname, short int major, short int minor, uint16_t firsttry );

        //! sendet ein Attributupdate an den Client
        // \param name der Name des Attributs
        // \param value der Wert des Attributs
        void sendAttrib( std::string name, unsigned short int value );

        //! ver�dert einen Attributwert und schickt ein Update an den Spieler
        // \param name der Name des Attributs
        // \param amount die �derung
        // \return der neuen Wert des Attributs
        unsigned short int increaseAttrib( std::string name, short int amount );
        
        /**
        * changes one attrib temporary
        *@param name name of the attrib which should be changed
        *@param amount how much this attrib should be changed
        *@param time how much seconds the temporary changing should hold
        */
        void tempChangeAttrib( std::string name, short int amount, uint16_t time);
        
        /**
        *checks all attribs if the time base is smaller 0 and sets them back to the default value
        */
        void tempAttribCheck();


        void startMusic(short int which);
        void defaultMusic();

        bool sendTextInFile( std::string filename );

        // Setters and Getters //
        unsigned char GetStatus();
        void SetStatus( unsigned char thisStatus);

        // What time does the status get reset?
        time_t GetStatusTime();
        void SetStatusTime(time_t thisStatustime);

        // Who banned/jailed the player?
        std::string GetStatusGM();
        void SetStatusGM( TYPE_OF_CHARACTER_ID thisStatusGM );

        // Why where they banned/jailed?
        std::string GetStatusReason();
        void SetStatusReason( std::string thisStatusreason );

        // World Map Turtle Graphics
        void setTurtleActive( bool tturtleActive );
        bool getTurtleActive( );
        void setTurtleTile( unsigned char tturtletile );
        unsigned char getTurtleTile( );

        // Clipping on/off (default to on)
        void setClippingActive( bool tclippingActive );
        bool getClippingActive( );

        //Set for Admin state, uin32_t bit flag
        void setAdmin( uint32_t tAdmin );
        bool isAdmin();

        void setEncumberedSent( bool tEncumberedSent );
        bool wasEncumberedSent( );

        void setUnconsciousSent( bool tUnconsciousSent );
        bool wasUnconsciousSent( );

        // player gets informed about something
        virtual void inform(std::string text);

        // player heard something
        virtual void receiveText(talk_type tt, std::string message, CCharacter* cc);

        // receive an introduction
        virtual void introducePerson(CCharacter* cc);

        // Move the Player
        virtual bool move(direction dir, uint8_t mode);

        // Increase Movement Cost by encumburance
        // \return true if walking possible else false
        virtual bool encumberance(uint16_t &movementCost);

        virtual void startPlayerMenu(UserMenuStruct menu);

        virtual bool Warp(position newPos);

        virtual bool forceWarp(position newPos);

        virtual void LTIncreaseHP(unsigned short int value, unsigned short int count, unsigned short int time);

        virtual void LTIncreaseMana(unsigned short int value, unsigned short int count, unsigned short int time);

        virtual void openDepot(uint16_t depotid);

    virtual void setQuestProgress( uint16_t questid, uint32_t progress ) throw();

    virtual uint32_t getQuestProgress( uint16_t questid ) throw();

    virtual bool moveDepotContentFrom( uint32_t sourcecharid, uint32_t targetdepotid, uint32_t sourcedepotid ) throw();

    void sendMessage(std::string Message);

    #ifdef _PLAYER_AUTO_SAVE_
        void checkSave();
    #endif

    protected:

    // shared code for all eraseItem methods
    virtual int _eraseItem( TYPE_OF_ITEM_ID itemid, int count, uint32_t data, bool useData );

    private:

    CLanguage* _player_language;

    #ifdef _PLAYER_AUTO_SAVE_
        CTimer * saveTimer; //save every 10 minutes 600 sec's
    #endif
    
        // Status of the player, Okay, waiting authroization, jailed, banned, etc..
        unsigned char status;

        // What time does the status get reset?
        time_t statustime;

        // Who banned/jailed the player?
        TYPE_OF_CHARACTER_ID statusgm;

        // Why where they banned/jailed?
        std::string statusreason;

        // World map turtle graphics
        bool turtleActive;
        unsigned char turtletile;

        // Clipping on/off (default to on)
        bool clippingActive;

        // the world we are in
        //CWorld* m_world;

        //! gibt an, ob der Spieler erweiterte Rechte hat - are they an admin?
        uint32_t admin;

        bool encumberedSent;

        bool unconsciousSent;
        
        bool monitoringClient;

};

// Binary to std::string conversions
template<class from>
const std::string toString(const from& convert) {
    std::stringstream stream;
    stream << convert;
    return stream.str();
}

template<> const std::string toString(const unsigned char& convertme);

#endif
