#pragma once

#include "g72x.h"

class G726ENC
{
private:
	uint32 m_sample_rate;
	uint32 m_channels;

	int m_enc_bit;
	g726_state state_enc;
	int (*enc_routine)(int, int, g726_state*);

public:
	bool Init(uint32 aud_bit, uint32 channel, uint32 samples);
	void Exit();
	bool Encode(AUD_SNDSRC* pAud_snd, uint8* pOutput, int & nOutsize);

public:
	G726ENC();
	virtual ~G726ENC();
};
