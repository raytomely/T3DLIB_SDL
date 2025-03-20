#pragma once

//#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>
#include <conio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>
#include <wchar.h>
#include <limits.h>
#include <float.h>
#include <search.h>

#include <ctype.h>

#define FALSE 0
#define TRUE 1

#define PC_NOCOLLAPSE 4

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

typedef struct SURFACE_TYP
{
    void *pixels;
    int h, w, pitch;

} SURF, *SURFACE;

typedef struct SURFACE_RECT_TYP
{
    int x, y, w, h;
} SURFACE_RECT;

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

typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY,*LPPALETTEENTRY,*PPALETTEENTRY;

typedef struct BITMAP_FILE_TAG
{
    BITMAPFILEHEADER bitmapfileheader;
    BITMAPINFOHEADER bitmapinfoheader;
    PALETTEENTRY     palette[256];
    UCHAR            *buffer;
    SURFACE          surface;

} BITMAP_FILE, *BITMAP_FILE_PTR;

typedef struct BITMAP_IMAGE_TYP
{
    int state;          // state of bitmap
    int attr;           // attributes of bitmap
    int x,y;            // position of bitmap
    int width, height;  // size of bitmap
    int num_bytes;      // total bytes of bitmap
    int bpp;            // bits per pixel
    UCHAR *buffer;      // pixels of bitmap

} BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

char *getFilePath(char *filename);

void load_bitmap(BITMAP_FILE_PTR bitmap, char *filename);
void unload_bitmap(BITMAP_FILE_PTR bitmap);
int load_image_bitmap16(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode);

SURFACE create_surface(int w, int h);
void set_color_key(SURFACE, int value);
void free_surface(SURFACE);
void lock_surface(SURFACE);
void unlock_surface(SURFACE);
void blit_surface(SURFACE source, SURFACE_RECT *source_rect, SURFACE dest, SURFACE_RECT *dest_rect);
void blit_surface_scaled_8b(SURFACE source, SURFACE_RECT *source_rect, SURFACE dest, SURFACE_RECT *dest_rect);
void blit_surface_scaled_16b(SURFACE source, SURFACE_RECT *source_rect, SURFACE dest, SURFACE_RECT *dest_rect);

void load_image_bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int  cx, int cy, int mode);
void set_palette(PALETTEENTRY *palette);
void sleep_ms(int ms);
