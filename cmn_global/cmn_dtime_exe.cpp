// cmn_dtime_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "cmn_dtime_exe.h"

/*****************************************************************************
 *
 *  cmn_dtime
 *
 ****************************************************************************/
cmn_dtime_exe::cmn_dtime_exe()
{
	memset(&m_mytzi,0,sizeof(TZone));
	GetTimeZoneInformation(&m_mytzi);
}
cmn_dtime_exe::~cmn_dtime_exe()
{
}

/*****************************************************************************
 *
 *  DTime & SYSTEMTIME Resource
 *
 ****************************************************************************/

const uint32 _DTIME_WEEKDAYOFF = 6;

const uint32 _DTIME_MONTHDAY[2][13] = {{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365} , 
									   {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366} };

const uint32 _DTIME_TOTALDAYOFMONTH[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} , 
											  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };

BOOL cmn_dtime_exe::_is_yoon(WORD wYear)
{
	return (!(wYear % 400) ? TRUE : !(wYear % 100) ? FALSE : !(wYear % 4) ? TRUE : FALSE);
}

BOOL cmn_dtime_exe::_is_last_weekofmonth(WORD wYear, WORD wMonth, WORD wDay)
{
	uint32 uiYoon = _is_yoon(wYear);
	if(uiYoon)
		uiYoon = 1;
	uint32 uiLastDay = _DTIME_TOTALDAYOFMONTH[uiYoon][wMonth-1];
	
	if(uiLastDay >= (uint32)(wDay+7))
		return FALSE;
	
	return TRUE;
}

int cmn_dtime_exe::_get_weekofmonth(int nDay, int nDayOfWeek)
{
	int nWeekOfMonth = 0;

	if(nDay < nDayOfWeek)
		 nWeekOfMonth = ((nDay - nDayOfWeek) / 7);
	else nWeekOfMonth = ((nDay - nDayOfWeek) / 7) + 1;
	
	return nWeekOfMonth;
}

BOOL cmn_dtime_exe::_dtimetostm(const DTime * pdt, SYSTEMTIME * pstm)
{
	if(!pdt || !pstm)
		return FALSE;

	if(*pdt==0)
	{
		ZeroMemory(pstm, sizeof(SYSTEMTIME));
		return FALSE;
	}

	DTime dt = *pdt;

	uint32 uiInDay = (uint32)(dt % _DTIME_DAY);
	uint32 uiTDay = (uint32)(dt / _DTIME_DAY);

	//weekday
	pstm->wDayOfWeek = (WORD)((uiTDay + _DTIME_WEEKDAYOFF) % 7);

	uint32 uiYear = uiTDay / 365;
	uint32 uiYoon = (uiYear-1)/4 - (uiYear-1)/100 + (uiYear-1)/400;

	uiYear -= (uiYoon / 365);
	for(uint32 n = 1 ; uiTDay != 0 ; uiYear += n)
	{
		if(uiYear < 0) uiYear = 0;
		uiYoon = ((uiYear*365)+(uiYear-1)/4-(uiYear-1)/100+(uiYear-1)/400);
		if(uiTDay >= uiYoon) break;

		n = uiTDay < uiYoon ? -1 : 1;
	}

	pstm->wYear = (WORD)uiYear;
	uiTDay -= ((uiYear*365)+(uiYear-1)/4-(uiYear-1)/100+(uiYear-1)/400);

	uiYoon = (pstm->wYear > 0 && _is_yoon(pstm->wYear)) ? 1 : 0;

	if(uiTDay == 0)
	{
		pstm->wYear -= 1;
		uiTDay = 365 + uiYoon;
	}

	pstm->wMonth = 0;
	//find month
	if ( uiTDay > 0 )
	{
		//5월보다 작은가?
		if(_DTIME_MONTHDAY[uiYoon][4] >= uiTDay)
		{
			//1월?
			if(_DTIME_MONTHDAY[uiYoon][1] >= uiTDay)
			{
				pstm->wMonth = 1;
			}
			//2월?
			else if(_DTIME_MONTHDAY[uiYoon][2] >= uiTDay)
			{
				pstm->wMonth = 2;
			}
			//3월?
			else if(_DTIME_MONTHDAY[uiYoon][3] >= uiTDay)
			{
				pstm->wMonth = 3;
			}
			//4월
			else
			{
				pstm->wMonth = 4;
			}
		}
		//9월 보다 작은가?
		else if(_DTIME_MONTHDAY[uiYoon][8] >= uiTDay)
		{
			//5월?
			if(_DTIME_MONTHDAY[uiYoon][5] >= uiTDay)
			{
				pstm->wMonth = 5;
			}
			//6월?
			else if(_DTIME_MONTHDAY[uiYoon][6] >= uiTDay)
			{
				pstm->wMonth = 6;
			}
			//7월?
			else if(_DTIME_MONTHDAY[uiYoon][7] >= uiTDay)
			{
				pstm->wMonth = 7;
			}
			//8월
			else
			{
				pstm->wMonth = 8;
			}
		}
		//13월보다 작은가?
		else
		{
			//9월?
			if(_DTIME_MONTHDAY[uiYoon][9] >= uiTDay)
			{
				pstm->wMonth = 9;
			}
			//10월?
			else if(_DTIME_MONTHDAY[uiYoon][10] >= uiTDay)
			{
				pstm->wMonth = 10;
			}
			//11월?
			else if(_DTIME_MONTHDAY[uiYoon][11] >= uiTDay)
			{
				pstm->wMonth = 11;
			}
			//12월
			else
			{
				pstm->wMonth = 12;
			}
		}
	}

	//monthday
	uiTDay -= _DTIME_MONTHDAY[uiYoon][pstm->wMonth-1];
	pstm->wDay = (WORD)uiTDay;

	//milliseconds
	pstm->wMilliseconds = (WORD)(uiInDay % 1000);
	uiInDay /= 1000;
	//second
	pstm->wSecond = (WORD)(uiInDay % 60);
	uiInDay /= 60;
	//minute
	pstm->wMinute = (WORD)(uiInDay % 60);
	uiInDay /= 60;
	//hour
	pstm->wHour = (WORD)(uiInDay % 24);
	uiInDay /= 24;

	return TRUE;
}

