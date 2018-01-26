#pragma once

/////////////////////////////////////////////////////////////////////////////////
// #includes
/////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>


/////////////////////////////////////////////////////////////////////////////////
// Class Defintion
/////////////////////////////////////////////////////////////////////////////////
class UDPServer
{
public:
	UDPServer() = delete;
	UDPServer(uint16_t prt, char *bindadr = nullptr);
	~UDPServer();

	bool StartServer();
	bool StopServer();
	int GetData(void *buff, const int bufflen);
	bool EPollCheck();

 private:
	int srvfd;
	char *bindaddr;
	uint16_t port;
};


class EPoller
{
public:
	enum epoller_rv {
		EPOLLER_ERR,
		EPOLLER_DATA,
		EPOLLER_NODATA,
	};

	EPoller();
	~EPoller();

	bool Start();
	bool Stop();
	bool Add();
	bool Remove();
	bool Proccess();
	epoller_rv Poll();

 private:
	int epollfd;
};
