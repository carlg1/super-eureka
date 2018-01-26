/////////////////////////////////////////////////////////////////////////////////
// #includes
/////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <unistd.h>

#include "config.h"
#include "udpsrv.h"
#include "epoller.h"


/////////////////////////////////////////////////////////////////////////////////
// #defines
/////////////////////////////////////////////////////////////////////////////////
#define EPOLL_TIMEOUT 10 //10 msec


/////////////////////////////////////////////////////////////////////////////////
// namespaces
/////////////////////////////////////////////////////////////////////////////////
using namespace std;


/////////////////////////////////////////////////////////////////////////////////
// Enums
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Structs
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Classes
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Global Vars
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// TODO
/////////////////////////////////////////////////////////////////////////////////
//Epoller could have -fdcnt (which is bad)


/////////////////////////////////////////////////////////////////////////////////
// Funcation Prototypes
/////////////////////////////////////////////////////////////////////////////////
static bool Initilaize();
static void Shutdown();


/////////////////////////////////////////////////////////////////////////////////
// Local Vars
/////////////////////////////////////////////////////////////////////////////////
tbdFlowConfig tbdconf;
UDPServer *srv;
EPoller epoller(EPOLL_TIMEOUT);


/////////////////////////////////////////////////////////////////////////////////
// main()
/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	EPoller::epoller_rv eprv;

	if(!tbdconf.ParseCmdArgs(argc, argv))
	{
		cout << "ParseCmdArgs Failed!" << endl;
		exit(1);
	}

	if(!Initilaize())
	{
		cerr << "Initilaize Failed!" << endl;
		exit(1);
	}

	cout << "Starting Main Loop!" << endl;
	while(tbdconf.Running())
	{
		eprv = epoller.Poll();
		if(eprv == EPoller::EPOLLER_DATA)
		{
			if(!epoller.ProccessLoop())
				break;

			//char data[100];
			//int len = srv->GetData(data, 100);
			//data[len] = '\0';
			//cout << "Read # " << len << " of bytes! Data = '" << data << "'" << endl; 
		}
		else if(eprv == EPoller::EPOLLER_ERR)
			break;
	}

	Shutdown();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// Local Funtions
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Initilaize
/////////////////////////////////////////////////////////////////////////////////
bool Initilaize()
{
	if(!tbdconf.ParseConfigFile())
		return false;

	if(tbdconf.GetPort() == 0)
	{
		cerr << "No port defined" << endl;
		return false;
	}

	srv = new UDPServer(tbdconf.GetPort());
	srv->StartServer();

	if(!epoller.Ready())
	{
		cerr << "epoller not ready!" << endl;
		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// Initilaize
/////////////////////////////////////////////////////////////////////////////////
void Shutdown()
{
	delete srv;
	//tbdconf.
}
