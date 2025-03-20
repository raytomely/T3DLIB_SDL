#include "winstub.h"

//-------------windows------------//

DWORD GetTickCount(void){return 0;}
HDC BeginPaint(HWND hwnd,PAINTSTRUCT *ps){return (HDC)0;}
LRESULT DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){return (LRESULT)0;}
HICON LoadIcon(void* ptr, int flags){return (HICON)0;}
HCURSOR LoadCursor(void* ptr, int flags){return (HCURSOR)0;}
HBRUSH GetStockObject(int flags){return (HBRUSH)0;}
HWND CreateWindow(char* name, char* title, int flags, int x, int y, int w, int h, HANDLE parent, HANDLE menu, HINSTANCE hinstance, void* parms){return (HWND)0;}
HANDLE GetMenu(HWND main_window_handle){return (HANDLE)0;}
int OpenFile(char* filename, OFSTRUCT* file_data, int flags){return 1;}
int RegisterClass(WNDCLASS * winclass){return 1;}
int GetWindowStyle(HWND main_window_handle){return 1;}
int GetWindowExStyle(HWND main_window_handle){return 1;}
int PeekMessage(MSG *msg, void* ptr, int num1, int num2, int flags){return 0;}
int GetAsyncKeyState(int vk_code){return 0;}
void GetWindowRect(HWND main_window_handle, RECT *dest_rect){}
void SetTextColor(HDC xdc, COLORREF color){}
void SetBkMode(HDC xdc, int flags){}
void TextOut(HDC xdc,int x, int y,char* text,int len){}
void SetTextColor(HDC xdc, COLORREF red, COLORREF Green, COLORREF Blue){}
void _lread(int file_handle, void* data,int data_size){}
void _lclose(int file_handle){}
void EndPaint(HWND hwnd, PAINTSTRUCT *ps){}
void PostQuitMessage(int msgnum){}
void AdjustWindowRectEx(RECT *window_rect, int style, BOOL menu, int exstyle){}
void MoveWindow(HWND main_window_handle, int x, int y, int w, int h, BOOL boolvar){}
void ShowWindow(HWND main_window_handle, int flas){}
void SystemParametersInfo(int flags, BOOL boolvar, void* ptr, int num){}
void TranslateMessage(MSG *msg){}
void DispatchMessage(MSG *msg){}
void ShowCursor(BOOL boolvar){}
void PostMessage(HWND main_window_handle, int flags, int num1, int num2){}

int MultiByteToWideChar(UINT a,DWORD b,LPCSTR c,int d,LPWSTR e,int f){return 1;}

//-------------directx------------//

int DirectDrawCreateEx(int *ptr, void **ptr2, int flags, int *ptr3){return 0;}

//------------------------direct_input---------------------------//

GUID GUID_SysKeyboard;
GUID GUID_SysMouse;
int c_dfDIMouse;
int c_dfDIJoystick;
int c_dfDIKeyboard;

int DirectInput8Create(HINSTANCE main_instance, int flags, int id, void** ptr1, void* ptr2){return 0;}

//------------------------direct_sound---------------------------//

int GUID_DirectMusicAllTypes, CLSID_DirectMusicSegment, IID_IDirectMusicSegment, GUID_StandardMIDIFile, GUID_Download, GUID_Unload;
int CLSID_DirectMusicPerformance, IID_IDirectMusicPerformance, IID_IDirectMusicLoader;

HMMIO mmioOpen(char* filename, void* ptr, int flags){return (HMMIO)1;}
int mmioFOURCC(char c1, char c2, char c3, char c4){return 1;}
int mmioDescend(HMMIO hwav, MMCKINFO* parent, void* pptr, int flags){return 0;}
int mmioRead(HMMIO hwav, char * wfmtx, BOOL boolval){return 0;}
int mmioAscend(HMMIO hwav, MMCKINFO* child, int num){return 0;}
int DirectSoundCreate(void* ptr1, LPDIRECTSOUND* lpds, void* ptr2){return 0;}
int CoInitialize(void* ptr){return 0;}
int CoCreateInstance(int num1, void* ptr1, int flags, int num2, void** ptr2){return 0;}
void mmioClose(HMMIO hwav, int num){}
void CoUninitialize(void){}



