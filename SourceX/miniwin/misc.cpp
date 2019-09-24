#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"
#include <SDL.h>
#include <string>

#include "DiabloUI/diabloui.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#if !defined(_MSC_VER) && !defined(__x86_64__) && !defined(__i386__)
unsigned int _rotl(unsigned int value, int shift)
{
	if ((shift &= 31) == 0)
		return value;
	return (value << shift) | (value >> (32 - shift));
}

unsigned int _rotr(unsigned int value, int shift)
{
	if ((shift &= 31) == 0)
		return value;
	return (value >> shift) | (value << (32 - shift));
}
#endif

namespace dvl {

DWORD last_error;

DWORD GetLastError()
{
	return last_error;
}

void SetLastError(DWORD dwErrCode)
{
	last_error = dwErrCode;
}

char *_strlwr(char *str)
{
	for (char *p = str; *p; ++p) {
		*p = tolower(*p);
	}
	return str;
}

int wsprintfA(LPSTR dest, LPCSTR format, ...)
{
	va_list args;
	va_start(args, format);
	return vsprintf(dest, format, args);
}

int wvsprintfA(LPSTR dest, LPCSTR format, va_list arglist)
{
	return vsnprintf(dest, 256, format, arglist);
}

int _strcmpi(const char *_Str1, const char *_Str2)
{
	return strcasecmp(_Str1, _Str2);
}

int _strnicmp(const char *_Str1, const char *_Str2, size_t n)
{
	return strncasecmp(_Str1, _Str2, n);
}

char *_itoa(int _Value, char *_Dest, int _Radix)
{
	switch (_Radix) {
	case 8:
		sprintf(_Dest, "%o", _Value);
		break;
	case 10:
		sprintf(_Dest, "%d", _Value);
		break;
	case 16:
		sprintf(_Dest, "%x", _Value);
		break;
	default:
		UNIMPLEMENTED();
		break;
	}

	return _Dest;
}

DWORD GetTickCount()
{
	return SDL_GetTicks();
}

void Sleep(DWORD dwMilliseconds)
{
	SDL_Delay(dwMilliseconds);
}

HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
	DUMMY();
	return (HANDLE)-1;
}

WINBOOL FindClose(HANDLE hFindFile)
{
	UNIMPLEMENTED();
	return true;
}

/**
 * @brief Normally this would get the Windows install, but Diablo uses it to find the old save game folder
 */
UINT GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize)
{
#ifndef SWITCH
	char *name = SDL_GetPrefPath("diasurgical", "devilution");
	strncpy(lpBuffer, name, uSize);
	SDL_free(name);

	DWORD len = strlen(lpBuffer);

	lpBuffer[len - 1] = '\0';

	return len - 1;
#else
	return 0;
#endif
}

WINBOOL GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters)
{
	*lpBytesPerSector = 1;
	*lpSectorsPerCluster = 1;
	*lpNumberOfFreeClusters = 10 << 20;
	*lpTotalNumberOfClusters = 10 << 20;
	return true;
}

/**
 * @brief Used for getting save path, by removing up to and including the last "\"
 */
DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
#ifndef SWITCH
	char *name = SDL_GetPrefPath("diasurgical", "devilution");
	strncpy(lpFilename, name, nSize);
	SDL_free(name);

	DWORD len = strlen(lpFilename);

	lpFilename[len - 1] = '\\';

	return len;
#else
	strcpy(lpFilename,"");
	return 0;
#endif
}

WINBOOL GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize)
{
	DUMMY();
	strncpy(lpBuffer, "localhost", *nSize);
	*nSize = strlen(lpBuffer);
	return true;
}

DWORD GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
	DUMMY();
	*lpdwHandle = 0;

	return 1532;
}

BOOL GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
	DUMMY();
	*(int *)lpData = 16711836; // TODO use actual version from .rc

	return true;
}

BOOL VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
	DUMMY();
	static VS_FIXEDFILEINFO lpBuffer;

	// Set internal version, TODO use actual version from .rc
	lpBuffer.dwProductVersionMS = 1;
	lpBuffer.dwProductVersionMS <<= 16;
	lpBuffer.dwProductVersionMS |= 0 & 0xFFFF;
	lpBuffer.dwProductVersionLS = 9;
	lpBuffer.dwProductVersionLS <<= 16;
	lpBuffer.dwProductVersionLS |= 2 & 0xFFFF;
	*lplpBuffer = (LPVOID *)&lpBuffer;

	return true;
}

DWORD GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer)
{
#ifndef SWITCH
	char *base_path = SDL_GetBasePath();
	if (base_path == NULL) {
		SDL_Log(SDL_GetError());
		base_path = SDL_strdup("./");
	}
	eprintf("BasePath: %s\n", base_path);

	strncpy(lpBuffer, base_path, nBufferLength);
	SDL_free(base_path);

	return strlen(lpBuffer);
#else
    return 0;
#endif
}

DWORD GetLogicalDriveStringsA(DWORD nBufferLength, LPSTR lpBuffer)
{
	return 0;
}

