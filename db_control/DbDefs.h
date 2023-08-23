#ifndef _DB_DEFS_H_
#define _DB_DEFS_H_

#ifdef _WIN64
#pragma comment(lib, "../3rdparty/sqlite3/x64/lib/sqlite3.lib")
#else
#pragma comment(lib, "../3rdparty/sqlite3/x86/lib/sqlite3.lib")
#endif

enum
{
	lst_none = 0,
	lst_count,
	lst_search,
};
struct _ArgList
{
	void * ptr;
	uint n_val;
	void * p_val[2];
	
	_ArgList(void* _p, uint _nv, void* _pv1, void* _pv2 = NULL)
	{
		this->ptr = _p;
		this->n_val = _nv;
		this->p_val[0] = _pv1;
		this->p_val[1] = _pv2;
	}
};

#define IDX_INS 0
#define IDX_SCH 1

#endif // _DB_DEFS_H_