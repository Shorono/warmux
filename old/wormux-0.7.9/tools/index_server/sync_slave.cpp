#include <list>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "sync_slave.h"
#include "config.h"
#include "client.h"
#include "net_data.h"
#include "download.h"
#include "debug.h"
#include "../../src/network/index_svr_msg.h"

SyncSlave sync_slave;

// map < version, ip >
std::multimap<std::string, FakeClient> fake_clients;

SyncSlave::SyncSlave()
{
}

SyncSlave::~SyncSlave()
{
	for(iterator it = begin();
		it != end();
		++it)
	{
		delete (*it);
	}
}


static ssize_t getline(std::string& line, std::ifstream& file)
{
        line.clear();
        std::getline(file, line);
	if(file.eof())
		return -1;
	return line.size();
}

bool SyncSlave::Start()
{
	const std::string server_fn = "./server_list";
	downloader.Get( server_list_url.c_str(), server_fn.c_str() );

	std::ifstream fin;
	fin.open(server_fn.c_str(), std::ios::in);
	if(!fin)
		return false;

	ssize_t read;
	std::string line;

	std::string my_hostname;
	config.Get( "my_hostname", my_hostname);
 
	// GNU getline isn't available on *BSD and Win32, so we use a new function, see getline above
	while ((read = getline(line, fin)) >= 0)
	{
		if(line.size() == 0 || line.at(0) == '#' )
			continue;

		std::string::size_type port_pos = line.find(':', 0);
		if(port_pos == std::string::npos)
			continue;

		std::string hostname = line.substr(0, port_pos);
		std::string portstr = line.substr(port_pos+1);
		int port = atoi(portstr.c_str());

		if( hostname != my_hostname )
			push_back( new IndexServerConn( hostname, port ) );
//  		push_back( new IndexServerConn( "localhost", 9997 ) );
	}
	fin.close();
	return true;
}

void SyncSlave::CheckGames()
{
	iterator it = begin();
	while(it != end())
	{
		int received;
		bool result = false;
				
		if( ioctl( (*it)->GetFD(), FIONREAD, &received) == -1 )
		{
			PRINT_ERROR;
			result = false;
		}

		if( received == 0 || (*it)->Receive() )
			result = true;

		if(!result)
		{
			delete (*it);
			it = erase(it);
		}
		else
			++it;
	}
}


IndexServerConn::IndexServerConn(const std::string &addr, int port)
{
	DPRINT(INFO, "Contacting index server at %s ...",addr.c_str());
    	ConnectTo( addr, port);
	SendInt((int)TS_MSG_VERSION);
	SendStr("WIS");
}

IndexServerConn::~IndexServerConn()
{
}

bool IndexServerConn::HandleMsg(const std::string & full_str)
{
	int nbr;
	int nbr2;
	switch(msg_id)
	{
	case TS_MSG_VERSION:
		if(full_str != "MassMurder!")
		{
			DPRINT(INFO,"Doesn't seem do be a valid server ..");
			return false;
		}
		DPRINT(INFO,"Server identified..");
		break;
	case TS_MSG_WIS_VERSION:
		if( received < 4 ) // The message is not completely received
			return true;
		if(!ReceiveInt(nbr)) // Receive the version number of the server index
			return false;
		if(nbr > VERSION)
		{
			DPRINT(INFO,"This server have a version %i, while we are only running a %i version",nbr, VERSION);
			exit(EXIT_FAILURE);
		}
		else
		if(nbr < VERSION)
		{
			DPRINT(INFO,"This server is running an old version (v%i) !",nbr);
			return false;
		}
		DPRINT(INFO,"We are running the same verion..");
		break;
	case TS_MSG_JOIN_LEAVE:
		if( received < 8 ) // The message is not completely received
			return true;
		if(!ReceiveInt(nbr)) // Receive the IP of the wormux server
			return false;
		if(!ReceiveInt(nbr2)) // Receive the port of the wormux server
			return false;
		if(nbr2 < 0) // means it disconnected
		{
			std::multimap<std::string, FakeClient>::iterator serv = fake_clients.find( full_str );
			if( serv != fake_clients.end() )
			{
				do
				{
					if( serv->second.ip == nbr 
					&&  serv->second.port == - nbr2 ) 
					{
						fake_clients.erase( serv );
						DPRINT(MSG, "A fake server disconnected");
						break;
					}
				} while (serv != fake_clients.upper_bound(full_str));
			}
		}
		else
		{
			fake_clients.insert( std::make_pair(full_str, FakeClient(nbr, nbr2)));
		}
		break;
	default:
		DPRINT(INFO,"Bad message!");
		return false;
	}
	msg_id = TS_NO_MSG;
	return true;
}
