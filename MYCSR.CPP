// mycsr.cpp
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include "res.h"
#include "mycsrlib.h"
#include "mycsr.h"

// static
static HINSTANCE ghInst;
static int nNoIcon;
static int nLockRes;
static BOOL fLock;
static BYTE szClassName[] = "MyCsrWClass";
static BYTE szIniFile[] = "mycsr.ini";
static BYTE szSctEnv[] = "Env";
static BYTE szEntNoIcon[] = "NoDesktopIcon";
static BYTE szEntLockRes[] = "LockCursorResource";
static BYTE szSctType[] = "Type";
static BYTE szEntPathName[] = "PathName";
static BYTE szEntInvColor[] = "InvertColor";
static MYCSRSTRUCT mycsr[CURSOR_MAX] = {
	{ IDC_ARROW,    "Arrow",      NULL, "", FALSE },
	{ IDC_IBEAM,    "I-Beam",     NULL, "", FALSE },
	{ IDC_WAIT,     "Wait",       NULL, "", FALSE },
	{ IDC_CROSS,    "Cross",      NULL, "", FALSE },
	{ IDC_SIZENWSE, "Size NW-SE", NULL, "", FALSE },
	{ IDC_SIZENESW, "Size NE-SW", NULL, "", FALSE },
	{ IDC_SIZEWE,   "Size W-E",   NULL, "", FALSE },
	{ IDC_SIZENS,   "Size N-S",   NULL, "", FALSE }
};

//
//	windows main
//
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hInstPrev, LPSTR lpszCmdLine, int nCmdShow )
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;

	if( hInstPrev != NULL ){
		hWnd = FindWindow( (LPSTR)szClassName, NULL );
		if( hWnd != NULL ){
			ShowWindow( hWnd, SW_SHOWMINIMIZED );
			BringWindowToTop( hWnd );
		}
		return( FALSE );
	}

	if( hInstPrev == NULL ){
		wc.style			= NULL;
		wc.lpfnWndProc		= MainWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= LoadIcon( hInstance, "MyCsrIcon" );
		wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= (LPSTR)szClassName;
		if( RegisterClass( &wc ) == 0 ){
			return( FALSE );
		} 
	}

	ghInst = hInstance;

	hWnd = CreateWindow( (LPSTR)szClassName, "MyCsr",
		WS_OVERLAPPEDWINDOW | WS_ICONIC,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL );

	if( nNoIcon == 0 ){
		ShowWindow( hWnd, SW_SHOWMINIMIZED );
		UpdateWindow( hWnd );
		BringWindowToTop( hWnd );
	} else {
		ShowWindow( hWnd, SW_HIDE );
	}

	InstallKeyboardHook( hWnd, WM_USER_INKEY );

	while( GetMessage( &msg, NULL, 0, 0 )){
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	UninstallKeyboardHook();

	return( msg.wParam );
}

