#ifndef _HELPER
#define _HELPER

#include<string>
#include<ctime>
#include<fstream>
#include<sstream>

namespace Helper
{
	class DateTime
	{ 
	private : 
		int day, month, year, hour, min, sec;

		std::string get_date_string(void) const
		{
			return std::to_string(year) + "-" + (month < 10 ? "0" : "") + std::to_string(month) +
				"-" + (day < 10 ? "0" : "") + std::to_string(day);
		}

		std::string get_time_string(const std::string &seperator = "-") const
		{
			return (hour < 10 ? "0" : "") + std::to_string(hour) + seperator +
				(min < 10 ? "0" : "") + std::to_string(min) + seperator +
				(sec < 10 ? "0" : "") + std::to_string(sec);
		}

	public :
		DateTime()
		{
			time_t ms;
			ms = time(NULL);

			struct tm *info = localtime(&ms);

			day = info->tm_mday;
			month = info->tm_mon + 1;
			year = info->tm_year + 1900;
			hour = info->tm_hour;
			min = info->tm_min;
			sec = info->tm_sec;
		}

		DateTime(int d, int mo, int y, int h, int mi, int s) 
			: day{ d }, month{ mo }, year{ y }, hour{ h }, min{ mi }, sec{ s } {}

		DateTime(int d, int mo, int y)
			: day{ d }, month{ mo }, year{ y }, hour{ 0 }, min{ 0 }, sec{ 0 } {}

		DateTime now(void) const
		{
			return DateTime{};
		}

		std::string get_date_time_string(const std::string &seperator = "-") const
		{
			return get_date_string() + " " + get_time_string(seperator);
		}
	};

	// ¼g¤J App log
	void write_app_log(const std::string &msg)
	{
		std::ofstream fout{ "AppLog.txt",std::ios_base::app };

		fout << '[' << DateTime{}.get_date_time_string() << ']' << '\n'
			<< msg << '\n' << std::endl;

		fout.close();
	}
}

#endif // !_HELPER

