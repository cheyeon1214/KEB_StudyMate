#pragma once

class ScaleImage
{
private:
	struct SwsContext * m_img_ctx;
	AVFrame * picture_yuv;

	uint8 *	m_ptr;

	AVPixelFormat src_fmt;
	uint32 src_f_col;
	int src_hsize, src_vsize;

	AVPixelFormat dst_fmt;
	uint32 dst_f_col;
	int dst_hsize, dst_vsize;

public:
	bool Init();
	void Exit();
	void Clear();
	bool Scaling(VID_IMGSRC * pVid_src, VID_IMGSRC * pVid_dst);

public:
	ScaleImage();
	virtual ~ScaleImage();
};