//
//	main window proc
//
LRESULT CALLBACK _export MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	BOOL fRet;
	HMENU hMenu;
	FARPROC lpfnProc;
	POINT point;

	switch( message ){
	case WM_CREATE:
		hMenu = GetSystemMenu( hWnd, FALSE );
		ModifyMenu( hMenu, SC_RESTORE,  MF_BYCOMMAND | MF_STRING | MF_ENABLED, SC_RESTORE, "&Setup..." );
		DeleteMenu( hMenu, SC_SIZE,     MF_BYCOMMAND );
		DeleteMenu( hMenu, SC_MINIMIZE, MF_BYCOMMAND );
		DeleteMenu( hMenu, SC_MAXIMIZE, MF_BYCOMMAND );
		DrawMenuBar( hWnd );
		LoadIniFile();
		LoadAllCursor();
		fLock = FALSE;
		if( nLockRes == TRUE ){
			LockAllCursor();
		}
		return( 0L );
	case WM_QUERYOPEN:
		lpfnProc = MakeProcInstance( (FARPROC)SetupDlgProc, ghInst );
		fRet = DialogBox( ghInst, "SetupBox", hWnd, (DLGPROC)lpfnProc );
		FreeProcInstance( lpfnProc );
		if( nNoIcon != 0 ){
			ShowWindow( hWnd, SW_HIDE );
		}
		if( nLockRes == TRUE ){
        	LockAllCursor();
		} else {
        	UnlockAllCursor();
		}
		return( 0L );
	case WM_ENDSESSION:
		SaveIniFile();
		return( 0L );
	case WM_CLOSE:
		DestroyWindow( hWnd );
		return( 0L );
	case WM_DESTROY:
		SaveIniFile();
		UnlockAllCursor();
		RestoreAllCursor();
		PostQuitMessage( 0 );
		return( 0L );
	case WM_USER_INKEY:
		GetCursorPos( &point );
		if( wParam == VK_NUMPAD4 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.x--;
		} else if( wParam == VK_NUMPAD6 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.x++;
		} else if( wParam == VK_NUMPAD8 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.y--;
		} else if( wParam == VK_NUMPAD2 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.y++;
		} else if( wParam == VK_NUMPAD7 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.x--;
			point.y--;
		} else if( wParam == VK_NUMPAD9 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.x++;
			point.y--;
		} else if( wParam == VK_NUMPAD1 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.x--;
			point.y++;
		} else if( wParam == VK_NUMPAD3 && ( HIWORD( lParam ) & 0x8000 ) == 0 ){
			point.x++;
			point.y++;
		}
		SetCursorPos( point.x, point.y );
		return( 0L );
	}
	return( DefWindowProc( hWnd, message, wParam, lParam ));
}

