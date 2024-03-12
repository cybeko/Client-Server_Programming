#include "ClientDlg.h"

SourceDlg* SourceDlg::ptr = NULL;
SourceDlg::SourceDlg(void)
{
	ptr = this;
}
void SourceDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL SourceDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hEditWrite = GetDlgItem(hwnd, IDC_EDIT_WRITE);
	hEditRead = GetDlgItem(hwnd, IDC_EDIT_READ);
	hBtnStart = GetDlgItem(hwnd, IDC_BUTTON_START);
	hBtnConnect = GetDlgItem(hwnd, IDC_BUTTON_CONNECT);
	hBtnEnd = GetDlgItem(hwnd, IDC_BUTTON_END);

	EnableWindow(hBtnEnd, FALSE);
	EnableWindow(hBtnStart, FALSE);
	return TRUE;
}
DWORD WINAPI Thread(LPVOID lp)
{
	SourceDlg* dlg = (SourceDlg*)lp;
	while (true)
	{
		char buf[STR_SIZE];
		int i = recv(dlg->_socket, buf, STR_SIZE, 0);
		if (i == INVALID_SOCKET)
		{
			WSACleanup();
			closesocket(dlg->acceptSocket);
			closesocket(dlg->_socket);
			//MessageBox(NULL, L"Connection lost", L"End of Session", MB_OK);
			break;
		}
		buf[i] = '\0';
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
		WCHAR str[STR_SIZE];
		MultiByteToWideChar(CP_UTF8, 0, buf, -1, str, requiredSize);
		SetWindowText(dlg->hEditRead, str);
	}
	return 0;
}

void SourceDlg::ConnectClient()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	addr.sin_port = htons(20000);
	connect(_socket, (SOCKADDR*)&addr, sizeof(addr));
	hThread = CreateThread(NULL, 0, Thread, this, 0, NULL);
	EnableWindow(hBtnConnect, FALSE);
	EnableWindow(hBtnEnd, TRUE);
	EnableWindow(hBtnStart, TRUE);
}
void SourceDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON_CONNECT)
	{
		ConnectClient();
		EnableWindow(hBtnEnd, TRUE);

		SetWindowText(hwnd, L"Connected");
	}
	else if (id == IDC_BUTTON_START)
	{
		WCHAR wBuf[STR_SIZE];
		GetWindowText(hEditWrite, wBuf, sizeof(wBuf));
		if (lstrlen(wBuf))
		{
			int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, NULL, 0, NULL, NULL);
			char buf[STR_SIZE];
			WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, buf, requiredSize, NULL, NULL);
			send(_socket, buf, strlen(buf), 0);
		}
		else
		{
			MessageBox(hwnd, L"Empty field", L"error", MB_OK);
		}
	}
	else if (id == IDC_BUTTON_END)
	{
		WSACleanup();
		closesocket(acceptSocket);
		closesocket(_socket);
		SetWindowText(hwnd, L"Disconnected");
		EnableWindow(hBtnEnd, FALSE);

	}
}

INT_PTR CALLBACK SourceDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}

void OpenConsole()
{
	AllocConsole();
	(void)freopen("conin$", "r", stdin);
	(void)freopen("conout$", "w", stdout);
	(void)freopen("conout$", "w", stderr);
}