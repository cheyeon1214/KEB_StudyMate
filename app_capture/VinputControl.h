#ifndef  _VINPUT_CONTROL_H_
#define  _VINPUT_CONTROL_H_

typedef void(*VInCallback)(void* Ptr, VID_IMGSRC* pVid_img);

class VinputControl
{
private:
	int m_index;
	int m_hsize, m_vsize;

	void* m_ptr;
	VInCallback m_cbVIn;

public:
	BOOL Init(int index, int hsize, int vsize);
	void Exit();
	void SetCallback(void* Ptr, VInCallback cb);

private:
	friend static void DevThread(void* Ptr);
	void DevThread();

	BOOL m_bDev;
	HANDLE m_hDevThread;
	HANDLE m_hDevThreadEnd;

public:
	VinputControl();
	virtual ~VinputControl();
};

#endif
