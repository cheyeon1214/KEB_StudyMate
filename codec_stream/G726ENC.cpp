#include "pch.h"
#include "G726ENC.h"

G726ENC::G726ENC()
{
	m_sample_rate = 0;
	m_channels = 0;
	m_enc_bit = 0;
}
G726ENC::~G726ENC()
{
}

bool G726ENC::Init(uint32 aud_bit, uint32 channel, uint32 samples)
{
	g726_init_state(&state_enc);

	m_enc_bit = (int)(aud_bit / 8);
	m_sample_rate = samples;
	m_channels = channel;

	switch(m_enc_bit)
	{
	case 2: enc_routine = &g726_16_encoder; break;
	case 3: enc_routine = &g726_24_encoder; break;
	case 4: enc_routine = &g726_32_encoder; break;
	case 5: enc_routine = &g726_40_encoder; break;
	}

	return true;
}
void G726ENC::Exit()
{
}

bool G726ENC::Encode(AUD_SNDSRC* pAud_snd, uint8* pOutput, int & nOutsize)
{
	short* src = (short*)pAud_snd->aud_ptr;
	int n_sample = pAud_snd->audbytes / 2;
	
	uint8 * out = pOutput;
	uint8 cw[8];

	int n_pos = 0;
	for(int i=0; i<n_sample; i+=8)
	{
		for(int j=0; j<8; j++)
		{
			cw[j] = enc_routine(src[n_pos], AUDIO_ENCODING_LINEAR, &state_enc);
			n_pos++;
		}
		out += g726_pack(out, cw, 8, m_enc_bit);
	}
	int n_res = n_sample - n_pos;
	if(n_res)
	{
		for(int j=0; j<n_res; j++)
		{
			cw[j] = enc_routine(src[n_pos], AUDIO_ENCODING_LINEAR, &state_enc);
			n_pos++;
		}
		out += g726_pack(out, cw, n_res, m_enc_bit);
	}

	nOutsize = (int)(out - pOutput);
	return true;
}