UINT GetDriveTypeA(LPCSTR lpRootPathName)
{
	return DVL_DRIVE_CDROM;
}

WINBOOL DeleteFileA(LPCSTR lpFileName)
{
	char name[DVL_MAX_PATH];
	TranslateFileName(name, sizeof(name), lpFileName);

	FILE *f = fopen(name, "r+");
	if (f) {
		fclose(f);
		remove(name);
		f = NULL;
		eprintf("Removed file: %s\n", name);
	} else {
		eprintf("Failed to remove file: %s\n", name);
	}

	return true;
}

WINBOOL CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, WINBOOL bFailIfExists)
{
	UNIMPLEMENTED();
	return true;
}

HFILE OpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	DUMMY();
	return DVL_HFILE_ERROR;
}

HWND SetCapture(HWND hWnd)
{
	DUMMY_ONCE();
	return hWnd;
}

WINBOOL ReleaseCapture()
{
	DUMMY_ONCE();
	return true;
}

WINBOOL DestroyWindow(HWND hWnd)
{
	DUMMY();
	return true;
}

HWND GetLastActivePopup(HWND hWnd)
{
	UNIMPLEMENTED();
	return hWnd;
}

/**
 * @return Always null as it's unused
 */
HWND SetFocus(HWND hWnd)
{
	SDL_RaiseWindow(window);
	MainWndProc(NULL, DVL_WM_ACTIVATEAPP, true, 0); // SDL_WINDOWEVENT_FOCUS_GAINED
	return NULL;
}

void FakeWMDestroy()
{
	MainWndProc(NULL, DVL_WM_DESTROY, 0, 0);
}

HWND CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
	if (SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC) <= -1) {
		SDL_Log(SDL_GetError());
		return NULL;
	}

#ifdef USE_SDL1
	SDL_EnableUNICODE( 1 );
#endif

	atexit(SDL_Quit);

	if (SDL_JoystickOpen(0) == NULL) {
		SDL_Log(SDL_GetError());
	}
#ifndef USE_SDL1
	if (SDL_GameControllerOpen(0) == NULL) {
		SDL_Log(SDL_GetError());
	}
#endif

	int upscale = 1;
	DvlIntSetting("upscale", &upscale);
	DvlIntSetting("fullscreen", &fullscreen);

	int grabInput = 1;
	DvlIntSetting("grab input", &grabInput);

#ifdef USE_SDL1
	int flags = SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_HWPALETTE;
	if (fullscreen) {
		SDL_Log("fullscreen not yet supported with SDL1");
	}
	// flags |= fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE;
	SDL_WM_SetCaption(lpWindowName, WINDOW_ICON_NAME);
	SDL_SetVideoMode(nWidth, nHeight, /*bpp=*/0, flags);
	window = SDL_GetVideoSurface();
	if (grabInput) SDL_WM_GrabInput(SDL_GRAB_ON);
#else
	int flags = 0;
	if (upscale) {
		flags |= fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE;

		char scaleQuality[2] = "2";
		DvlStringSetting("scaling quality", scaleQuality, 2);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQuality);
	} else if (fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (grabInput) {
		flags |= SDL_WINDOW_INPUT_GRABBED;
	}

#ifndef SWITCH
	window = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, nWidth, nHeight, flags);
#else
	window = SDL_CreateWindow(lpWindowName, 0, 0, 1920, 1080, 0);
#endif
#endif
	if (window == NULL) {
		SDL_Log(SDL_GetError());
	}
	atexit(FakeWMDestroy);

	if (upscale) {
#ifdef USE_SDL1
		SDL_Log("upscaling not supported with USE_SDL1");
#else
#ifndef SWITCH
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
#else
		renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
#endif
		if (renderer == NULL) {
			SDL_Log(SDL_GetError());
		}

		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, nWidth, nHeight);
		if (texture == NULL) {
			SDL_Log(SDL_GetError());
		}

		if (SDL_RenderSetLogicalSize(renderer, nWidth, nHeight) <= -1) {
			SDL_Log(SDL_GetError());
		}
#endif
	}

	return window;
}

BOOL InvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase)
{
	DUMMY();
	return true;
}

/**
 * @brief Appears to be used to clear the FB on init
 */
BOOL UpdateWindow(HWND hWnd)
{
	DUMMY();
	return true;
}

BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
	if (nCmdShow == DVL_SW_HIDE) {
		SDL_HideWindow(window);
	} else if (nCmdShow == DVL_SW_SHOWNORMAL) {
		SDL_ShowWindow(window);
	}

	return true;
}

/**
 * @brief Because we don't change resolution it dosen't make sens to use SDL_GetCurrentDisplayMode
 */
int GetSystemMetrics(int nIndex)
{
	switch (nIndex) {
	case DVL_SM_CXSCREEN:
		return SCREEN_WIDTH;
	case DVL_SM_CYSCREEN:
		return SCREEN_HEIGHT;
	}

	return 0;
}

/**
 * @brief Used for getting a black brush
 */
HGDIOBJ GetStockObject(int i)
{
	return NULL;
}

