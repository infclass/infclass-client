#ifndef GAME_SERVER_TEEINFO_H
#define GAME_SERVER_TEEINFO_H

#include <engine/shared/protocol.h>
#include <game/generated/protocol.h>

class CTeeInfo
{
public:
	constexpr static const float ms_DarkestLGT7 = 61 / 255.0f;

	char m_aSkinName[64] = {'\0'};
	int m_UseCustomColor = 0;
	int m_ColorBody = 0;
	int m_ColorFeet = 0;

	// 0.7
	char m_apSkinPartNames[NUM_SKINPARTS][MAX_SKIN_LENGTH]{};
	bool m_aUseCustomColors[NUM_SKINPARTS]{};
	int m_aSkinPartColors[NUM_SKINPARTS]{};

	CTeeInfo() = default;

	CTeeInfo(const char *pSkinName, int UseCustomColor, int ColorBody, int ColorFeet);
	CTeeInfo(const char *apSkinPartNames[NUM_SKINPARTS], const int (&aUseCustomColors)[NUM_SKINPARTS], const int (&aSkinPartColors)[NUM_SKINPARTS]);

	void FromSixup();
	void ToSixup();
};
#endif //GAME_SERVER_TEEINFO_H