//
//	setup dialog proc
//
BOOL CALLBACK _export SetupDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	int i, j;
	HDC hDC;
	TEXTMETRIC tm;
	RECT rect;
	HCURSOR hCsr;
	OFSTRUCT of;
	LPBYTE lpbMem;
	LPDRAWITEMSTRUCT lpdraw;
	HBRUSH hBrush;

	switch( message ){
	case WM_INITDIALOG:
		hDC = GetDC( GetDlgItem( hDlg, IDD_CURLIST ));
		SelectObject( hDC, GetStockObject( SYSTEM_FONT ));
		GetTextMetrics( hDC, &tm );
		ReleaseDC( GetDlgItem( hDlg, IDD_CURLIST ), hDC );
		i = max( tm.tmHeight, GetSystemMetrics( SM_CYICON )) + 4;
		SendDlgItemMessage( hDlg, IDD_CURLIST, LB_SETITEMHEIGHT, 0, MAKELPARAM( i, 0 ));
		for( i = 0; i < CURSOR_MAX; i++ ){
			SendDlgItemMessage( hDlg, IDD_CURLIST, LB_ADDSTRING, 0, (LPARAM)mycsr[i].name );
		}
		CheckDlgButton( hDlg, IDD_NOICON, nNoIcon );
		CheckDlgButton( hDlg, IDD_LOCKRES, nLockRes );
		SendDlgItemMessage( hDlg, IDD_PATHNAME, EM_LIMITTEXT, _MAX_PATH, 0L ); 
		SelectListBox( hDlg, 0 );
		GetWindowRect( hDlg, &rect );
		i = ( GetSystemMetrics( SM_CXSCREEN ) - ( rect.right - rect.left )) / 2;
		j = ( GetSystemMetrics( SM_CYSCREEN ) - ( rect.bottom - rect.top )) / 2;
		SetWindowPos( hDlg, NULL, i, j, 0, 0, SWP_NOSIZE );
		return( TRUE );
	case WM_COMMAND:
		switch( wParam ){
		case IDD_CURLIST:
			switch( HIWORD( lParam )){
			case LBN_SELCHANGE:
				i = (int)SendDlgItemMessage( hDlg, IDD_CURLIST, LB_GETCURSEL, 0, 0L );
				SelectListBox( hDlg, i );
			}
			return( TRUE );
		case IDD_INVCOLOR:
			i = (int)SendDlgItemMessage( hDlg, IDD_CURLIST, LB_GETCURSEL, 0, 0L );
			mycsr[i].fInvColor = IsDlgButtonChecked( hDlg, IDD_INVCOLOR );
			hCsr = FileToCursor( (LPSTR)mycsr[i].szPathName, mycsr[i].fInvColor );
			if( hCsr != NULL ){
				lpbMem = (LPBYTE)GlobalLock( hCsr );
				MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
				GlobalUnlock( hCsr );
				DestroyCursor( hCsr );
			}
			SelectListBox( hDlg, i );
			return( TRUE );
		case IDD_BROWSE:
			i = (int)SendDlgItemMessage( hDlg, IDD_CURLIST, LB_GETCURSEL, 0, 0L );
			lstrcpy( (LPSTR)of.szPathName, (LPSTR)mycsr[i].szPathName );
			if( CursorFileBrowse( hDlg, &of ) == TRUE ){
				hCsr = FileToCursor( of.szPathName, mycsr[i].fInvColor );
				if( hCsr != NULL ){
					lpbMem = (LPBYTE)GlobalLock( hCsr );
					MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
					GlobalUnlock( hCsr );
					DestroyCursor( hCsr );
					lstrcpy( (LPSTR)mycsr[i].szPathName, (LPSTR)of.szPathName );
					SelectListBox( hDlg, i );
				}
			}
			return( TRUE );
		case IDD_DEFAULT:
			i = (int)SendDlgItemMessage( hDlg, IDD_CURLIST, LB_GETCURSEL, 0, 0L );
			lpbMem = (LPBYTE)GlobalLock( mycsr[i].hcsr );
			MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
			GlobalUnlock( mycsr[i].hcsr );
			mycsr[i].szPathName[0] = 0x00;
			mycsr[i].fInvColor = FALSE;
			SelectListBox( hDlg, i );
			return( TRUE );
		case IDOK:
			nNoIcon  = IsDlgButtonChecked( hDlg, IDD_NOICON );
			nLockRes = IsDlgButtonChecked( hDlg, IDD_LOCKRES );
			SaveIniFile();
			ShowCursor( FALSE );
			ShowCursor( TRUE );
			EndDialog( hDlg, TRUE );
			return( TRUE );
		case IDCANCEL:
			LoadIniFile();
			for( i = 0; i < CURSOR_MAX; i++ ){
				j = FALSE;
				if( mycsr[i].szPathName[0] != 0x00 ){
					hCsr = FileToCursor( (LPSTR)mycsr[i].szPathName, mycsr[i].fInvColor );
					if( hCsr != NULL ){
						lpbMem = (LPBYTE)GlobalLock( hCsr );
						MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
						GlobalUnlock( hCsr );
						DestroyCursor( hCsr );
						j = TRUE;
					}
				}
				if( j == FALSE ){
					lpbMem = (LPBYTE)GlobalLock( mycsr[i].hcsr );
					MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
					GlobalUnlock( mycsr[i].hcsr );
					mycsr[i].szPathName[0] = 0x00;
				}
			}
			ShowCursor( FALSE );
			ShowCursor( TRUE );
			EndDialog( hDlg, FALSE );
			return( TRUE );
		}
		return( FALSE );
	case WM_DRAWITEM:
		lpdraw = (LPDRAWITEMSTRUCT)lParam;
		switch( lpdraw->CtlID ){
		case IDD_CURLIST:
			if( lpdraw->itemID == -1 ){
				CopyRect( &rect, &lpdraw->rcItem );
				InflateRect( &rect, -1, -1 );
				DrawFocusRect( lpdraw->hDC, &rect );
			} else {
				switch( lpdraw->itemAction ){
				case ODA_DRAWENTIRE:
				case ODA_SELECT:
					CopyRect( &rect, &lpdraw->rcItem );
					InflateRect( &rect, -1, -1 );
					if( lpdraw->itemState & ODS_SELECTED ){
						hBrush = CreateSolidBrush( GetSysColor( COLOR_HIGHLIGHT ));
						SetTextColor( lpdraw->hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ));
						SetBkColor( lpdraw->hDC, GetSysColor( COLOR_HIGHLIGHT ));
					} else {
						hBrush = CreateSolidBrush( GetSysColor( COLOR_WINDOW ));
						SetTextColor( lpdraw->hDC, GetSysColor( COLOR_WINDOWTEXT ));
						SetBkColor( lpdraw->hDC, GetSysColor( COLOR_WINDOW ));
					}
					FillRect( lpdraw->hDC, &rect, hBrush );
					DeleteObject( hBrush );
					CopyRect( &rect, &lpdraw->rcItem );
					j = lpdraw->rcItem.bottom - lpdraw->rcItem.top;
					rect.top +=  (( j - GetSystemMetrics( SM_CYICON )) / 2 );
					hCsr = LoadCursor( NULL, mycsr[lpdraw->itemID].type );
					DrawIcon( lpdraw->hDC, rect.left + 6, rect.top, (HICON)hCsr );
					rect.left += ( GetSystemMetrics( SM_CXICON ) + 12 );
					DrawText( lpdraw->hDC, mycsr[lpdraw->itemID].name, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
					break;
				case ODA_FOCUS:
					CopyRect( &rect, &lpdraw->rcItem );
					InflateRect( &rect, -1, -1 );
					DrawFocusRect( lpdraw->hDC, &rect );
					break;
				}
			}
		}
		return( TRUE );
	}
	return( FALSE );
}

