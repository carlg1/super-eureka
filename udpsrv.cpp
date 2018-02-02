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
/////////////////////////////////////////////////////////////////////////////////
//
// Class Funtions
//
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// UDPServer()
/////////////////////////////////////////////////////////////////////////////////
UDPServer::UDPServer(const uint16_t _port, const char *const _bindaddr)
{
	srvfd = -1;

	if(_port < 0 || _port > 65535)
		throw invalid_argument("_port must be between 1 and 65536");
	else
		port = _port;

	if(_bindaddr && !ValidateIP(_bindaddr))
		throw invalid_argument("_bindaddr must be a valide IPv4 or IPv6 addresss");

	bindaddr = _bindaddr;

	epcb.obj = this;
	epcb.funct = udpsrv_epoller_cb;
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
	int optval;
	int addrlen;
	struct sockaddr_in6 addr;

	srvfd = socket(AF_INET6, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if(srvfd < 0)
	{
		typeof(errno) en = errno;
		cerr << "'socket' failed [" << en << " <" << strerror(en) << ">]!" << endl;
		return false;
	}

	optval = 1;
	rv = setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (void *) &optval, sizeof(int));
	if(rv != 0)
	{
		typeof(errno) en = errno;
		cerr << "'setsockopt' failed [" << en << " <" << strerror(en) << ">]!" << endl;

		StopServer();
		return false;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in6));
	if(!bindaddr.empty())
	{
		void *dst;
		struct sockaddr_in  *addr4 = (sockaddr_in *) &addr;
		struct sockaddr_in6 *addr6 = &addr;

		if(strchr(bindaddr.c_str(), ':') == nullptr)
		{
			dst = &addr4->sin_addr.s_addr;
			addr4->sin_family = AF_INET;
			addrlen = sizeof(struct sockaddr_in);
		}
		else
		{
			dst = addr6->sin6_addr.s6_addr;
			addr6->sin6_family = AF_INET6;
			addrlen = sizeof(struct sockaddr_in6);
		}

		inet_pton(addr.sin6_family, bindaddr.c_str(), &dst);
	}
	else
	{
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		addr.sin6_addr = in6addr_any;
		addrlen = sizeof(struct sockaddr_in6);
	}

	rv = bind(srvfd, (const sockaddr*) &addr, addrlen);
	if(rv != 0)
	{
		typeof(errno) en = errno;
		cerr << "'bind' failed [" << en << " <" << strerror(en) << ">]!" << endl;

		StopServer();
		return false;
	}

	if(!ep->AddFD(srvfd, EPOLLIN, &epcb))
	{
		StopServer();
		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// StopServer()
/////////////////////////////////////////////////////////////////////////////////
bool UDPServer::StopServer()
{
	close(srvfd);
	srvfd = -1;

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
		tmp = ip; //just to set tmp to be not null
		strcpy(ip, "Not IP");
		strcpy(port, "N/A");
	}

	if(!tmp)
		strcpy(ip, "inet_ntop failed");

	data[rv] = '\0';
	cout << "Read # " << rv << " of bytes! Data = '" << data << "'" << endl;
	cout << "Received from IP '" << ip << "' and port  = '" << port << "'" << endl;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// ValidateIP()
/////////////////////////////////////////////////////////////////////////////////
bool UDPServer::ValidateIP(const char *const addr)
{
	int af;
	struct in6_addr dst;

	if(addr == nullptr)
		return false;

	if(strchr(addr, ':') == nullptr)
		af = AF_INET;
	else
		af = AF_INET6;

	if(inet_pton(af, addr, &dst) != 1)
		return false;

	return true;
}
