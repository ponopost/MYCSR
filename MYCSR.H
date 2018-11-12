// mycsr.h

// windowsx.h
//#define HANDLE_MSG(hwnd,message,fn)		case (message): return HANDLE_##message((hwnd), (wParam), (lParam), (fn))

// define
#define WM_USER_INKEY	(WM_USER+0x100)
#define CURSOR_MAX		8

// struct
typedef struct tagMYCSRSTRUCT {
	LPCSTR type;
	LPSTR name;
	HCURSOR hcsr;
	BYTE szPathName[_MAX_PATH];
	BOOL fInvColor;
} MYCSRSTRUCT;

//
typedef struct tagCURSORDIRENTRY {
	BYTE bWidth;
	BYTE bHeight;
	BYTE bColorCount;
	BYTE bReserved;
	WORD wXHotspot;
	WORD wYHotspot;
	DWORD lBYtesInRes;
	DWORD dwImageOffset;
} CURSORDIRENTRY;
typedef CURSORDIRENTRY FAR* LPCURSORDIRENTRY;

//
typedef struct tagCURSORDIR {
	WORD cbReserved;
	WORD cbType;
	WORD cbCount;
	CURSORDIRENTRY cbEntries[1];
} CURSORDIR;
typedef CURSORDIR FAR* LPCURSORDIR;

// file open dialog hook proc
typedef UINT (CALLBACK* DLGHOOKPROC)( HWND, UINT, WPARAM, LPARAM );

// prototypes
LRESULT CALLBACK _export MainWndProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK _export SetupDlgProc( HWND, UINT, WPARAM, LPARAM );
VOID SelectListBox( HWND, int );
VOID LoadIniFile( VOID );
VOID SaveIniFile( VOID );
VOID LoadAllCursor( VOID );
VOID RestoreAllCursor( VOID );
VOID LockAllCursor( VOID );
VOID UnlockAllCursor( VOID );
BOOL MemoryToCursor( LPBYTE, HCURSOR );
HCURSOR FileToCursor( LPSTR, BOOL );
BOOL CursorFileBrowse( HWND, LPOFSTRUCT );
UINT CALLBACK _export FileHookProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

// [EOF]