//
//	select list box
//
VOID SelectListBox( HWND hDlg, int no )
{
	ShowCursor( FALSE );

	SendDlgItemMessage( hDlg, IDD_CURLIST, LB_SETCURSEL, no, 0L );

	if( mycsr[no].szPathName[0] != 0x00 ){
		SetDlgItemText( hDlg, IDD_PATHNAME, (LPSTR)mycsr[no].szPathName );
		EnableWindow( GetDlgItem( hDlg, IDD_INVCOLOR ), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDD_DEFAULT ),  TRUE );
	} else {
		SetDlgItemText( hDlg, IDD_PATHNAME, "<Default>" );
		EnableWindow( GetDlgItem( hDlg, IDD_INVCOLOR ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDD_DEFAULT ),  FALSE );
	}

	SendDlgItemMessage( hDlg, IDD_PATHNAME, EM_SETSEL, 0, MAKELPARAM( 0, -1 )); 

	CheckDlgButton( hDlg, IDD_INVCOLOR, mycsr[no].fInvColor );

	ShowCursor( TRUE );
}

//
//	load ini file
//
VOID LoadIniFile( VOID )
{
	int i;
	BYTE szEnt[100];

	nNoIcon  = GetPrivateProfileInt( (LPSTR)szSctEnv, (LPSTR)szEntNoIcon,  0, (LPSTR)szIniFile );
	nLockRes = GetPrivateProfileInt( (LPSTR)szSctEnv, (LPSTR)szEntLockRes, 0, (LPSTR)szIniFile );

	for( i = 0; i < CURSOR_MAX; i++ ){
		wsprintf( (LPSTR)szEnt, "%s%d", (LPSTR)szEntPathName, i );
		GetPrivateProfileString( (LPSTR)szSctType, (LPSTR)szEnt, "", (LPSTR)mycsr[i].szPathName, sizeof(mycsr[i].szPathName), (LPSTR)szIniFile );
		wsprintf( (LPSTR)szEnt, "%s%d", (LPSTR)szEntInvColor, i );
		mycsr[i].fInvColor = GetPrivateProfileInt( (LPSTR)szSctType, (LPSTR)szEnt, FALSE, (LPSTR)szIniFile );
	}
}

