/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#ifndef MACRO_CONFIG_INT

#error "Config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc);
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc);
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc);

#endif

MACRO_CONFIG_STR(ClAssetInfclass, cl_asset_infclass, 50, "default", CFGFLAG_SAVE | CFGFLAG_CLIENT, "The asset for infclass")
MACRO_CONFIG_INT(ClInfStatusSize, infc_status_size, 16, 4, 64, CFGFLAG_SAVE | CFGFLAG_CLIENT, "The size of infclass status icons")
MACRO_CONFIG_INT(InfcShowBoomerWeapon, infc_show_boomer_weapon, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Toggle the Boomer hammer visibility")