BOOL IsBadReadPtr(const void *lp, UINT_PTR ucb)
{
	UNIMPLEMENTED();
	return true;
}

BOOL IsBadWritePtr(LPVOID lp, UINT_PTR ucb)
{
	UNIMPLEMENTED();
	return true;
}

SIZE_T VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
	UNIMPLEMENTED();
	return false;
}

LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	UNIMPLEMENTED();
	return NULL;
}

BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
	UNIMPLEMENTED();
	return false;
}

void GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	UNIMPLEMENTED();
}

long _findfirst(const char *pattern, struct DVL_finddata_t *finder)
{
	DUMMY();
	return -1;
}

int _findnext(long, struct DVL_finddata_t *finder)
{
	UNIMPLEMENTED();
	return -1;
}

HMODULE GetModuleHandleA(LPCSTR lpModuleName)
{
	UNIMPLEMENTED();
	return NULL;
}

BOOL GetUserNameA(LPSTR lpBuffer, LPDWORD pcbBuffer)
{
	UNIMPLEMENTED();
	return false;
}

HDC GetDC(HWND hWnd)
{
	DUMMY();
	return NULL;
}

int ReleaseDC(HWND hWnd, HDC hDC)
{
	DUMMY();
	return 0;
}

int GetDeviceCaps(HDC hdc, int index)
{
	SDL_DisplayMode current;

	if (SDL_GetCurrentDisplayMode(0, &current) <= -1) {
		SDL_Log(SDL_GetError());
		return 0;
	}

	if (index == DVL_HORZRES) {
		return current.w;
	}
	if (index == DVL_VERTRES) {
		return current.h;
	}

	return 0;
}

BOOL GetWindowRect(HWND hDlg, tagRECT *Rect)
{
	int x, y, w, h;
	SDL_GetWindowPosition(window, &x, &y);
	SDL_GetWindowSize(window, &w, &h);

	Rect->right = x;
	Rect->top = y;
	Rect->left = w + x;
	Rect->bottom = h + y;

	return true;
}

UINT GetSystemPaletteEntries(HDC hdc, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries)
{
	DUMMY();
	return 0;
}

BOOL GetVersionExA(LPOSVERSIONINFOA lpVersionInformation)
{
	lpVersionInformation->dwMajorVersion = 5;
	lpVersionInformation->dwMinorVersion = 0;
	lpVersionInformation->dwPlatformId = DVL_VER_PLATFORM_WIN32_NT;
	return true;
}

void lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength)
{
	strncpy(lpString1, lpString2, iMaxLength);
}

DWORD GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
    DWORD nSize, LPCSTR lpFileName)
{
	if (!SRegLoadString(lpAppName, lpKeyName, 0, lpReturnedString, nSize)) {
		strncpy(lpReturnedString, lpDefault, nSize);
		SRegSaveString(lpAppName, lpKeyName, 0, lpReturnedString);
	}
	return 0; // dummy return value
}

int MessageBoxA(HWND hWnd, const char *Text, const char *Title, UINT Flags)
{
#ifndef SWITCH
	Uint32 SDLFlags = 0;
	if (Flags & DVL_MB_ICONHAND) {
		SDLFlags |= SDL_MESSAGEBOX_ERROR;
	} else if (Flags & DVL_MB_ICONEXCLAMATION) {
		SDLFlags |= SDL_MESSAGEBOX_WARNING;
	}

	if (SDL_ShowSimpleMessageBox(SDLFlags, Title, Text, window) <= -1) {
		SDL_Log(SDL_GetError());
		return -1;
	}

#endif
	return 0;
}

LSTATUS RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	return 1;
};

/**
 * @brief This is only ever used to enable or disable the screen saver in a hackish way
 * For all other settings operation SReg* from Storm is used instead.
 */
LSTATUS RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, BYTE *lpData, LPDWORD lpcbData)
{
	#ifndef USE_SDL1
	if (SDL_IsScreenSaverEnabled()) {
		lpData[0] = '0';
		lpData[1] = '\0';
	}
	#endif

	return 1;
};

/**
 * @brief This is only ever used to enable or disable the screen saver in a hackish way
 * For all other settings operation SReg* from Storm is used instead.
 */
LSTATUS RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData)
{
	#ifndef USE_SDL1
	if (lpData[0] == '0') {
		SDL_DisableScreenSaver();
	} else {
		SDL_EnableScreenSaver();
	}
	#endif

	return 1;
};

LSTATUS RegCloseKeyA(HKEY hKey)
{
	return 1;
};

void PostQuitMessage(int nExitCode)
{
	DUMMY();
	PostMessageA(NULL, DVL_WM_QUERYENDSESSION, 0, 0);
}

LRESULT DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DUMMY_ONCE();
	if (Msg == DVL_WM_QUERYENDSESSION)
		exit(0);

	return 0;
}

LONG GetWindowLongA(HWND hWnd, int nIndex)
{
	DUMMY();
	return 0;
}

LONG SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong)
{
	DUMMY();
	return 0;
}
}