//
//	save ini file
//
VOID SaveIniFile( VOID )
{
	int i;
	BYTE szEnt[100];
	BYTE szBuf[100];

	wsprintf( (LPSTR)szBuf, "%d", nNoIcon );
	WritePrivateProfileString( (LPSTR)szSctEnv, (LPSTR)szEntNoIcon,  (LPSTR)szBuf, (LPSTR)szIniFile );
	wsprintf( (LPSTR)szBuf, "%d", nLockRes );
	WritePrivateProfileString( (LPSTR)szSctEnv, (LPSTR)szEntLockRes, (LPSTR)szBuf, (LPSTR)szIniFile );

	for( i = 0; i < CURSOR_MAX; i++ ){
		wsprintf( (LPSTR)szEnt, "%s%d", (LPSTR)szEntPathName, i );
		if( mycsr[i].szPathName[0] != 0x00 ){
			WritePrivateProfileString( (LPSTR)szSctType, (LPSTR)szEnt, (LPSTR)mycsr[i].szPathName, (LPSTR)szIniFile );
		} else {
			WritePrivateProfileString( (LPSTR)szSctType, (LPSTR)szEnt, "", (LPSTR)szIniFile );
		}
		wsprintf( (LPSTR)szEnt, "%s%d", (LPSTR)szEntInvColor, i );
		wsprintf( (LPSTR)szBuf, "%d", mycsr[i].fInvColor );
		WritePrivateProfileString( (LPSTR)szSctType, (LPSTR)szEnt, (LPSTR)szBuf, (LPSTR)szIniFile );
	}
}

//
//	load all cursor
//
VOID LoadAllCursor( VOID )
{
	int i;
	LPBYTE lpbMem;
	HCURSOR hCsr;

	for( i = 0; i < CURSOR_MAX; i++ ){
		hCsr = LoadCursor( NULL, mycsr[i].type );
		mycsr[i].hcsr = CopyCursor( ghInst, hCsr );
		GlobalLRUNewest( hCsr );
		if( mycsr[i].szPathName[0] != 0x00 ){
			hCsr = FileToCursor( (LPSTR)mycsr[i].szPathName, mycsr[i].fInvColor );
			if( hCsr != NULL ){
				lpbMem = (LPBYTE)GlobalLock( hCsr );
				MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
				GlobalUnlock( hCsr );
				DestroyCursor( hCsr );
			} else {
				mycsr[i].szPathName[0] = 0x00;
			}
		}
	}
}

//
//	restore all cursor
//
VOID RestoreAllCursor( VOID )
{
	int i;
	LPBYTE lpbMem;
	HCURSOR hCsr;

	ShowCursor( FALSE );
	hCsr = GetCursor();

	for( i = 0; i < CURSOR_MAX; i++ ){
		hCsr = LoadCursor( NULL, mycsr[i].type );
		lpbMem = (LPBYTE)GlobalLock( mycsr[i].hcsr );
		SetCursor( LoadCursor( NULL, mycsr[i].type ));
		MemoryToCursor( lpbMem, LoadCursor( NULL, mycsr[i].type ));
		GlobalUnlock( mycsr[i].hcsr );
		DestroyCursor( mycsr[i].hcsr );
	}

	SetCursor( hCsr );
	ShowCursor( TRUE );
}

//
//	lock all cursor
//
VOID LockAllCursor( VOID )
{
	int i;

	if( fLock == FALSE ){
		for( i = 0; i < CURSOR_MAX; i++ ){
			GlobalLock( LoadCursor( NULL, mycsr[i].type ));
		}
		fLock = TRUE;
	}
}

//
//	unlock all cursor
//
VOID UnlockAllCursor( VOID )
{
	int i;

	if( fLock == TRUE ){
		for( i = 0; i < CURSOR_MAX; i++ ){
			GlobalUnlock( LoadCursor( NULL, mycsr[i].type ));
		}
		fLock = FALSE;
	}
}

