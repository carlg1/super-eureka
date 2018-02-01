/////////////////////////////////////////////////////////////////////////////////
// #includes
/////////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "udpsrv.h"


/////////////////////////////////////////////////////////////////////////////////
// namespaces
/////////////////////////////////////////////////////////////////////////////////
using namespace std;


/////////////////////////////////////////////////////////////////////////////////
// Class Funtions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// UDPServer()
/////////////////////////////////////////////////////////////////////////////////
UDPServer::UDPServer(uint16_t prt, char *bindadr)
{
	srvfd = -1;

	port = prt;
	bindaddr = bindadr;

	epcb.obj = this;
	epcb.funct = udpsrv_epoller_cb;

	//fix me -- validate now?
}


/////////////////////////////////////////////////////////////////////////////////
// ~UDPServer()
/////////////////////////////////////////////////////////////////////////////////
UDPServer::~UDPServer()
{
	if(srvfd != -1)
		StopServer();
}


/////////////////////////////////////////////////////////////////////////////////
// StartServer()
/////////////////////////////////////////////////////////////////////////////////
bool UDPServer::StartServer(EPoller *ep)
{
	int rv;
	int ov;
	struct sockaddr_in6 sa;

	srvfd = socket(AF_INET6, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if(srvfd < 0)
	{
		cerr << "Failed to obtain 'socket'!" << endl;
		return false;
	}

	ov = 1;
	rv = setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (void *) &ov, 4);
	if(rv != 0)
	{
		typeof(errno) en = errno;
		cerr << "'setsockopt' failed (continuing to run) [" << en << " <" << strerror(en) << ">]!" << endl;
	}

	memset(&sa, 0, sizeof(struct sockaddr_in6));
	sa.sin6_family = AF_INET6;
	sa.sin6_port = htons(port);
	sa.sin6_addr = in6addr_any;

	rv = bind(srvfd, (const sockaddr*) &sa, sizeof(struct sockaddr_in6));

	ep->AddFD(srvfd, EPOLLIN, &epcb);

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// GetData()
/////////////////////////////////////////////////////////////////////////////////
bool UDPServer::GetData()
{
	const char *tmp;
	char port[6];
	ssize_t rv;
	char data[100];
	char ip[INET6_ADDRSTRLEN];
	struct sockaddr_in6 srcaddr;
	socklen_t srcaddrlen = sizeof(struct sockaddr_in6);

	rv = recvfrom(srvfd, data, 100, 0, (struct sockaddr *) &srcaddr, &srcaddrlen);
	if(rv < 0)
	{
		typeof(errno) en = errno;
		cerr << "'recvfrom' failed (continuing to run) [" << en << " <" << strerror(en) << ">]!" << endl;
		return false;
	}

	if(srcaddr.sin6_family == AF_INET)
	{
		tmp = inet_ntop(AF_INET, &((struct sockaddr_in *) &srcaddr)->sin_addr, ip, INET6_ADDRSTRLEN);
		sprintf(port, "%d", ntohs(srcaddr.sin6_port));
	}
	else if(srcaddr.sin6_family == AF_INET6)
	{
		tmp = inet_ntop(AF_INET6, &srcaddr.sin6_addr, ip, INET6_ADDRSTRLEN);
		sprintf(port, "%d", ntohs(srcaddr.sin6_port));
	}
	else
	{
		strcpy(ip, "Not IP");
		strcpy(port, "N/A");
		tmp = ip; //just set tmp to be not null
	}

	if(!tmp)
		strcpy(ip, "inet_ntop failed");

	data[rv] = '\0';
	cout << "Read # " << rv << " of bytes! Data = '" << data << "'" << endl;
	cout << "Received from IP '" << ip << "' and port  = '" << port << "'" << endl;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// StopServer()
/////////////////////////////////////////////////////////////////////////////////
bool UDPServer::StopServer()
{
	close(srvfd);
	return true;
}
