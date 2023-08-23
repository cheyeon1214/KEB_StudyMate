#include "pch.h"
#include "G726DEC.h"

G726DEC::G726DEC()
{
	m_buf = NULL;
	m_sample_rate = 0;
	m_channels = 0;
	m_dec_bit = 0;
}
G726DEC::~G726DEC()
{
	if(m_buf != NULL)
	{
		delete [] m_buf;
		m_buf = NULL;
	}
}

bool G726DEC::Init(uint32 aud_bit, uint32 channel, uint32 samples)
{
	g726_init_state(&state_dec);

	m_dec_bit = (int)(aud_bit / 8);
	m_sample_rate = samples;
	m_channels = channel;

	switch(m_dec_bit)
	{
	case 2: dec_routine = &g726_16_decoder; break;
	case 3: dec_routine = &g726_24_decoder; break;
	case 4: dec_routine = &g726_32_decoder; break;
	case 5: dec_routine = &g726_40_decoder; break;
	}

	return true;
}
void G726DEC::Exit()
{
	if(m_buf != NULL)
	{
		delete [] m_buf;
		m_buf = NULL;
	}
}

bool G726DEC::Decode(uint8* pInput, int nInsize, AUD_SNDSRC* pAud_snd)
{
	if(m_buf == NULL)
	{
		if(!GetOutputBuff(nInsize))
			return false;
	}

	short* dst = (short*)m_buf;
	int n_sample = nInsize * 8 / m_dec_bit;
	
	uint8 * in = pInput;
	uint8 cw[8];

	int n_pos = 0;
	for(int i=0; i<n_sample; i+=8)
	{
		in += g726_unpack(cw, in, 8, m_dec_bit);
		for(int j=0; j<8; j++)
		{
			dst[n_pos] = dec_routine(cw[j], AUDIO_ENCODING_LINEAR, &state_dec);
			n_pos++;
		}
	}
	int n_res = n_sample - n_pos;
	if(n_res)
	{
		in += g726_unpack(cw, in, n_res, m_dec_bit);
		for(int j=0; j<n_res; j++)
		{
			dst[n_pos] = dec_routine(cw[j], AUDIO_ENCODING_LINEAR, &state_dec);
			n_pos++;
		}
	}
	if(n_sample)
	{
		pAud_snd->audbytes = 2 * n_sample;
		pAud_snd->aud_ptr = m_buf;
		return true;
	}
	return false;
}

bool G726DEC::GetOutputBuff(int nInsize)
{
	if(m_buf != NULL)
	{
		delete [] m_buf;
		m_buf = NULL;
	}
	m_buf = new uint8 [MAX_AUDIO_SIZE];

	return true;
}