//
//	memory To cursor
//
BOOL MemoryToCursor( LPBYTE lpbMem, HCURSOR hCsr )
{
	UINT i, j;	
	LPBYTE lpbCsr;

	if( lpbMem == NULL || hCsr == NULL ){
		return( FALSE );
	}

	lpbCsr = (LPBYTE)GlobalLock( hCsr );
	if( lpbCsr ==  NULL ){
		return( FALSE );
	}

	j = (UINT)GlobalSize( hCsr );
	if( j == 0 ){
		GlobalUnlock( hCsr );
		return( FALSE );
	}

	for( i = 0; i < j; i++ ){
		lpbCsr[i] = lpbMem[i];
	}

	GlobalUnlock( hCsr );

	return( TRUE );
}

//
//	file to cursor
//
HCURSOR FileToCursor( LPSTR lpszPathName, BOOL fInvert )
{
	int i, j;
	BYTE bDat;
	BYTE bXor[128];
	BYTE bAnd[128];
	HCURSOR hCsr;
	HFILE hFile;	
	DWORD dwPos;
	DWORD dwLen;
	HGLOBAL hgMem;
	LPBYTE lpbMem;
	LPCURSORDIR lpdir;
	LPCURSORDIRENTRY lpent;
	LPBITMAPINFOHEADER lphead;
	LPRGBQUAD lprgb;
	LPBYTE lpxor;
	LPBYTE lpand;

	hFile = _lopen( lpszPathName, OF_READ | OF_SHARE_DENY_WRITE );
	if( hFile == NULL ){
		return( NULL );
	}

	dwPos = _llseek( hFile, 0L, 1 );
	dwLen = _llseek( hFile, 0L, 2 );
	_llseek( hFile, dwPos, 0 );
	if( dwLen == 0L ){
		_lclose( hFile );
		return( NULL );
	}

	hgMem = GlobalAlloc( GHND, dwLen );
	if( hgMem == NULL ){
		_lclose( hFile );
		return( NULL );
	}

	lpbMem = (LPBYTE)GlobalLock( hgMem );
	_lread( hFile, lpbMem, (UINT)dwLen );	
	_lclose( hFile );

	lpdir = (LPCURSORDIR)lpbMem;
	if( lpdir->cbReserved != 0 || lpdir->cbType != 2 || lpdir->cbCount < 1 ){
		GlobalUnlock( hgMem );
		GlobalFree( hgMem );
		return( NULL );
	}

	lpent = lpdir->cbEntries;
	lphead = (LPBITMAPINFOHEADER)( (DWORD)lpbMem + lpent->dwImageOffset );
	lprgb = (LPRGBQUAD)( (DWORD)lphead + lphead->biSize );
	lpxor = (LPBYTE)lprgb + sizeof(RGBQUAD) * ( 1 << lphead->biBitCount );
	lpand = (LPBYTE)lpxor + 128;

	for( i = 0; i < 32; i++ ){
		for( j = 0; j < 4; j++ ){
			bXor[i*4+j] = lpxor[(j+124)-(i*4)];
			bAnd[i*4+j] = lpand[(j+124)-(i*4)];
			if( fInvert == TRUE ){
				bDat = bAnd[i*4+j];
				bXor[i*4+j] ^= ( bDat ^ 0xFF );
			}
		}
	}

	i = GetSystemMetrics( SM_CXCURSOR );
	j = GetSystemMetrics( SM_CYCURSOR );
	hCsr = CreateCursor( ghInst, lpent->wXHotspot, lpent->wYHotspot, i, j, bAnd, bXor );

	GlobalUnlock( hgMem );
	GlobalFree( hgMem );

	return( hCsr );
}

