#ifndef WINSTUB_H
#define WINSTUB_H


//-------------windows------------//

//#undef main
//#define WinMain main

typedef int BOOL;
typedef char CHAR;
typedef unsigned char  BYTE;
typedef unsigned char UCHAR,*PUCHAR;
typedef unsigned short WORD;
typedef unsigned int UINT,*PUINT,*LPUINT;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef unsigned long DWORD;
typedef unsigned long ULONG,*PULONG;
typedef wchar_t WCHAR;
typedef WCHAR *PWCHAR,*LPWCH,*PWCH,*NWPSTR,*LPWSTR,*PWSTR;
typedef DWORD COLORREF;
typedef void *PVOID,*LPVOID;
typedef long LONG;
typedef long LONG_PTR, *PLONG_PTR;
typedef LONG HRESULT;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef void*  HANDLE;
#define HWND HANDLE
#define HDC__ HANDLE
#define HINSTANCE HANDLE
#define HDC HANDLE*
#define HICON HANDLE*
#define HCURSOR HANDLE*
#define HBRUSH HANDLE*
#define FALSE 0
#define TRUE 1
#define FAILED(Status) ((HRESULT)(Status)<0)
#define RGB(r,g,b) ((COLORREF)((BYTE)(r)|((BYTE)(g) << 8)|((BYTE)(b) << 16)))

#define PC_EXPLICIT 2
#define RDH_RECTANGLES	1
#define TRANSPARENT 1
#define OFS_MAXPATHNAME 128
#define OF_READ 0
#define PC_NOCOLLAPSE 4
#define WM_CREATE 1
#define WM_PAINT 15
#define WM_DESTROY 2
#define CS_DBLCLKS 8
#define CS_OWNDC 32
#define CS_HREDRAW 2
#define CS_VREDRAW 1
#define IDI_APPLICATION 32512
#define IDC_ARROW 32512
#define BLACK_BRUSH 4
#define WS_OVERLAPPED 0
#define WS_SYSMENU 0x80000
#define WS_CAPTION 0xc00000
#define WS_POPUP 0x80000000
#define WS_VISIBLE 0x10000000
#define SW_SHOW 5
#define PM_REMOVE 1
#define WM_QUIT 18

#define CP_ACP 0
#define MB_PRECOMPOSED 1

#define CALLBACK
#define WINAPI

#ifndef CONST
#define CONST const
#endif

typedef CHAR *PCHAR,*LPCH,*PCH,*NPSTR,*LPSTR,*PSTR;
typedef CONST CHAR *LPCCH,*PCSTR,*LPCSTR;
typedef LRESULT(CALLBACK *WNDPROC)(HWND,UINT,WPARAM,LPARAM);

typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID, *REFGUID, *LPGUID;

typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} BITMAPFILEHEADER,*LPBITMAPFILEHEADER,*PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER,*LPBITMAPINFOHEADER,*PBITMAPINFOHEADER;

typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT,*PRECT,*LPRECT;

typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY,*LPPALETTEENTRY,*PPALETTEENTRY;

typedef struct _RGNDATAHEADER {
	DWORD dwSize;
	DWORD iType;
	DWORD nCount;
	DWORD nRgnSize;
	RECT rcBound;
} RGNDATAHEADER;

typedef struct _RGNDATA {
	RGNDATAHEADER rdh;
	char Buffer[1];
} RGNDATA,*PRGNDATA, *LPRGNDATA;

