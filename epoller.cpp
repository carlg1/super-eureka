/////////////////////////////////////////////////////////////////////////////////
// #includes
/////////////////////////////////////////////////////////////////////////////////
//#include <iostream>
//
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
//#include <fcntl.h>

#include "eplller.h"


/////////////////////////////////////////////////////////////////////////////////
// namespaces
/////////////////////////////////////////////////////////////////////////////////
using namespace std;


/////////////////////////////////////////////////////////////////////////////////
// Class Funtions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// EPoller()
/////////////////////////////////////////////////////////////////////////////////
EPoller::EPoller()
{
	if((epollfd = epoll_create1(EPOLL_CLOEXEC)) < 0)
	{
		typeof(errno) en = errno;
		cerr << "'epoll_create1' failed [" << en << " <" << strerror(en) << ">]!" << endl;
		return; //fix me -- throw an exception
	}
}


/////////////////////////////////////////////////////////////////////////////////
// ~EPoller()
/////////////////////////////////////////////////////////////////////////////////
EPoller::~EPoller()
{
	if(srvfd != -1)
		StopServer();
}


/////////////////////////////////////////////////////////////////////////////////
// StartServer()
/////////////////////////////////////////////////////////////////////////////////
bool EPoller::StartServer()
{
	int rv;
	int ov;
	int flags;
	struct epoll_event nepe;
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

	if((flags = fcntl(srvfd, F_GETFL, 0)) < 0)
	{
		typeof(errno) en = errno;
		cerr << "'fcntl' failed getting flags [" << en << " <" << strerror(en) << ">]!" << endl;
		return false;
	}

	if(fcntl(srvfd, F_SETFL, flags | O_NONBLOCK) < 0) 
	{
		typeof(errno) en = errno;
		cerr << "'fcntl' failed getting flags [" << en << " <" << strerror(en) << ">]!" << endl;
		return false;
	}

	//nepe.events = EPOLLIN;
	//
	//if(epoll_ctl(epollfd, EPOLL_CTL_ADD, srvfd, &nepe) < 0)
	//{
	//	typeof(errno) en = errno;
	//	cerr << "'epoll_create1' failed [" << en << " <" << strerror(en) << ">]!" << endl;
	//	return false;
	//}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// GetData()
/////////////////////////////////////////////////////////////////////////////////
int EPoller::GetData(void *buff, const int bufflen)
{
	ssize_t rv;
	rv = recvfrom(srvfd, buff, bufflen, 0, NULL, NULL);
	return rv;
}


/////////////////////////////////////////////////////////////////////////////////
// StopServer()
/////////////////////////////////////////////////////////////////////////////////
bool EPoller::StopServer()
{
	//close(epollfd);
	close(srvfd);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// EPollCheck()
/////////////////////////////////////////////////////////////////////////////////
//bool EPoller::EPollCheck()
//{
//#define MAX_EVENTS 2
//	int nfds;
//	struct epoll_event events[MAX_EVENTS];
//	nfds = epoll_wait(epollfd, events, MAX_EVENTS, 0);
//	if(nfds < 1)
//		return false;
//	return true;
//#undef MAX_EVENTS
//}
