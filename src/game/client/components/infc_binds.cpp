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
}
