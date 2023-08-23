#ifndef _CODEC_STREAM_H_
#define _CODEC_STREAM_H_

#ifdef _API_STATIC_LIB_
	#define API_CODEC_STREAM
#else
	#ifdef _CODEC_STREAM_DLL_
		#define API_CODEC_STREAM __declspec(dllexport)
	#else
		#define API_CODEC_STREAM __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/codec_stream.lib")
				#pragma message("Automatically linking with ../x64/libd/codec_stream.lib")
			#else
				#pragma comment(lib, "../x64/lib/codec_stream.lib")
				#pragma message("Automatically linking with ../x64/lib/codec_stream.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/codec_stream.lib")
				#pragma message("Automatically linking with ../x86/libd/codec_stream.lib")
			#else
				#pragma comment(lib, "../x86/lib/codec_stream.lib")
				#pragma message("Automatically linking with ../x86/lib/codec_stream.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

BOOL API_CODEC_STREAM _codec_ffmpeg_Init();
void API_CODEC_STREAM _codec_ffmpeg_Exit();

class H264ENC;
class API_CODEC_STREAM codec_video_enc
{
private:
	H264ENC* m_penc;

public:
	bool Init(int hsize, int vsize, int fps, int quality);
	void Exit();
	bool Encode(VID_IMGSRC* pVid_img, uint8* pOutput, int& nOutsize, BOOL& bRecvI);

public:
	codec_video_enc();
	virtual ~codec_video_enc();
};

class H264DEC;
class API_CODEC_STREAM codec_video_dec
{
private:
	H264DEC* m_pdec;

public:
	bool Init();
	void Exit();
	bool Decode(uint8* pInput, int nInsize, VID_IMGSRC* pVid_img);

public:
	codec_video_dec();
	virtual ~codec_video_dec();
};

class G726ENC;
class API_CODEC_STREAM codec_audio_enc
{
private:
	G726ENC* m_penc;

public:
	bool Init(uint32 aud_bit, uint32 channel, uint32 samples);
	void Exit();
	bool Encode(AUD_SNDSRC* pAud_snd, uint8* pOutput, int& nOutsize);

public:
	codec_audio_enc();
	virtual ~codec_audio_enc();
};

class G726DEC;
class API_CODEC_STREAM codec_audio_dec
{
private:
	G726DEC* m_pdec;

public:
	bool Init(uint32 aud_bit, uint32 channel, uint32 samples);
	void Exit();
	bool Decode(uint8* pInput, int nInsize, AUD_SNDSRC* pAud_snd);

public:
	codec_audio_dec();
	virtual ~codec_audio_dec();
};

/*
* Image Scaling with format transform
*/
class ScaleImage;
class API_CODEC_STREAM codec_scale
{
private:
	ScaleImage * m_pscl;

public:
	void Clear();
	BOOL ScaleVideo(VID_IMGSRC* pVid_src, VID_IMGSRC* pVid_dst);

public:
	codec_scale();
	virtual ~codec_scale();
};

#endif