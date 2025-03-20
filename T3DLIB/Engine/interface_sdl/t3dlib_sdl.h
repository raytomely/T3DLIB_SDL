#ifndef T3DLIIB_SDL_H
#define T3DLIIB_SDL_H

#include <SDL/SDL.h>
#include <SDL_mixer.h>
//#include "winstub.h"

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

#define FALSE 0
#define TRUE 1

#define PC_NOCOLLAPSE 4
#define RGB(r,g,b) ((COLORREF)((BYTE)(r)|((BYTE)(g) << 8)|((BYTE)(b) << 16)))

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
    SDL_Surface      *surface;

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

typedef struct MOUSESTATE_TYP
{
    int lX, lY;
    UCHAR rgbButtons[3];

} MOUSESTATE;

typedef struct JOYSTATE_TYP
{
    int lX, lY;
    UCHAR rgbButtons[3];

} JOYSTATE;

typedef short SHORT;
typedef int INT;
typedef long long INT64;

extern int main_loop;
extern SDL_Surface *screen;
extern Uint8 *keystate;
extern MOUSESTATE mouse_state;
extern JOYSTATE joy_state;
extern int mouse_mode;

void T3DLIB_SDL_init(void);
void T3DLIB_SDL_quit(void);
void processInput(void);
void sleep(void);
void sleep_ms(int ms);
void init_clock(int speed);
void main_event_loop(void);
void draw_rectangle(int x1, int y1, int x2, int y2, int color);
void load_bitmap(BITMAP_FILE_PTR bitmap, char *filename);
void unload_bitmap(BITMAP_FILE_PTR bitmap);
int load_image_bitmap16(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode);
int load_image_bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode);
int load_bitmap_file(BITMAP_FILE_PTR bitmap, char *filename);
void set_palette(PALETTEENTRY *palette);
void set_palette_entry(int color_index, PALETTEENTRY *color);
void draw_text(char *text, int x, int y, Uint32 color);
void draw_white_text(char *text, int x, int y);
char *getFilePath(char *filename);
void display_open(int width, int height, int bpp, int windowed);
void display_close(void);
void font_init(void);
void font_quit(void);
void sound_init(void);
void sound_quit(void);
int load_wav(char *filename);
void play_sound(int id, int loop);
void sound_set_volume(int sound_id, int volume);
int sound_playing(void);
int replicate_sound(int source_id);
int load_music(char *filename);
void play_music(int id);
void read_mouse(void);
void read_joystick(void);
void blit_scaled_16b(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
void blit_scaled_8b(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
SDL_Surface *create_surface(int width, int height);

#define max(a,b)	((a) > (b) ? (a) : (b))
#define min(a,b)	((a) < (b) ? (a) : (b))

#define KBD_RIGHT SDLK_RIGHT
#define KBD_LEFT SDLK_LEFT
#define KBD_DOWN SDLK_DOWN
#define KBD_UP SDLK_UP
#define KBD_SPACE SDLK_SPACE
#define KBD_ESCAPE SDLK_ESCAPE
#define KBD_PAGEUP SDLK_PAGEUP
#define KBD_PAGEDOWN SDLK_PAGEDOWN
#define KBD_RETURN SDLK_RETURN
#define KBD_A SDLK_a
#define KBD_B SDLK_b
#define KBD_C SDLK_c
#define KBD_D SDLK_d
#define KBD_I SDLK_i
#define KBD_H SDLK_h
#define KBD_L SDLK_l
#define KBD_M SDLK_m
#define KBD_N SDLK_n
#define KBD_O SDLK_o
#define KBD_P SDLK_p
#define KBD_S SDLK_s
#define KBD_T SDLK_t
#define KBD_V SDLK_v
#define KBD_W SDLK_w
#define KBD_X SDLK_x
#define KBD_Y SDLK_y
#define KBD_Z SDLK_z
#define KBD_1 SDLK_1
#define KBD_2 SDLK_2
#define KBD_3 SDLK_3
#define KBD_4 SDLK_4

#define MOUSE_RELATIVE_MODE 1
#define MOUSE_ABSOLUTE_MODE 2
#define mouse_init(mode) mouse_mode = mode;

#define SURFACE SDL_Surface*
#define SURFACE_RECT SDL_Rect
#define lock_surface(surface) SDL_LockSurface(surface)
#define unlock_surface(surface) SDL_UnlockSurface(surface)
#define blit_surface(src, srcrect, dst, dstrect) SDL_BlitSurface(src, srcrect, dst, dstrect)
#define blit_surface_scaled_16b(src, srcrect, dst, dstrect) blit_scaled_16b(src, srcrect, dst, dstrect)
#define blit_surface_scaled_8b(src, srcrect, dst, dstrect) blit_scaled_8b(src, srcrect, dst, dstrect)
#define free_surface(surface) SDL_FreeSurface(surface)
#define set_color_key(surface, color) SDL_SetColorKey(surface, SDL_SRCCOLORKEY, color)

#define show_screen()  SDL_Flip(screen)
#define fill_screen(color)  SDL_FillRect(screen, NULL, color)
#define lock_screen_surface()  SDL_LockSurface(screen)
#define unlock_screen_surface()  SDL_UnlockSurface(screen)

#define get_palette(bitmap8bit) ((PALETTEENTRY*)bitmap8bit.surface->format->palette->colors)

#define read_keyboard()  processInput()
#define show_cursor(bool)  SDL_ShowCursor(bool)

#define get_clock()  SDL_GetTicks()

#define fopen(fn,mode) fopen(getFilePath((char*)fn),mode)

#endif // T3DLIIB_SDL_H
