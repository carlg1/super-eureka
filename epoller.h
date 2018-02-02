#pragma once

/////////////////////////////////////////////////////////////////////////////////
// #includes
/////////////////////////////////////////////////////////////////////////////////
#include <sys/epoll.h>


/////////////////////////////////////////////////////////////////////////////////
// Struct
/////////////////////////////////////////////////////////////////////////////////
typedef struct epoller_cb_
{
	void *obj;
	bool (*funct)(void *obj, int events);
} epoller_cb_t;


/////////////////////////////////////////////////////////////////////////////////
// Class Defintion
/////////////////////////////////////////////////////////////////////////////////
class EPoller
{
public:
	enum epoller_rv {
		EPOLLER_ERR,
		EPOLLER_DATA,
		EPOLLER_NODATA,
	};

	EPoller() = delete;
	EPoller(int _timeout = 0);
	//_timemout -- 0 == don't wait, -1 == block until event, or wait for 'to' msec (max 10,000 msec [10 sec])
	~EPoller();

	void Shutdown();
	bool AddFD(const int fd, const int epoll_events, const epoller_cb_t *cb);
	bool RemoveFD(const int fd);
	bool ModifyFD(const int fd, const int epoll_events, const epoller_cb_t *cb);
	epoller_rv Poll();
	bool ProccessLoop();

 private:
	bool epollerctlfd(const int fd, const int epoll_events, const epoller_cb_t *cb, const int op);

	int timeout;
	int epollfd;

	int rdyfds;
	int maxevtcnt;
	struct epoll_event *events;
};
