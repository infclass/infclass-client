#ifndef GAME_SERVER_GAMEMODES_MOD_H
#define GAME_SERVER_GAMEMODES_MOD_H

#include <game/server/gamecontroller.h>

class CGameControllerMod : public IGameController
{
public:
	CGameControllerMod(class CGameContext *pGameServer);
	~CGameControllerMod();

	CPlayer *CreatePlayer(int ClientID, int StartTeam) override;

	void Tick() override;

	using IGameController::GameServer;
};
#endif // GAME_SERVER_GAMEMODES_MOD_H