BOOL cmn_dtime_exe::_is_dst(const SYSTEMTIME * pstm, const TIME_ZONE_INFORMATION * ptzi)
{
	if(!pstm || !ptzi)
		return FALSE;

	if(ptzi->DaylightBias && ptzi->DaylightDate.wMonth && ptzi->StandardDate.wMonth)
	{
		if(ptzi->StandardDate.wMonth > ptzi->DaylightDate.wMonth)
		{
			//북반구
			if(pstm->wMonth > ptzi->DaylightDate.wMonth && pstm->wMonth < ptzi->StandardDate.wMonth)
			{
				return TRUE;
			}
			if(pstm->wMonth == ptzi->DaylightDate.wMonth)
			{
				//시작 달
				if(ptzi->DaylightDate.wDay == 5)
				{
					if(_is_last_weekofmonth(pstm->wYear, pstm->wMonth, pstm->wDay))
					{
						if(pstm->wDayOfWeek == ptzi->DaylightDate.wDayOfWeek)
						{
							if(pstm->wHour >= ptzi->DaylightDate.wHour)
							{
								return TRUE;
							}
						}
						else
						{
							uint32 uiSunday = (uint32)(pstm->wDay + ( 7 - pstm->wDayOfWeek ));
							uint32 uiYoon = _is_yoon(pstm->wYear) ? 1 : 0;
							if(uiSunday > _DTIME_TOTALDAYOFMONTH[uiYoon][pstm->wMonth-1])
								return TRUE;
						}
					}
				}
				else
				{
					int nWeekOfMonth = _get_weekofmonth(pstm->wDay, pstm->wDayOfWeek);
					if(nWeekOfMonth > ptzi->DaylightDate.wDay)
						return TRUE;

					if(nWeekOfMonth == ptzi->DaylightDate.wDay)
					{
						if(pstm->wDayOfWeek > ptzi->DaylightDate.wDayOfWeek)
							return TRUE;
						if(pstm->wDayOfWeek == ptzi->DaylightDate.wDayOfWeek)
						{
							if(pstm->wHour >= ptzi->DaylightDate.wHour)
								return TRUE;
						}
					}
				}
			}
			else if(pstm->wMonth == ptzi->StandardDate.wMonth)
			{
				//끝나는 달
				if(ptzi->StandardDate.wDay == 5) 
				{
					if(_is_last_weekofmonth(pstm->wYear, pstm->wMonth, pstm->wDay))
					{
						if(pstm->wDayOfWeek == ptzi->StandardDate.wDayOfWeek)
						{
							if(pstm->wHour < ptzi->StandardDate.wHour)
								return TRUE;
						}
						else
						{
							uint32 uiSunday = (uint32)(pstm->wDay + ( 7 - pstm->wDayOfWeek ));
							uint32 uiYoon = _is_yoon(pstm->wYear) ? 1 : 0;
							if(uiSunday < _DTIME_TOTALDAYOFMONTH[uiYoon][pstm->wMonth-1])
								return TRUE;
						}
					}
					else
						return TRUE;
				}
                else
				{
                    int nWeekOfMonth = _get_weekofmonth(pstm->wDay, pstm->wDayOfWeek);
					if(nWeekOfMonth < ptzi->StandardDate.wDay)
						return TRUE;

					if(nWeekOfMonth == ptzi->StandardDate.wDay)
					{
						if(pstm->wDayOfWeek < ptzi->StandardDate.wDayOfWeek)
							return TRUE;

						if(pstm->wDayOfWeek == ptzi->StandardDate.wDayOfWeek)
						{
							if(pstm->wHour < ptzi->StandardDate.wHour)
								return TRUE;
						}
					}
				}
			}
		}
		else
		{
			//남반구
			if((pstm->wMonth>0 && pstm->wMonth < ptzi->StandardDate.wMonth) || pstm->wMonth > ptzi->DaylightDate.wMonth)
			{
				return TRUE;
			}
			if(pstm->wMonth == ptzi->DaylightDate.wMonth)
			{
				//시작달
				if(ptzi->DaylightDate.wDay == 5) 
				{
					if(_is_last_weekofmonth(pstm->wYear, pstm->wMonth, pstm->wDay))
					{
						if(pstm->wDayOfWeek == ptzi->DaylightDate.wDayOfWeek)
						{
							if(pstm->wHour >= ptzi->DaylightDate.wHour)
							{
								return TRUE;
							}
						}
						else
						{
							uint32 uiSunday = (uint32)(pstm->wDay + ( 7 - pstm->wDayOfWeek ));
							uint32 uiYoon = _is_yoon(pstm->wYear) ? 1 : 0;
							if(uiSunday > _DTIME_TOTALDAYOFMONTH[uiYoon][pstm->wMonth-1])
								return TRUE;
						}
					}
				}
				else
				{
					int nWeekOfMonth = _get_weekofmonth(pstm->wDay, pstm->wDayOfWeek);
					if(nWeekOfMonth == ptzi->DaylightDate.wDay)
					{
						if(pstm->wDayOfWeek>ptzi->DaylightDate.wDayOfWeek)
							return TRUE;
						if(pstm->wDayOfWeek==ptzi->DaylightDate.wDayOfWeek)
						{
							if(pstm->wHour >= ptzi->DaylightDate.wHour)
								return TRUE;
						}
					}
					
					if(nWeekOfMonth > ptzi->DaylightDate.wDay)
					{
						if(pstm->wDayOfWeek > ptzi->DaylightDate.wDayOfWeek)
							return TRUE;
						
						if(pstm->wDayOfWeek==ptzi->DaylightDate.wDayOfWeek)
						{
							if(pstm->wHour >= ptzi->DaylightDate.wHour)
								return TRUE;
						}
					}
				}
			}
			else if(pstm->wMonth == ptzi->StandardDate.wMonth)
			{
				//끝나는 달
				if(ptzi->StandardDate.wDay == 5) 
				{
					if(_is_last_weekofmonth(pstm->wYear, pstm->wMonth, pstm->wDay))
					{
						if(pstm->wDayOfWeek == ptzi->StandardDate.wDayOfWeek)
						{
							if(pstm->wHour < ptzi->StandardDate.wHour)
								return TRUE;
						}
						else
						{
							uint32 uiSunday = (uint32)(pstm->wDay + ( 7 - pstm->wDayOfWeek ));
							uint32 uiYoon = _is_yoon(pstm->wYear) ? 1 : 0;
							if(uiSunday < _DTIME_TOTALDAYOFMONTH[uiYoon][pstm->wMonth-1])
								return TRUE;
						}
					}
					else
						return TRUE;
				}
				else
				{
					int nWeekOfMonth = _get_weekofmonth(pstm->wDay, pstm->wDayOfWeek);
					if(nWeekOfMonth < ptzi->StandardDate.wDay)
						return TRUE;

					if(nWeekOfMonth == ptzi->StandardDate.wDay)
					{
						if(pstm->wDayOfWeek < ptzi->StandardDate.wDayOfWeek)
							return TRUE;

						if(pstm->wDayOfWeek==ptzi->StandardDate.wDayOfWeek)
						{
							if(pstm->wHour < ptzi->StandardDate.wHour)
								return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL cmn_dtime_exe::_is_dst(const DTime * pdt, const TIME_ZONE_INFORMATION * ptzi)
{
	if(!pdt || !ptzi)
		return FALSE;

	SYSTEMTIME stm;
	if(!_dtimetostm(pdt, &stm))
		return FALSE;
	return _is_dst(&stm, ptzi);
}

BOOL cmn_dtime_exe::_is_dst_start_day(const SYSTEMTIME * pstm, const TIME_ZONE_INFORMATION * ptzi)
{
	if(!ptzi || !pstm)
		return FALSE;

	if(ptzi->DaylightBias && ptzi->DaylightDate.wMonth && ptzi->StandardDate.wMonth)
	{
		if(pstm->wMonth == ptzi->DaylightDate.wMonth)
		{
			if(pstm->wDayOfWeek==ptzi->DaylightDate.wDayOfWeek)
			{
				if(ptzi->DaylightDate.wDay == 5)
				{
					if(_is_last_weekofmonth(pstm->wYear, pstm->wMonth, pstm->wDay))
						return TRUE;
				}
				else
				{
					int nWeekOfMonth = _get_weekofmonth(pstm->wDay, pstm->wDayOfWeek);
					if((pstm->wDayOfWeek == ptzi->DaylightDate.wDayOfWeek) && 
						(nWeekOfMonth == ptzi->DaylightDate.wDay))
					{
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
BOOL cmn_dtime_exe::_is_dst_start_day(const DTime *pdt, const TIME_ZONE_INFORMATION * ptzi)
{
	if(!pdt || !ptzi)
		return FALSE;

	SYSTEMTIME stm;
	if(!_dtimetostm(pdt, &stm))
		return FALSE;
	return _is_dst_start_day(&stm, ptzi);
}

BOOL cmn_dtime_exe::_is_dst_end_day(const SYSTEMTIME * pstm, const TIME_ZONE_INFORMATION * ptzi)
{
	if(!ptzi || !pstm)
		return FALSE;

	if(ptzi->DaylightBias && ptzi->DaylightDate.wMonth && ptzi->StandardDate.wMonth)
	{
		if(pstm->wMonth == ptzi->StandardDate.wMonth)
		{
			if(pstm->wDayOfWeek == ptzi->StandardDate.wDayOfWeek)
			{
				if(ptzi->StandardDate.wDay == 5) 
				{
					if( _is_last_weekofmonth(pstm->wYear, pstm->wMonth, pstm->wDay) )
						return TRUE;
				}
				else
				{
					int nWeekOfMonth = _get_weekofmonth(pstm->wDay, pstm->wDayOfWeek);
					if((pstm->wDayOfWeek == ptzi->StandardDate.wDayOfWeek) && 
						(nWeekOfMonth == ptzi->StandardDate.wDay))
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}
BOOL cmn_dtime_exe::_is_dst_end_day(const DTime *pdt, const TIME_ZONE_INFORMATION * ptzi)
{
	if(!pdt || !ptzi)
		return FALSE;

	SYSTEMTIME stm;
	if(!_dtimetostm(pdt, &stm))
		return FALSE;
	return _is_dst_end_day(&stm, ptzi);
}

BOOL cmn_dtime_exe::_dtimetostm(const DTime * pdt, const TIME_ZONE_INFORMATION * ptzi, SYSTEMTIME * pstm, BOOL &bApplyDst)
{
	if(!pdt || !ptzi || !pstm)
		return FALSE;

	DTime dt = *pdt;
	dt -= ptzi->Bias * _DTIME_MINUTE;

	if(!_dtimetostm(&dt, pstm))
		return FALSE;

	bApplyDst = FALSE;
	if(_is_dst_end_day(pstm, ptzi))
	{
		DTime offset = 0;
		offset -= ptzi->DaylightBias;
		offset *= _DTIME_MINUTE;
		dt += offset;
		if(!_is_dst(&dt, ptzi))
		{
			dt += ptzi->DaylightBias * _DTIME_MINUTE;
        }
		else
		{
			bApplyDst = TRUE;
		}

		if(!_dtimetostm(&dt, pstm))
			return FALSE;
	}
	else if(_is_dst(pstm, ptzi))
	{
		DTime offset = 0;
		offset -= ptzi->DaylightBias;
		offset *= _DTIME_MINUTE;
		dt += offset;

		bApplyDst = TRUE;

		if(!_dtimetostm(&dt, pstm))
			return FALSE;
	}

	return TRUE;
}

/*
typedef signed __int64 DTime;

const DTime _DTIME_SECOND	= 1000;
const DTime _DTIME_MINUTE	= 60 * 1000;
const DTime _DTIME_HOUR		= 60 * 60 * 1000;
const DTime _DTIME_DAY		= (DTime)24 * 60 * 60 * 1000;
*/
BOOL cmn_dtime_exe::_stmtodtime(const SYSTEMTIME * pstm, DTime * pdt)
{
	if(!pdt || !pstm)
		return FALSE;

	uint32 uiYear = (uint32)(pstm->wYear - 1);
	uint32 uiTDay = (uint32)pstm->wYear * 365 + uiYear / 4 - uiYear / 100 + uiYear / 400;

	if ( pstm->wYear > 0 || pstm->wMonth > 0)
	{
		if(_is_yoon(pstm->wYear))
			uiTDay += _DTIME_MONTHDAY[1][pstm->wMonth - 1];
		else
			uiTDay += _DTIME_MONTHDAY[0][pstm->wMonth - 1];
	}

	uiTDay += (uint32)pstm->wDay;

	*pdt = (DTime)uiTDay * _DTIME_DAY;
	*pdt += _DTIME_HOUR * pstm->wHour + 
			_DTIME_MINUTE * pstm->wMinute + 
			_DTIME_SECOND * pstm->wSecond + 
			pstm->wMilliseconds;

	return TRUE;
}

/*****************************************************************************
 *
 *  DTime & LTime User Interface
 *
 ****************************************************************************/

void cmn_dtime_exe::_GetTimeZone(TZone * ptzi)
{
	if(!ptzi) return;

	memcpy(ptzi, &m_mytzi, sizeof(TZone));
}
BOOL cmn_dtime_exe::_IsMyTimeZone(TZone * ptzi)
{
	if(!ptzi) return FALSE;

	if(m_mytzi.Bias != ptzi->Bias)
		return FALSE;
	if(m_mytzi.StandardDate.wMonth != ptzi->StandardDate.wMonth)
		return FALSE;
	if(m_mytzi.StandardDate.wMonth != 0)
	{
		if(m_mytzi.StandardDate.wDayOfWeek != ptzi->StandardDate.wDayOfWeek)
			return FALSE;
		if(m_mytzi.StandardDate.wDay != ptzi->StandardDate.wDay)
			return FALSE;
		if(m_mytzi.StandardDate.wHour != ptzi->StandardDate.wHour)
			return FALSE;
		if(m_mytzi.StandardBias != ptzi->StandardBias)
			return FALSE;
	}
	if(m_mytzi.DaylightDate.wMonth != ptzi->DaylightDate.wMonth)
		return FALSE;
	if(m_mytzi.DaylightDate.wMonth != 0)
	{
		if(m_mytzi.DaylightDate.wDayOfWeek != ptzi->DaylightDate.wDayOfWeek)
			return FALSE;
		if(m_mytzi.DaylightDate.wDay != ptzi->DaylightDate.wDay)
			return FALSE;
		if(m_mytzi.DaylightDate.wHour != ptzi->DaylightDate.wHour)
			return FALSE;
		if(m_mytzi.DaylightBias != ptzi->DaylightBias)
			return FALSE;
	}

	return TRUE;
}
void cmn_dtime_exe::_CopyTimeZone(TZone * pdst, TZone * psrc)
{
	if(!pdst || !psrc) return;

	memcpy(pdst, psrc, sizeof(TZone));
}
BOOL cmn_dtime_exe::_CompTimeZone(TZone * ptz1, TZone * ptz2)
{
	if(!ptz1 || !ptz2) return FALSE;

	if(ptz1->Bias != ptz2->Bias)
		return FALSE;
	if(ptz1->StandardDate.wMonth != ptz2->StandardDate.wMonth)
		return FALSE;
	if(ptz1->StandardDate.wMonth != 0)
	{
		if(ptz1->StandardDate.wDayOfWeek != ptz2->StandardDate.wDayOfWeek)
			return FALSE;
		if(ptz1->StandardDate.wDay != ptz2->StandardDate.wDay)
			return FALSE;
		if(ptz1->StandardDate.wHour != ptz2->StandardDate.wHour)
			return FALSE;
		if(ptz1->StandardBias != ptz2->StandardBias)
			return FALSE;
	}
	if(ptz1->DaylightDate.wMonth != ptz2->DaylightDate.wMonth)
		return FALSE;
	if(ptz1->DaylightDate.wMonth != 0)
	{
		if(ptz1->DaylightDate.wDayOfWeek != ptz2->DaylightDate.wDayOfWeek)
			return FALSE;
		if(ptz1->DaylightDate.wDay != ptz2->DaylightDate.wDay)
			return FALSE;
		if(ptz1->DaylightDate.wHour != ptz2->DaylightDate.wHour)
			return FALSE;
		if(ptz1->DaylightBias != ptz2->DaylightBias)
			return FALSE;
	}

	return TRUE;
}

void cmn_dtime_exe::_GetCurDTime(DTime * pdt)
{
	if(!pdt) return;

	SYSTEMTIME stTime;
	GetSystemTime(&stTime);
	if(!_stmtodtime(&stTime, pdt))
		*pdt = 0;
}
void cmn_dtime_exe::_GetCurLTime(LTime * plt, BOOL bGetDst)
{
	if(!plt) return;

	if(bGetDst)
	{
		DTime dtTime = 0;
		_GetCurDTime(&dtTime);
		_DTimetoLTime(&dtTime, NULL, plt);
	}
	else
	{
		plt->bApplyDst = FALSE;
		GetLocalTime(&(plt->stm));
	}
}
void cmn_dtime_exe::_GetCurSTime(WCHAR * pst, BOOL bMillisec, BOOL bDst)
{
	if(bDst)
	{
		LTime lt;
		DTime dtTime = 0;
		_GetCurDTime(&dtTime);
		_DTimetoLTime(&dtTime, NULL, &lt);

		swprintf_s(pst, 64, L"%04d-%02d-%02d %02d:%02d:%02d",
			(int)lt.stm.wYear, (int)lt.stm.wMonth, (int)lt.stm.wDay,
			(int)lt.stm.wHour, (int)lt.stm.wMinute, (int)lt.stm.wSecond
			);
		if(bMillisec)
		{
			swprintf_s(pst, 64, L"%s:%03d",
				pst,
				(int)lt.stm.wMilliseconds
				);
		}
		if(lt.bApplyDst)
		{
			swprintf_s(pst, 64, L"%s DST",
				pst
				);
		}
	}
	else
	{
		LTime lt;
		lt.bApplyDst = FALSE;
		GetLocalTime(&(lt.stm));

		swprintf_s(pst, 64, L"%04d-%02d-%02d %02d:%02d:%02d",
			(int)lt.stm.wYear, (int)lt.stm.wMonth, (int)lt.stm.wDay,
			(int)lt.stm.wHour, (int)lt.stm.wMinute, (int)lt.stm.wSecond
			);
		if(bMillisec)
		{
			swprintf_s(pst, 64, L"%s:%03d",
				pst,
				(int)lt.stm.wMilliseconds
				);
		}
	}
}
void cmn_dtime_exe::_DTimetoLTime(const DTime * pdt, TZone * ptzi, LTime * plt)
{
	if(!pdt || !plt)
		return;
	if(!ptzi){
		ptzi = &m_mytzi;
	}

	if(!_dtimetostm(pdt, ptzi, &(plt->stm), plt->bApplyDst))
		memset(plt, 0, sizeof(LTime));
}
void cmn_dtime_exe::_LTimetoDTime(const LTime * plt, TZone * ptzi, DTime * pdt)
{
	if(!plt || !pdt)
		return;
	if(!ptzi){
		ptzi = &m_mytzi;
	}

	if(!_stmtodtime(&(plt->stm), pdt)){
		*pdt = 0;
		return;
	}

	*pdt += (ptzi->Bias * _DTIME_MINUTE);
	if(plt->bApplyDst)
	{
		*pdt += ptzi->DaylightBias * _DTIME_MINUTE;
	}
}
void cmn_dtime_exe::_DTimetoSDate(const DTime * pdt, TZone * ptzi, WCHAR * psd)
{
	if(!pdt || !psd)
		return;
	if(!ptzi){
		ptzi = &m_mytzi;
	}

	LTime lt;
	if(!_dtimetostm(pdt, ptzi, &(lt.stm), lt.bApplyDst))
		*psd = L'\0';
	else
	{
		swprintf_s(psd, 64, L"%04d-%02d-%02d",
			(int)lt.stm.wYear, (int)lt.stm.wMonth, (int)lt.stm.wDay
			);
	}
}
void cmn_dtime_exe::_DTimetoSTime(const DTime * pdt, TZone * ptzi, WCHAR * pst, BOOL bMillisec, BOOL bDst)
{
	if(!pdt || !pst)
		return;
	if(!ptzi){
		ptzi = &m_mytzi;
	}

	LTime lt;
	if(!_dtimetostm(pdt, ptzi, &(lt.stm), lt.bApplyDst))
		*pst = L'\0';
	else
	{
		swprintf_s(pst, 64, L"%04d-%02d-%02d %02d:%02d:%02d",
			(int)lt.stm.wYear, (int)lt.stm.wMonth, (int)lt.stm.wDay,
			(int)lt.stm.wHour, (int)lt.stm.wMinute, (int)lt.stm.wSecond
			);
		if(bMillisec)
		{
			swprintf_s(pst, 64, L"%s:%03d",
				pst,
				(int)lt.stm.wMilliseconds
				);
		}
		if(bDst && lt.bApplyDst)
		{
			swprintf_s(pst, 64, L"%s DST",
				pst
				);
		}
	}
}
void cmn_dtime_exe::_LTimetoSDate(const LTime * plt, WCHAR * psd)
{
	if(!plt || !psd)
		return;

	swprintf_s(psd, 64, L"%04d-%02d-%02d",
		(int)plt->stm.wYear, (int)plt->stm.wMonth, (int)plt->stm.wDay
		);
}
void cmn_dtime_exe::_LTimetoSTime(const LTime * plt, WCHAR * pst, BOOL bMillisec, BOOL bDst)
{
	if(!plt || !pst)
		return;

	swprintf_s(pst, 64, L"%04d-%02d-%02d %02d:%02d:%02d",
		(int)plt->stm.wYear, (int)plt->stm.wMonth, (int)plt->stm.wDay,
		(int)plt->stm.wHour, (int)plt->stm.wMinute, (int)plt->stm.wSecond
		);
	if(bMillisec)
	{
		swprintf_s(pst, 64, L"%s:%03d",
			pst,
			(int)plt->stm.wMilliseconds
			);
	}
	if(bDst && plt->bApplyDst)
	{
		swprintf_s(pst, 64, L"%s DST",
			pst
			);
	}
}
void cmn_dtime_exe::_GetDayZone(const LTime * plt, TZone * ptzi, DTime * pdtDayStart, DTime * pdtDayEnd)
{
	if(!plt || !pdtDayStart || !pdtDayEnd)
		return;
	if(!ptzi){
		ptzi = &m_mytzi;
	}

	SYSTEMTIME ltStart, ltEnd;
	memcpy(&ltStart, &(plt->stm), sizeof(SYSTEMTIME));
	memcpy(&ltEnd, &(plt->stm), sizeof(SYSTEMTIME));
	ltStart.wHour = 0;
	ltStart.wMinute = 0;
	ltStart.wSecond = 0;
	ltStart.wMilliseconds = 0;
	ltEnd.wHour = 23;
	ltEnd.wMinute = 59;
	ltEnd.wSecond = 59;
	ltEnd.wMilliseconds = 999;

	// for WIN2000
	_stmtodtime(&ltStart, pdtDayStart);
	*pdtDayStart += (ptzi->Bias * _DTIME_MINUTE);
	if(_is_dst(&ltStart, ptzi)) *pdtDayStart += (ptzi->DaylightBias * _DTIME_MINUTE);

	_stmtodtime(&ltEnd, pdtDayEnd);
	*pdtDayEnd += (ptzi->Bias * _DTIME_MINUTE);
	if(_is_dst(&ltEnd, ptzi)) *pdtDayEnd += (ptzi->DaylightBias * _DTIME_MINUTE);
}
/*****************************************************************************
	 LTime localtime;
	 localtime.stm.wYear = 2007;
	 localtime.stm.wMonth = 12;
	 localtime.stm.wDay = 13;
	
	 DTime dtDayStart, dtDayEnd;
	 _cmn_dtime_GetDayZone(&localtime, NULL, &dtDayStart, &dtDayEnd);
	 for(DTime i=dtDayStart; i<=dtDayEnd; i+=_DTIME_HOUR)
	 {
		_cmn_dtime_DTimetoLTime(&i, NULL, &localtime);
	 }
 ****************************************************************************/

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

cmn_dtime_exe g_cmn_dtime;

void _cmn_GetTZone(TZone * ptzi)
{
	g_cmn_dtime._GetTimeZone(ptzi);
}
BOOL _cmn_IsMyTZone(TZone * ptzi)
{
	return g_cmn_dtime._IsMyTimeZone(ptzi);
}
void _cmn_CopyTZone(TZone * pdst, TZone * psrc)
{
	g_cmn_dtime._CopyTimeZone(pdst, psrc);
}
BOOL _cmn_CompTZone(TZone * ptz1, TZone * ptz2)
{
	return g_cmn_dtime._CompTimeZone(ptz1, ptz2);
}

DTime _Get_DTime()
{
	DTime dt;
	g_cmn_dtime._GetCurDTime(&dt); return dt;
}
LTime _Get_LTime(BOOL bGetDst)
{
	LTime lt;
	g_cmn_dtime._GetCurLTime(&lt, bGetDst); return lt;
}
void _cmn_dtime_GetCurDTime(DTime * pdt)
{
	g_cmn_dtime._GetCurDTime(pdt);
}
void _cmn_dtime_GetCurLTime(LTime * plt, BOOL bGetDst)
{
	g_cmn_dtime._GetCurLTime(plt, bGetDst);
}
void _cmn_dtime_GetCurSTime(WCHAR * pst, BOOL bMillisec, BOOL bDst)
{
	g_cmn_dtime._GetCurSTime(pst, bMillisec, bDst);
}
void _cmn_dtime_DTimetoLTime(const DTime * pdt, TZone * ptzi, LTime * plt)
{
	g_cmn_dtime._DTimetoLTime(pdt, ptzi, plt);
}
void _cmn_dtime_LTimetoDTime(const LTime * plt, TZone * ptzi, DTime * pdt)
{
	g_cmn_dtime._LTimetoDTime(plt, ptzi, pdt);
}
void _cmn_dtime_DTimetoSDate(const DTime * pdt, TZone * ptzi, WCHAR * psd)
{
	g_cmn_dtime._DTimetoSDate(pdt, ptzi, psd);
}
void _cmn_dtime_DTimetoSTime(const DTime * pdt, TZone * ptzi, WCHAR * pst, BOOL bMillisec, BOOL bDst)
{
	g_cmn_dtime._DTimetoSTime(pdt, ptzi, pst, bMillisec, bDst);
}
void _cmn_dtime_LTimetoSDate(const LTime * plt, WCHAR * psd)
{
	g_cmn_dtime._LTimetoSDate(plt, psd);
}
void _cmn_dtime_LTimetoSTime(const LTime * plt, WCHAR * pst, BOOL bMillisec, BOOL bDst)
{
	g_cmn_dtime._LTimetoSTime(plt, pst, bMillisec, bDst);
}
void _cmn_dtime_GetDayZone(const LTime * plt, TZone * ptzi, DTime * pdtDayStart, DTime * pdtDayEnd)
{
	g_cmn_dtime._GetDayZone(plt, ptzi, pdtDayStart, pdtDayEnd);
}
