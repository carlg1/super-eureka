#pragma once

/////////////////////////////////////////////////////////////////////////////////
// #includes
/////////////////////////////////////////////////////////////////////////////////


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

	EPoller();
	~EPoller();

	bool Start();
	bool Stop();
	bool Add();
	bool Remove();
	bool Proccess(;)
	epoller_rv Poll();

 private:
	int epollfd;
};
