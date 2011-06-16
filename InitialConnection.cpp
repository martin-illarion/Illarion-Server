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


#include "InitialConnection.h"
#include <boost/thread.hpp>
#include <sstream>
#define InitialConnection_DEBUG

CInitialConnection::CInitialConnection() 
{
#ifdef InitialConnection_DEBUG
	std::cout << "CInternetConnection Konstruktor Start\n";
#endif
        boost::thread servicethread(boost::bind(&CInitialConnection::run_service,this));
#ifdef InitialConnection_DEBUG
	std::cout << "CInternetConnection Konstruktor Ende\n";
#endif
}


CInitialConnection::TVECTORPLAYER& CInitialConnection::get_Player_Vector()
{
	return playerVector;
}

void CInitialConnection::run_service()
{
    int port;
    std::stringstream ss;
    ss << configOptions["port"];
    ss >> port;

    boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
    acceptor = new boost::asio::ip::tcp::acceptor(io_service,endpoint);
    boost::shared_ptr<CNetInterface> newConnection(new CNetInterface(io_service));
    acceptor->async_accept(newConnection->getSocket(),boost::bind(&CInitialConnection::accept_connection, this, newConnection, boost::asio::placeholders::error));
    std::cout<<"Starting the IO Service!"<<std::endl;
    io_service.run();
}

//CInternetConnection* CInitialConnection::accept_connection() {
void CInitialConnection::accept_connection(boost::shared_ptr<CNetInterface> connection, const boost::system::error_code& error)
{
    if (!error)
    {
        if (connection->activate())
        {
            //Verbindung in die Liste aufnehmen
            playerVector.push_back(connection);
        }
        else
        {
            std::cerr<<"Fehler bei Aktivierung der Connection"<<std::endl;
        }
        boost::shared_ptr<CNetInterface> newConnection(new CNetInterface(io_service));
        acceptor->async_accept(newConnection->getSocket(),boost::bind(&CInitialConnection::accept_connection, this, newConnection, boost::asio::placeholders::error));
    }
    else
    {
        std::cerr<<"Fehler im Accept:" << error.message() << ": " <<error.value() <<std::endl;
    }
        
}



CInitialConnection::~CInitialConnection() {
#ifdef InitialConnection_DEBUG
	std::cout << "CInternetConnection Destruktor Start" << std::endl;
#endif
        io_service.stop();
        delete acceptor;
        acceptor = NULL;
#ifdef InitialConnection_DEBUG
	std::cout << "CInternetConnection Destruktor Ende" << std::endl;
#endif
}
