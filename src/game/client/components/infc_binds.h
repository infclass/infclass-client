/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_INFC_BINDS_H
#define GAME_CLIENT_COMPONENTS_INFC_BINDS_H

#include "binds.h"

class IConfigManager;

class CInfCBinds : public CBinds
{
	static void ConfigSaveCallback(IConfigManager *pConfigManager, void *pUserData);

public:
	CInfCBinds() = default;
	~CInfCBinds() = default;

	virtual void OnConsoleInit();
	virtual bool OnInput(IInput::CEvent Event);

	void LoadPreset();
};
#endif