//
//	cursor file browse
//
BOOL CursorFileBrowse( HWND hWnd, LPOFSTRUCT lpof )
{
	int len;
	BYTE szFile[_MAX_PATH];
	BYTE szFileTitle[_MAX_PATH];
	BYTE szInitDir[_MAX_DIR];
	BYTE szDrv[_MAX_DRIVE];
	BYTE szDir[_MAX_DIR];
	BYTE szName[_MAX_FNAME];
	BYTE szExt[_MAX_EXT];
	OPENFILENAME ofn;
	static BYTE szFilter[] = "Cursors (*.cur)\0*.cur\0All Files (*.*)\0*.*\0\0";

	lstrcpy( (LPSTR)szFile, lpof->szPathName );
	_splitpath( (char*)szFile, (char*)szDrv, (char*)szDir, (char*)szName, (char*)szExt );
	lstrcpy( (LPSTR)szFile, (LPSTR)szName );
	lstrcat( (LPSTR)szFile, (LPSTR)szExt );
	lstrcpy( (LPSTR)szInitDir, (LPSTR)szDrv );
	lstrcat( (LPSTR)szInitDir, (LPSTR)szDir );
	len = lstrlen( (LPSTR)szInitDir );
	szInitDir[len-1] = 0;

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= hWnd;
	ofn.hInstance			= ghInst;
	ofn.lpstrFilter			= (LPSTR)szFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= (LPSTR)szFile;
	ofn.nMaxFile			= sizeof(szFile);
	ofn.lpstrFileTitle		= (LPSTR)szFileTitle;
	ofn.nMaxFileTitle		= sizeof(szFileTitle);
	ofn.lpstrInitialDir		= (LPSTR)szInitDir;
	ofn.lpstrTitle			= "Browse";
	ofn.Flags				= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
	ofn.lpstrDefExt			= NULL;
	ofn.lCustData			= 0L;
	ofn.lpfnHook			= (DLGHOOKPROC)MakeProcInstance( (FARPROC)FileHookProc, ghInst );
	ofn.lpTemplateName		= "BrowseBox";

	if( GetOpenFileName( &ofn ) == FALSE ){
		FreeProcInstance( (FARPROC)ofn.lpfnHook );
		return( FALSE );
	}

	if( OpenFile( (LPSTR)szFile, lpof, OF_EXIST ) == -1 ){
		FreeProcInstance( (FARPROC)ofn.lpfnHook );
		return( FALSE );
	}

	FreeProcInstance( (FARPROC)ofn.lpfnHook );

	return( TRUE );
}

//
//	file hook proc
//
UINT CALLBACK _export FileHookProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UINT uSel;
	BYTE szBuf[300];
	static HCURSOR hCur;

	switch( message ){
	case WM_INITDIALOG:
		hCur = NULL;
		break;
	case WM_COMMAND:
		switch( wParam ){
		case lst1:
			switch( HIWORD( lParam )){
			case LBN_KILLFOCUS:
			//	SendDlgItemMessage( hDlg, stc20, STM_SETICON, (WPARAM)NULL, 0L );
				break;
			case LBN_SELCHANGE:
				uSel = (UINT)SendDlgItemMessage( hDlg, lst1, LB_GETCURSEL, 0, 0L );
				SendDlgItemMessage( hDlg, lst1, LB_GETTEXT, uSel, (LPARAM)(LPSTR)szBuf );
				if( hCur != NULL ){
					DestroyCursor( hCur );
					hCur = NULL;
				}
				hCur = FileToCursor( (LPSTR)szBuf, FALSE );
				if( hCur != NULL ){
					SendDlgItemMessage( hDlg, stc20, STM_SETICON, (WPARAM)hCur, 0L );
				}
				break;
			}
			break;
		case IDOK:
		case IDCANCEL:
			if( hCur != NULL ){
				DestroyCursor( hCur );
				hCur = NULL;
			}
			break;
		}
		break;
	}
	return( FALSE );
}

// [EOF]
