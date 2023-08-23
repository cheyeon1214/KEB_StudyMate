#include "pch.h"
#include "CVFaceDetect.h"

CVFaceDetect::CVFaceDetect()
{
	m_pDetector = NULL;

	m_face_size = 50.0f;
	m_scale_factor = 0.709f;
	m_thres_NMS = 0.5f;
}
CVFaceDetect::~CVFaceDetect()
{
	Exit();
}

BOOL CVFaceDetect::Init(WCHAR* szDir, int thresh)
{
	char czDir[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, szDir, -1, czDir, MAX_PATH, NULL, NULL);

	std::string modelPath = czDir;

	ProposalNetwork::Config pConfig;
	pConfig.caffeModel = modelPath + "/det1.caffemodel";
	pConfig.protoText = modelPath + "/det1.prototxt";
	pConfig.threshold = 0.6f + (((float)thresh - 5.0f) / 20.0f);
	
	RefineNetwork::Config rConfig;
	rConfig.caffeModel = modelPath + "/det2.caffemodel";
	rConfig.protoText = modelPath + "/det2.prototxt";
	rConfig.threshold = 0.7f;// + (((float)thresh - 5.0f) / 20.0f);
	
	OutputNetwork::Config oConfig;
	oConfig.caffeModel = modelPath + "/det3.caffemodel";
	oConfig.protoText = modelPath + "/det3.prototxt";
	oConfig.threshold = 0.7f;// + (((float)thresh - 5.0f) / 20.0f);

	m_pDetector = new MTCNNDetector (pConfig, rConfig, oConfig);

	return TRUE;
}
void CVFaceDetect::Exit()
{
	if(m_pDetector)
	{
		delete m_pDetector;
		m_pDetector = NULL;
	}
}
void CVFaceDetect::Set(int dsp_dist, int dsp_size, int thres_nms)
{
	m_face_size = 50.0f;
	m_scale_factor = 0.709f;
	switch(dsp_dist)
	{
	case 0: m_face_size = 50.0f; break;
	case 1: m_face_size = 100.0f; break;
	case 2: m_face_size = 150.0f; break;
	}
	switch(dsp_size)
	{
	case 0: m_face_size /= 1.0f; m_scale_factor /= 1.0f; break;
	case 1: m_face_size /= 2.0f; m_scale_factor /= 2.0f; break;
	case 2: m_face_size /= 4.0f; m_scale_factor /= 4.0f; break;
	}
	m_thres_NMS = (float)thres_nms / 100.0f;
}
BOOL CVFaceDetect::Run(VID_IMGSRC* pVid_img, vector<CRect>& re_ary)
{
	if(!m_pDetector) return FALSE;

	int type = 0;
	switch(pVid_img->f_col)
	{
	case FOURCC_BGRA: type = CV_8UC4; break;
	case FOURCC_BGR24: type = CV_8UC3; break;
	}
	
	int hsize = pVid_img->hsize;
	int vsize = pVid_img->vsize;

	VID_IMGSRC stVid_scl;
	if(!type)
	{
		stVid_scl.f_col = FOURCC_BGR24;
		stVid_scl.hsize = hsize;
		stVid_scl.vsize = vsize;
		if(!m_scale.ScaleVideo(pVid_img, &stVid_scl)) return FALSE;

		pVid_img = &stVid_scl;
		type = CV_8UC3;
	}

	Mat img(
		vsize, hsize,
		type,
		pVid_img->x_ptr, pVid_img->x_stride
	);

	std::vector<Face> faces;
	faces = m_pDetector->detect(img, m_face_size, m_scale_factor, m_thres_NMS);

	for(auto& i : faces)
	{
		CRect re = CRect(i.bbox.x1, i.bbox.y1, i.bbox.x2, i.bbox.y2);
		re_ary.push_back(re);
	}
	return TRUE;
}
