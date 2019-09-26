#pragma once

#include <string>
#include <queue>

#include "devilution.h"
#include "dvlnet/abstract_net.h"

namespace dvl {
namespace net {

class loopback : public abstract_net {
private:
	std::queue<buffer_t> message_queue;
	buffer_t message_last;
	const int plr_single = 0;

public:
	virtual int create(std::string addrstr, std::string passwd);
	virtual int join(std::string addrstr, std::string passwd);
	virtual bool SNetReceiveMessage(int *sender, char **data, int *size);
	virtual bool SNetSendMessage(int dest, void *data, unsigned int size);
	virtual bool SNetReceiveTurns(char **data, unsigned int *size,
		DWORD *status);
	virtual bool SNetSendTurn(char *data, unsigned int size);
	virtual int SNetGetProviderCaps(struct _SNETCAPS *caps);
	virtual bool SNetRegisterEventHandler(event_type evtype,
		SEVTHANDLER func);
	virtual bool SNetUnregisterEventHandler(event_type evtype,
		SEVTHANDLER func);
	virtual bool SNetLeaveGame(int type);
	virtual bool SNetDropPlayer(int playerid, DWORD flags);
	virtual bool SNetGetOwnerTurnsWaiting(DWORD *turns);
	virtual bool SNetGetTurnsInTransit(int *turns);
	virtual void setup_gameinfo(buffer_t info);
};

} // namespace net
} // namespace dvl
