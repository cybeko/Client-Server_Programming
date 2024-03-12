#include "ServerDlg.h"

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
	hEditRead = GetDlgItem(hwnd, IDC_EDIT_READ);
	hEditWrite = GetDlgItem(hwnd, IDC_EDIT_WRITE);
	hBtnStart = GetDlgItem(hwnd, IDC_BUTTON_START);
	hBtnSend = GetDlgItem(hwnd, IDC_BUTTON_SEND);
	hBtnEnd = GetDlgItem(hwnd, IDC_BUTTON_END);
	EnableWindow(hBtnSend, FALSE);
	EnableWindow(hBtnEnd, FALSE);

	return TRUE;
}

DWORD WINAPI Thread(LPVOID lp)
{
	SourceDlg* dlg = (SourceDlg*)lp;
	dlg->acceptSocket = accept(dlg->_socket, NULL, NULL);

	while (true)
	{
		char buf[STR_SIZE];
		int i = recv(dlg->acceptSocket, buf, STR_SIZE, 0);
		if (i == INVALID_SOCKET)
		{
			WSACleanup();
			closesocket(dlg->acceptSocket);
			closesocket(dlg->_socket);
			break;
		}
		buf[i] = '\0';
		WCHAR str[STR_SIZE];
		int size = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
		MultiByteToWideChar(CP_UTF8, 0, buf, -1, str, size);
		SetWindowText(dlg->hEditRead, str);
	}
	return 0;
}

void SourceDlg::StartServer()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	addr.sin_port = htons(20000);
	bind(_socket, (SOCKADDR*)&addr, sizeof(addr));
	listen(_socket, 1);

	hThread = CreateThread(NULL, 0, Thread, this, 0, NULL);
	EnableWindow(hBtnStart, FALSE);
	EnableWindow(hBtnSend, TRUE);
}

void SourceDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON_START) 
	{
		StartServer();
		EnableWindow(hBtnEnd, TRUE);
		SetWindowText(hwnd, L"Server Started");
	}
	else if (id == IDC_BUTTON_SEND) 
	{
		WCHAR wBuf[STR_SIZE];
		GetWindowText(hEditWrite, wBuf, sizeof(wBuf));
		if (lstrlen(wBuf))
		{
			int size = WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, NULL, 0, NULL, NULL);
			char buf[STR_SIZE];
			WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, buf, size, NULL, NULL);
			send(acceptSocket, buf, strlen(buf), 0);
		}
		else
		{
			MessageBox(hwnd, L"Empty field", L"error", MB_OK);
		}
	}
	else if (id == IDC_BUTTON_END)
	{
		TerminateThread(hThread, 0);
		WSACleanup();
		closesocket(acceptSocket);
		closesocket(_socket);
		SetWindowText(hwnd, L"Server Disabled");
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