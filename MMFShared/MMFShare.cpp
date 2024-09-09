#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <commctrl.h>


static HANDLE g_hFileMap = nullptr;
void CreateFileMappingAndView(HWND hwnd)
{
	g_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL,
		PAGE_READWRITE, 0, 4 * 1024, __TEXT("MMFSharedData"));
	if (g_hFileMap != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			MessageBox(hwnd, __TEXT("Mapping already exists - ")
				__TEXT("not created."), NULL, MB_OK);
			CloseHandle(g_hFileMap);
		}
		else
		{
			LPVOID lpView = MapViewOfFile(g_hFileMap,
				FILE_MAP_READ | FILE_MAP_WRITE,
				0, 0, 4 * 1024);
			if ((BYTE*)lpView != nullptr)
			{
				UnmapViewOfFile((LPVOID)lpView);
				Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE),
					FALSE);
				Button_Enable(GetDlgItem(hwnd, IDC_READ),
					TRUE);
				Button_Enable(GetDlgItem(hwnd, IDC_WRITE),
					TRUE);
				Button_Enable(GetDlgItem(hwnd, IDC_CLOSEFILE),
					TRUE);
			}
			else
			{
				MessageBox(hwnd,
					TEXT("Can't map view of file."),
					NULL,
					MB_OK);
			}
		}
	}
	else
	{
		MessageBox(hwnd,
			__TEXT("Can't create file mapping."),
			NULL,
			MB_OK);
	}
}
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MMFSHARE));
	SetClassLong(hwnd, GCL_HICON, (LONG)hIcon);
	HFONT hFont = CreateFont(
		20,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		TEXT("Times New Roman"));
	HWND hEditWnd = GetDlgItem(
		hwnd,
		IDC_DATA);
	Edit_SetText(hEditWnd, __TEXT("Здесь вводите данные."));
	SendMessage(hEditWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	HWND hTextWnd = GetDlgItem(hwnd, IDC_STATIC);
	SendMessage(hTextWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	HWND hCloseButton = GetDlgItem(hwnd, IDC_CLOSEFILE);
	Button_Enable(hCloseButton, FALSE);
	SendMessage(hCloseButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	HWND hCreateButton = GetDlgItem(hwnd, IDC_CREATEFILE);
	SendMessage(hCreateButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	HWND hReadButton = GetDlgItem(hwnd, IDC_READ);
	SendMessage(hReadButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	HWND hWriteButton = GetDlgItem(hwnd, IDC_WRITE);
	SendMessage(hWriteButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	return (TRUE);
}
void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HANDLE hFileMapT;
	switch (id)
	{
	case IDC_CREATEFILE:
	{
		if (codeNotify != BN_CLICKED)
			break;
		CreateFileMappingAndView(hwnd);
	}
	break;
	case IDC_CLOSEFILE:
	{
		if (codeNotify != BN_CLICKED)
			break;
		if (CloseHandle(g_hFileMap))
		{
			Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE), TRUE);
			Button_Enable(GetDlgItem(hwnd, IDC_WRITE), FALSE);
			Button_Enable(hwndCtl, FALSE);
		}
	}
	break;
	case IDC_READ:
	{
		if (codeNotify != BN_CLICKED)
			break;
		hFileMapT = OpenFileMapping(
			FILE_MAP_READ | FILE_MAP_WRITE,
			FALSE,
			__TEXT("MMFSharedData"));
		if (hFileMapT != NULL)
		{
			LPVOID lpView = MapViewOfFile(hFileMapT,
				FILE_MAP_READ | FILE_MAP_WRITE,
				0, 0, 0);
			if ((BYTE*)lpView != NULL)
			{
				Edit_SetText(GetDlgItem(hwnd, IDC_DATA),
					(LPTSTR)lpView);
				UnmapViewOfFile((LPVOID)lpView);
			}
			else
			{
				MessageBox(hwnd, __TEXT("Can't map view."),
					NULL, MB_OK);
			}
			CloseHandle(hFileMapT);
		}
		else
		{
			MessageBox(hwnd,
				__TEXT("Can't open mapping."),
				NULL,
				MB_OK);
		}
	}
	break;
	case IDC_WRITE:
	{
		if (codeNotify != BN_CLICKED)
			break;
		hFileMapT = OpenFileMapping(
			FILE_MAP_READ | FILE_MAP_WRITE,
			FALSE,
			__TEXT("MMFSharedData"));
		if (hFileMapT != NULL)
		{
			LPVOID lpView = MapViewOfFile(hFileMapT,
				FILE_MAP_READ | FILE_MAP_WRITE,
				0, 0, 4 * 1024);
			if ((BYTE*)lpView != NULL)
			{
				Edit_GetText(GetDlgItem(hwnd, IDC_DATA),
					(LPTSTR)lpView, 4 * 1024);
				UnmapViewOfFile((LPVOID)lpView);
			}
			else
			{
				MessageBox(hwnd, __TEXT("Can't map view."),
					NULL, MB_OK);
			}
			CloseHandle(hFileMapT);
		}
		else
		{
			MessageBox(hwnd, __TEXT("Can't open mapping."),
				NULL, MB_OK);
		}
	}
	break;
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;
	}
}
BOOL CALLBACK Dlg_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL fProcessed = TRUE;
	switch (uMsg)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, Dlg_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, Dlg_OnCommand);
	default:
		fProcessed = FALSE;
		break;
	}
	return(fProcessed);
}
int WINAPI WinMain(HINSTANCE hinstExe, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int nCrndShow)
{
	DialogBox(hinstExe, MAKEINTRESOURCE(IDD_MMFSHARE), NULL, (DLGPROC)Dlg_Proc);
	return (0);
}
