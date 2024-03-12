#pragma once
#include "Header.h"
void OpenConsole();
static const int MAXSTRLEN = 256;

class SourceDlg
{
public:
	SourceDlg(void);
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	static SourceDlg* ptr;

	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void Cls_OnClose(HWND hwnd);
	void ConnectClient();

	void DisconnectClient();

	SOCKET _socket;
	SOCKET acceptSocket;
	sockaddr_in addr;
	WSADATA wsaData;
	HWND hDialog, hEditRead, hEditWrite, hBtnStart, hBtnEnd, hBtnConnect;
	HANDLE hThread;
};