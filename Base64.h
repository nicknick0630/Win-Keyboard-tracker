#ifndef _BASE64
#define _BASE64

#include<vector>
#include<string>

namespace Base64
{
	const std::string TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	const std::string ELEMENT_1 = "LM::TB::BB";
	const std::string ELEMENT_2 = "_:/_77";
	const std::string ELEMENT_3 = "line=wowC++";

	class Encryption
	{
	private :
		std::string base64_encode(const std::string &str)
		{
			//*     *      *      *
			//00110001 00110010 00110011

			std::string ans;
			const unsigned char PATTERN = 63;
			int val = 0;
			int bits = -6;		//	-6 -4 -2 

								//	 |	|  |
								//	 v  v  v

								//	 2  4  6  0

			for (const auto &ch : str)
			{
				val = (val << 8) + static_cast<int>(ch);
				bits += 8;

				while (bits >= 0)
				{
					ans.push_back(TABLE[ (val >> bits) & PATTERN ]);
					bits -= 6;
				}
			}

			if (bits > -6)
				ans.push_back(TABLE[(val << -bits) & PATTERN]);

			while (ans.size() % 4)
				ans.push_back('=');

			return ans;
		}

	public : 
		std::string encode(std::string str)
		{
			str = ELEMENT_1 + str + ELEMENT_2 + ELEMENT_3;
			str = base64_encode(str);
			str.insert(7, ELEMENT_3);
			str += ELEMENT_1;
			str = base64_encode(str);
			str = ELEMENT_2 + ELEMENT_3 + str + ELEMENT_1;
			str = base64_encode(str);
			str.insert(1, "L");
			str.insert(7, "M");

			return str;

			//return base64_encode(str);
		}
	};
}

#endif // !_BASE64
