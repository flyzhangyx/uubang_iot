#include "../head/SERVER.h"
int CmpDate(int year, int month, int day)
{
	time_t now_time = 0, set_time = 0;
	struct tm p;
	p.tm_year = year-1900;
	p.tm_mon = month-1;
	p.tm_mday = day;
	p.tm_hour = 0;
	p.tm_min = 0;
	p.tm_sec = 0;
	p.tm_isdst = 0;
	set_time = mktime(&p);
	time(&now_time);
	return now_time - set_time;
}
