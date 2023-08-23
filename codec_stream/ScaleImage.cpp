#include "pch.h"
#include "ScaleImage.h"

ScaleImage::ScaleImage()
{
	m_img_ctx = NULL;
	picture_yuv = NULL;

	m_ptr = NULL;

	Clear();
}
ScaleImage::~ScaleImage()
{
	Exit();
}

bool ScaleImage::Init()
{
	picture_yuv = av_frame_alloc();

	return true;
}
void ScaleImage::Exit()
{
	Clear();

	if(picture_yuv != NULL)
	{
		av_frame_free(&picture_yuv);
		picture_yuv = NULL;
	}
}
void ScaleImage::Clear()
{
	if(m_ptr != NULL)
	{
		av_free(m_ptr);
		m_ptr = NULL;
	}

	src_fmt = AV_PIX_FMT_NONE;
	src_f_col = 0;
	src_hsize = 0;
	src_vsize = 0;

	dst_fmt = AV_PIX_FMT_NONE;
	dst_f_col = 0;
	dst_hsize = 0;
	dst_vsize = 0;
}
bool ScaleImage::Scaling(VID_IMGSRC * pVid_src, VID_IMGSRC * pVid_dst)
{
	bool b_new = false;
	if(m_ptr == NULL) b_new = true;
	if(src_f_col != pVid_src->f_col) b_new = true;
	if(src_hsize != pVid_src->hsize) b_new = true;
	if(src_vsize != pVid_src->vsize) b_new = true;
	if(dst_f_col != pVid_dst->f_col) b_new = true;
	if(dst_hsize != pVid_dst->hsize) b_new = true;
	if(dst_vsize != pVid_dst->vsize) b_new = true;

	if(b_new)
	{
		if(m_ptr != NULL)
		{
			av_free(m_ptr);
			m_ptr = NULL;
		}

		src_fmt = AV_PIX_FMT_NONE;
		switch(pVid_src->f_col)
		{
		case FOURCC_YV12: src_fmt = AV_PIX_FMT_YUV420P; break;
		case FOURCC_BGRA: src_fmt = AV_PIX_FMT_BGRA; break;
		case FOURCC_BGR24: src_fmt = AV_PIX_FMT_BGR24; break;
		}
		if(src_fmt == AV_PIX_FMT_NONE)
			return false;

		src_f_col = pVid_src->f_col;
		src_hsize = pVid_src->hsize;
		src_vsize = pVid_src->vsize;

		dst_fmt = AV_PIX_FMT_NONE;
		switch(pVid_dst->f_col)
		{
		case FOURCC_YV12: dst_fmt = AV_PIX_FMT_YUV420P; break;
		case FOURCC_BGRA: dst_fmt = AV_PIX_FMT_BGRA; break;
		case FOURCC_BGR24: dst_fmt = AV_PIX_FMT_BGR24; break;
		}
		if(dst_fmt == AV_PIX_FMT_NONE)
			return false;

		dst_f_col = pVid_dst->f_col;
		dst_hsize = pVid_dst->hsize;
		dst_vsize = pVid_dst->vsize;

		int numBytes = av_image_get_buffer_size(dst_fmt, dst_hsize, dst_vsize, 1);
		m_ptr = (uint8*)av_malloc(numBytes);
		av_image_fill_arrays(picture_yuv->data, picture_yuv->linesize, m_ptr, dst_fmt, dst_hsize, dst_vsize, 1);

		m_img_ctx = sws_getContext(
			src_hsize, src_vsize, src_fmt,
			dst_hsize, dst_vsize, dst_fmt,
			SWS_FAST_BILINEAR, NULL, NULL, NULL
			);
		if(m_img_ctx == NULL)
		{
			Clear();
			return false;
		}
	}

	uint8 * data[3] = {0,0,0};
	int linesize[3] = {0,0,0};
	switch(src_fmt)
	{
	case AV_PIX_FMT_YUV420P:
		{
			data[0] = pVid_src->y_src;
			data[1] = pVid_src->u_src;
			data[2] = pVid_src->v_src;
			linesize[0] = pVid_src->y_stride;
			linesize[1] = pVid_src->uv_stride;
			linesize[2] = pVid_src->uv_stride;
		}
		break;
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_BGR24:
		{
			data[0] = pVid_src->x_ptr;
			linesize[0] = pVid_src->x_stride;
		}
		break;
	}

	sws_scale(m_img_ctx,
		data, linesize, 0, src_vsize,
		picture_yuv->data, picture_yuv->linesize
		);

	AVFrame *picture = picture_yuv;

	switch(dst_fmt)
	{
	case AV_PIX_FMT_YUV420P:
		{
			pVid_dst->y_src = picture->data[0];
			pVid_dst->u_src = picture->data[1];
			pVid_dst->v_src = picture->data[2];
			pVid_dst->y_stride = picture->linesize[0];
			pVid_dst->uv_stride = picture->linesize[1];
			return true;
		}
		break;
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_BGR24:
		{
			pVid_dst->x_ptr = picture->data[0];
			pVid_dst->x_stride = picture->linesize[0];
			return true;
		}
		break;
	}

	return false;
}
