#include "infc_binds.h"

#include <game/client/gameclient.h>

void CInfCBinds::OnConsoleInit()
{
	// bindings
	IConfigManager *pConfigManager = Kernel()->RequestInterface<IConfigManager>();
	if(pConfigManager)
		pConfigManager->RegisterCallback(ConfigSaveCallback, this);

	Console()->Register("infc_bind", "s[key] r[command]", CFGFLAG_CLIENT, ConBind, this, "Bind key to execute the command");
	Console()->Register("infc_binds", "?s[key]", CFGFLAG_CLIENT, ConBinds, this, "Print command executed by this keybindind or all binds");
	Console()->Register("infc_unbind", "s[key]", CFGFLAG_CLIENT, ConUnbind, this, "Unbind key");
	Console()->Register("infc_unbindall", "", CFGFLAG_CLIENT, ConUnbindAll, this, "Unbind all keys");

	// default bindings
	SetDefaults();
}

bool CInfCBinds::OnInput(const IInput::CEvent &Event)
{
	if(!GameClient()->m_GameInfo.m_InfClass)
		return false;

	return CBinds::OnInput(Event);
}

void CInfCBinds::ConfigSaveCallback(IConfigManager *pConfigManager, void *pUserData)
{
	CInfCBinds *pSelf = (CInfCBinds *)pUserData;

	char aBuffer[256];
	char *pEnd = aBuffer + sizeof(aBuffer);
	pConfigManager->InfClassWriteLine("infc_unbindall");
	for(int i = 0; i < MODIFIER_COMBINATION_COUNT; i++)
	{
		for(int j = 0; j < KEY_LAST; j++)
		{
			if(!pSelf->m_aapKeyBindings[i][j])
				continue;

			str_format(aBuffer, sizeof(aBuffer), "infc_bind %s%s \"", GetKeyBindModifiersName(i), pSelf->Input()->KeyName(j));
			// process the string. we need to escape some characters
			char *pDst = aBuffer + str_length(aBuffer);
			str_escape(&pDst, pSelf->m_aapKeyBindings[i][j], pEnd);
			str_append(aBuffer, "\"", sizeof(aBuffer));

			pConfigManager->InfClassWriteLine(aBuffer);
		}
	}
}

void CInfCBinds::SetDefaults()
{
	UnbindAll();

	bool FreeOnly = false;
	int MOD_SHIFT_COMBINATION = 1 << MODIFIER_SHIFT;

	Bind(KEY_KP_1, "say_team_location bottomleft", FreeOnly);
	Bind(KEY_KP_2, "say_team_location bottom", FreeOnly);
	Bind(KEY_KP_3, "say_team_location bottomright", FreeOnly);
	Bind(KEY_KP_4, "say_team_location left", FreeOnly);
	Bind(KEY_KP_5, "say_team_location middle", FreeOnly);
	Bind(KEY_KP_6, "say_team_location right", FreeOnly);
	Bind(KEY_KP_7, "say_team_location topleft", FreeOnly);
	Bind(KEY_KP_8, "say_team_location top", FreeOnly);
	Bind(KEY_KP_9, "say_team_location topright", FreeOnly);

	Bind(KEY_KP_1, "say_team_location bottomleft clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_2, "say_team_location bottom clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_3, "say_team_location bottomright clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_4, "say_team_location left clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_5, "say_team_location middle clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_6, "say_team_location right clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_7, "say_team_location topleft clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_8, "say_team_location top clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_KP_9, "say_team_location topright clear", FreeOnly, MOD_SHIFT_COMBINATION);

	Bind(KEY_B, "say_team_location bunker", FreeOnly);
	Bind(KEY_Z, "say_team_location bonuszone", FreeOnly);

	Bind(KEY_B, "say_team_location bunker clear", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_Z, "say_team_location bonuszone clear", FreeOnly, MOD_SHIFT_COMBINATION);

	Bind(KEY_R, "say_message run", FreeOnly);
	Bind(KEY_G, "say_message ghost", FreeOnly);
	Bind(KEY_H, "say_message help", FreeOnly);
	Bind(KEY_W, "say_message where", FreeOnly, MOD_SHIFT_COMBINATION);
	Bind(KEY_F, "say_message bfhf", FreeOnly);
	Bind(KEY_C, "say_message clear", FreeOnly);

	Bind(KEY_W, "witch", FreeOnly);
}
