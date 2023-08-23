// cmn_dtime_exe.h
//

#pragma once

class cmn_dtime_exe
{
private:
	TZone m_mytzi;

private:
	BOOL _is_yoon(WORD wYear);
	BOOL _is_last_weekofmonth(WORD wYear, WORD wMonth, WORD wDay);
	int _get_weekofmonth(int nDay, int nDayOfWeek);
	BOOL _dtimetostm(const DTime * pdt, SYSTEMTIME * pstm);
	BOOL _is_dst(const SYSTEMTIME * pstm, const TIME_ZONE_INFORMATION * ptzi);
	BOOL _is_dst(const DTime * pdt, const TIME_ZONE_INFORMATION * ptzi);
	BOOL _is_dst_start_day(const SYSTEMTIME * pstm, const TIME_ZONE_INFORMATION * ptzi);
	BOOL _is_dst_start_day(const DTime *pdt, const TIME_ZONE_INFORMATION * ptzi);
	BOOL _is_dst_end_day(const SYSTEMTIME * pstm, const TIME_ZONE_INFORMATION * ptzi);
	BOOL _is_dst_end_day(const DTime *pdt, const TIME_ZONE_INFORMATION * ptzi);
	BOOL _dtimetostm(const DTime * pdt, const TIME_ZONE_INFORMATION * ptzi, SYSTEMTIME * pstm, BOOL &bApplyDst);
	BOOL _stmtodtime(const SYSTEMTIME * pstm, DTime * pdt);

public:
	void _GetTimeZone(TZone * ptzi);
	BOOL _IsMyTimeZone(TZone * ptzi);
	void _CopyTimeZone(TZone * pdst, TZone * psrc);
	BOOL _CompTimeZone(TZone * ptz1, TZone * ptz2);

	void _GetCurDTime(DTime * pdt);
	void _GetCurLTime(LTime * plt, BOOL bGetDst);
	void _GetCurSTime(WCHAR * pst, BOOL bMillisec, BOOL bDst);
	void _DTimetoLTime(const DTime * pdt, TZone * ptzi, LTime * plt);
	void _LTimetoDTime(const LTime * plt, TZone * ptzi, DTime * pdt);
	void _DTimetoSDate(const DTime * pdt, TZone * ptzi, WCHAR * psd);
	void _DTimetoSTime(const DTime * pdt, TZone * ptzi, WCHAR * pst, BOOL bMillisec, BOOL bDst);
	void _LTimetoSDate(const LTime * plt, WCHAR * psd);
	void _LTimetoSTime(const LTime * plt, WCHAR * pst, BOOL bMillisec, BOOL bDst);
	void _GetDayZone(const LTime * plt, TZone * ptzi, DTime * pdtDayStart, DTime * pdtDayEnd);

public:
	cmn_dtime_exe();
	~cmn_dtime_exe();
};
