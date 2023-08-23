#pragma once

#include "g72x.h"

class G726DEC
{
private:
	uint8* m_buf;
	uint32 m_sample_rate;
	uint32 m_channels;

	int m_dec_bit;
	g726_state state_dec;
	int (*dec_routine)(int, int, g726_state*);
	
	bool GetOutputBuff(int nInsize);

public:
	bool Init(uint32 aud_bit, uint32 channel, uint32 samples);
	void Exit();
	bool Decode(uint8* pInput, int nInsize, AUD_SNDSRC* pAud_snd);

public:
	G726DEC();
	virtual ~G726DEC();
};