typedef struct _OFSTRUCT {
	BYTE cBytes;
	BYTE fFixedDisk;
	WORD nErrCode;
	WORD Reserved1;
	WORD Reserved2;
	CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT,*LPOFSTRUCT,*POFSTRUCT;

typedef struct tWAVEFORMATEX {
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
} WAVEFORMATEX,*PWAVEFORMATEX,*LPWAVEFORMATEX;

typedef struct tagPAINTSTRUCT {
	HDC	hdc;
	BOOL fErase;
	RECT rcPaint;
	BOOL fRestore;
	BOOL fIncUpdate;
	BYTE rgbReserved[32];
} PAINTSTRUCT,*LPPAINTSTRUCT;

typedef struct _WNDCLASSA {
	UINT style;
	WNDPROC lpfnWndProc;
	int cbClsExtra;
	int cbWndExtra;
	HINSTANCE hInstance;
	HICON hIcon;
	HCURSOR hCursor;
	HBRUSH hbrBackground;
	LPCSTR lpszMenuName;
	LPCSTR lpszClassName;
} WNDCLASSA,*LPWNDCLASSA,*PWNDCLASSA;

typedef WNDCLASSA WNDCLASS,*LPWNDCLASS,*PWNDCLASS;

typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT,POINTL,*PPOINT,*LPPOINT,*PPOINTL,*LPPOINTL;

typedef struct tagMSG {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG,*LPMSG,*PMSG;

DWORD GetTickCount(void);
HDC BeginPaint(HWND hwnd,PAINTSTRUCT *ps);
LRESULT DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
HICON LoadIcon(void* ptr, int flags);
HCURSOR LoadCursor(void* ptr, int flags);
HBRUSH GetStockObject(int flags);
HWND CreateWindow(char* name, char* title, int flags, int x, int y, int w, int h, HANDLE parent, HANDLE menu, HINSTANCE hinstance, void* parms);
HANDLE GetMenu(HWND main_window_handle);
int OpenFile(char* filename, OFSTRUCT* file_data, int flags);
int RegisterClass(WNDCLASS * winclass);
int GetWindowStyle(HWND main_window_handle);
int GetWindowExStyle(HWND main_window_handle);
int PeekMessage(MSG *msg, void* ptr, int num1, int num2, int flags);
int GetAsyncKeyState(int vk_code);
void GetWindowRect(HWND main_window_handle, RECT *dest_rect);
void SetTextColor(HDC xdc, COLORREF color);
void SetBkMode(HDC xdc, int flags);
void TextOut(HDC xdc,int x, int y,char* text,int len);
void SetTextColor(HDC xdc, COLORREF red, COLORREF Green, COLORREF Blue);
void _lread(int file_handle, void* data,int data_size);
void _lclose(int file_handle);
void EndPaint(HWND hwnd, PAINTSTRUCT *ps);
void PostQuitMessage(int msgnum);
void AdjustWindowRectEx(RECT *window_rect, int style, BOOL menu, int exstyle);
void MoveWindow(HWND main_window_handle, int x, int y, int w, int h, BOOL boolvar);
void ShowWindow(HWND main_window_handle, int flas);
void SystemParametersInfo(int flags, BOOL boolvar, void* ptr, int num);
void TranslateMessage(MSG *msg);
void DispatchMessage(MSG *msg);
void ShowCursor(BOOL boolvar);
void PostMessage(HWND main_window_handle, int flags, int num1, int num2);

int MultiByteToWideChar(UINT a,DWORD b,LPCSTR c,int d,LPWSTR e,int f);

//-------------directx------------//

typedef struct
{
    int dwSize;
    int dwFlags;
    int dwWidth;
    int dwHeight;
    struct {
        int dwCaps;
    }ddsCaps;
    unsigned char *lpSurface;
    int lPitch;
    int dwBackBufferCount;
}DDSURFACEDESC2;

typedef struct
{
    int dwCaps;
}DDSCAPS2;

typedef struct
{
    int dwColorSpaceLowValue;
    int dwColorSpaceHighValue;
}DDCOLORKEY;

typedef struct
{
    int dwSize;
    int dwRGBBitCount;
}DDPIXELFORMAT;

typedef struct
{
    void Release(void){}
    void SetEntries(int num1, int color_index, int nuum2, LPPALETTEENTRY color){}
    void GetEntries(int num1, int color_index, int colors, LPPALETTEENTRY work_pal){}
}DIRECTDRAWPALETTE,*LPDIRECTDRAWPALETTE;

typedef struct
{
    int dwSize;
    int dwFillColor;
}DDBLTFX;

typedef struct
{
    int SetHWnd(int num, HWND main_window_handle){return 0;}
    int SetClipList(LPRGNDATA region_data, int num){return 0;}
    void Release(void){}
}DIRECTDRAWCLIPPER, *LPDIRECTDRAWCLIPPER;

typedef struct DIRECTDRAWSURFACE7
{
    void SetColorKey(int flags, DDCOLORKEY *color_key){}
    void Release(void){}
    int Blt(RECT *dstrect, DIRECTDRAWSURFACE7 *surf, RECT *srcrect, int flags, DDBLTFX *ddbltfx){return 0;}
    void Lock(int *ptr1, DDSURFACEDESC2* ddsd, int flags, int *ptr){}
    void Unlock(int *ptr){}
    void GetPixelFormat(DDPIXELFORMAT *ddpf){}
    int GetAttachedSurface(DDSCAPS2 *ddscaps,DIRECTDRAWSURFACE7 **lpddsback){return 0;}
    int SetPalette(LPDIRECTDRAWPALETTE lpddpal){return 0;}
    int SetClipper(LPDIRECTDRAWCLIPPER lpddclipperwin){return 0;}
    int Flip(int* ptr, int flags){return 0;}
    int GetDC(HDC__** hdcptr){return 0;}
    void ReleaseDC(HDC__* hdcptr){}
}DIRECTDRAWSURFACE7, *LPDIRECTDRAWSURFACE7;

typedef struct
{
    int CreateSurface(DDSURFACEDESC2 *ddsd, LPDIRECTDRAWSURFACE7 *surf, int* intptr){return 0;}
    int SetCooperativeLevel(HWND main_window_handle,int flags){return 0;}
    int SetDisplayMode(int width, int height,int bpp,int num1,int num2){return 0;}
    int CreatePalette(int flags, PALETTEENTRY* palette,LPDIRECTDRAWPALETTE* lpddpal,int* ptr){return 0;}
    int CreateClipper(int num,LPDIRECTDRAWCLIPPER* lpddclipperwin,int *ptr){return 0;}
    void Release(void){}
    void WaitForVerticalBlank(int flags, int num){}
}DIRECTDRAW7, *LPDIRECTDRAW7;

int DirectDrawCreateEx(int *ptr, void **ptr2, int flags, int *ptr3);

#define DDSCAPS_OFFSCREENPLAIN 1
#define DDCKEY_SRCBLT 1
#define DDBLT_WAIT 1
#define DDBLT_KEYSRC 2
#define DDLOCK_WAIT 1
#define DDLOCK_SURFACEMEMORYPTR 2
#define IID_IDirectDraw7 1
#define DDSCL_NORMAL 1
#define DDSCL_ALLOWMODEX 1
#define DDSCL_FULLSCREEN 2
#define DDSCL_EXCLUSIVE 3
#define DDSCL_ALLOWREBOOT 4
#define DDSCL_MULTITHREADED 5
#define DDSD_CAPS 1
#define DDSD_WIDTH 2
#define DDSD_HEIGHT 3
#define DDSD_BACKBUFFERCOUNT 2;
#define DDSCAPS_PRIMARYSURFACE 1
#define DDSCAPS_FLIP 2
#define DDSCAPS_COMPLEX 3
#define DDSCAPS_BACKBUFFER 1
#define DDSCAPS_SYSTEMMEMORY 1
#define DDPCAPS_8BIT 1
#define DDPCAPS_INITIALIZE 2
#define DDPCAPS_ALLOW256 1
#define DDFLIP_WAIT 1
#define DDWAITVB_BLOCKBEGIN 1
#define DDBLT_COLORFILL 1

//------------------------direct_input---------------------------//

#define LPDIRECTINPUTDEVICE int*
#define DIMOUSESTATE int
#define DIJOYSTATE int
#define DIPROPHEADER int
#define DIENUM_STOP 1
#define IID_IDirectInput8 1
#define DI8DEVCLASS_GAMECTRL 1
#define DIEDFL_ATTACHEDONLY 1
#define DI_OK 1
#define DISCL_NONEXCLUSIVE 1
#define DISCL_BACKGROUND 2
#define DIJOFS_X 1
#define DIJOFS_Y 1
#define DIPH_BYOFFSET 1
#define DIPROP_RANGE 1
#define DIPROP_DEADZONE 1

extern int c_dfDIJoystick;
extern int c_dfDIMouse;
extern int c_dfDIKeyboard;
extern GUID GUID_SysMouse;
extern GUID GUID_SysKeyboard;

typedef struct {int dwSize, dwHeaderSize, dwObj, dwHow;}DIPH;

typedef struct
{
    int lMin, lMax;
    DIPH diph;
}DIPROPRANGE;

typedef struct
{
    int dwData;
    DIPH diph;
}DIPROPDWORD;

typedef struct
{
    int SetCooperativeLevel(HWND main_window_handle, int flags){return 1;}
    int SetDataFormat(int* c_dfDIJoystick){return 1;}
    int Acquire(void){return 1;}
    int Poll(void){return 1;}
    int GetDeviceState(int datasize, LPVOID joy_state){return 1;}
    void SetProperty(int flags,DIPH* diph){}
    void Unacquire(void){}
    void Release(void){}
}DIRECTINPUTDEVICE8, *LPDIRECTINPUTDEVICE8;

typedef struct
{
    GUID guidInstance;
    char* tszProductName;
}CDIDEVICEINSTANCE, *LPCDIDEVICEINSTANCE;

typedef struct
{
    void Release(void){}
    void EnumDevices(int flags1, BOOL (*func)(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr), GUID* joystickGUID, int flags2){}
    int CreateDevice(GUID joystickGUID, LPDIRECTINPUTDEVICE8* lpdijoy, void* ptr){return 1;}
}DIRECTINPUT8, *LPDIRECTINPUT8;

int DirectInput8Create(HINSTANCE main_instance, int flags, int id, void** ptr1, void* ptr2);

//------------------------direct_sound---------------------------//

#define HMMIO HANDLE

#define DSCAPS int
#define DSBCAPS	int
#define FOURCC int

#define DSBCAPS_CTRLFREQUENCY 1
#define DSBCAPS_CTRLPAN 2
#define DSBCAPS_CTRLVOLUME 3

#define MMIO_READ 1
#define MMIO_ALLOCBUF 2
#define MMIO_FINDRIFF 1
#define WAVE_FORMAT_PCM 1
#define MMIO_FINDCHUNK 1
#define DSBCAPS_STATIC 1
#define DSBCAPS_LOCSOFTWARE 2
#define DSBLOCK_FROMWRITECURSOR 1
#define DSSCL_NORMAL 1
#define DS_OK 1
#define S_OK 1
#define CLSCTX_INPROC 1
#define CLSID_DirectMusicLoader 1

#define DMUS_OBJ_CLASS 1
#define DMUS_OBJ_FILENAME 2

typedef struct
{
    int ckid, cksize, fccType, dwDataOffset, dwFlags;
}MMCKINFO;

typedef struct
{
    int dwSize, dwFlags, dwBufferBytes;
    WAVEFORMATEX* lpwfxFormat;
}DSBUFFERDESC;

typedef struct
{
    int Lock(int num, int cksize, void** audio_ptr_1, DWORD *audio_length_1, void** audio_ptr_2, DWORD *audio_length_2, int flags){return 0;}
    int Unlock(UCHAR* audio_ptr_1, DWORD audio_length_1, UCHAR* audio_ptr_2, DWORD audio_length_2){return 0;}
    int SetCurrentPosition(int pos){return 0;}
    int Play(int num1,int num2,int flags){return 0;}
    int SetVolume(int vol){return 1;}
    int SetFrequency(int freq){return 1;}
    int SetPan(int pan){return 1;}
    void GetStatus(ULONG* status){}
    void Stop(void){}
    void Release(void){}
}DIRECTSOUNDBUFFER, *LPDIRECTSOUNDBUFFER;

typedef struct
{
    int CreateSoundBuffer(DSBUFFERDESC* dsbd, LPDIRECTSOUNDBUFFER* dsbuffer, void* ptr){return 0;}
    int DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER dsbuffer1, LPDIRECTSOUNDBUFFER* dsbuffer2){return 0;}
    int SetCooperativeLevel(HWND main_window_handle, int flags){return 0;}
    void Release(void){}
}DIRECTSOUND, *LPDIRECTSOUND;

HMMIO mmioOpen(char* filename, void* ptr, int flags);
int mmioFOURCC(char c1, char c2, char c3, char c4);
int mmioDescend(HMMIO hwav, MMCKINFO* parent, void* pptr, int flags);
int mmioRead(HMMIO hwav, char * wfmtx, BOOL boolval);
int mmioAscend(HMMIO hwav, MMCKINFO* child, int num);
int DirectSoundCreate(void* ptr1, LPDIRECTSOUND* lpds, void* ptr2);
void mmioClose(HMMIO hwav, int num);

extern int GUID_DirectMusicAllTypes, CLSID_DirectMusicSegment, IID_IDirectMusicSegment, GUID_StandardMIDIFile, GUID_Download, GUID_Unload;
extern int CLSID_DirectMusicPerformance, IID_IDirectMusicPerformance, IID_IDirectMusicLoader;

typedef struct
{
    int dwSize, guidClass, dwValidData;
    WCHAR* wszFileName;
}DMUS_OBJECTDESC;

typedef struct
{
    int dm_segstate;
}IDirectMusicSegmentState;

typedef struct
{
    int dm_segment;
    HRESULT SetParam(int num1,int num2, int num3, int num4, void* ptr){return (HRESULT)1;}
    void Release(void){}
}IDirectMusicSegment;

typedef struct
{
    void PlaySegment(IDirectMusicSegment* dm_segment, int num1, int num2, IDirectMusicSegmentState** dm_segstate){}
    void Stop(IDirectMusicSegment* dm_segment, void* ptr, int num1, int num2){}
    int IsPlaying(IDirectMusicSegment* dm_segment, void* ptr){return 1;}
    int Init(void* ptr, LPDIRECTSOUND lpds, HWND main_window_handle){return 0;}
    int AddPort(void* ptr){return 0;}
    void CloseDown(void){}
    void Release(void){}
}IDirectMusicPerformance;

typedef struct
{
    HRESULT SetSearchDirectory(int num, WCHAR* wszDir, BOOL boolval){return (HRESULT)1;}
    HRESULT GetObject(DMUS_OBJECTDESC* ObjDesc,int num, void**pSegment){return (HRESULT)1;}
    void Release(void){}
}IDirectMusicLoader;

int CoInitialize(void* ptr);
int CoCreateInstance(int num1, void* ptr1, int flags, int num2, void** ptr2);
void CoUninitialize(void);

//------------------------------------------------//

#define SPI_SCREENSAVERRUNNING 1

/*#define DIK_ESCAPE 0
#define VK_ESCAPE 0
#define VK_DOWN 1
#define VK_UP 2
#define VK_RIGHT 3
#define VK_LEFT 4*/

#endif // WINSTUB_H
