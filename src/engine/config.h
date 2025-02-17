/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include "kernel.h"

class IConfigManager : public IInterface
{
	MACRO_INTERFACE("config", 0)
public:
	typedef void (*SAVECALLBACKFUNC)(IConfigManager *pConfig, void *pUserData);

	virtual void Init() = 0;
	virtual void Reset() = 0;
	virtual void Reset(const char *pScriptName) = 0;
	virtual bool Save() = 0;
	virtual class CConfig *Values() = 0;

	virtual void RegisterCallback(SAVECALLBACKFUNC pfnFunc, void *pUserData) = 0;

	virtual void WriteLine(const char *pLine) = 0;
	virtual void InfClassWriteLine(const char *pLine) = 0;
};

extern IConfigManager *CreateConfigManager();

#endif
