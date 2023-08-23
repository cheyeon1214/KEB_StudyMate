#pragma once

using namespace cv;

#include "detector.h"

struct bbox_t {
    uint32 x, y, w, h;
    float prob;
    uint32 track_id;
	uint32 track_cnt;
};

class CVFaceDetect
{
private:
	MTCNNDetector * m_pDetector;

	float m_face_size;
	float m_scale_factor;
	float m_thres_NMS;

	codec_scale m_scale;

public:
	BOOL Init(WCHAR* szDir, int thresh);
	void Exit();
	void Set(int dsp_dist, int dsp_size, int thres_nms);
	BOOL Run(VID_IMGSRC* pVid_img, vector<CRect>& re_ary);

public:
	CVFaceDetect();
	~CVFaceDetect();
};
