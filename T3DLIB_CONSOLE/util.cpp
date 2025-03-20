#include "CommonHeader.h"

char data_folder[512] = "./";  //".T3DLIB/t3dlib_demo/T3DIICHAP08";
char file_path[1024];


char *getFilePath(char *filename)
{
    int len = strlen(data_folder);
    memset(file_path, 0, sizeof(file_path));
    strcpy(file_path, data_folder);
    file_path[len] = '/'; len++;
    strcpy(&file_path[len], filename);
    return file_path;
}

void load_bitmap(BITMAP_FILE_PTR bitmap, char *filename){}
void unload_bitmap(BITMAP_FILE_PTR bitmap){}
int load_image_bitmap16(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode){return 1;}

SURFACE create_surface(int w, int h){};
void set_color_key(SURFACE, int value){};
void free_surface(SURFACE){};
void lock_surface(SURFACE){};
void unlock_surface(SURFACE){};
void blit_surface(SURFACE source, SURFACE_RECT *source_rect, SURFACE dest, SURFACE_RECT *dest_rect){};
void blit_surface_scaled_8b(SURFACE source, SURFACE_RECT *source_rect, SURFACE dest, SURFACE_RECT *dest_rect){};
void blit_surface_scaled_16b(SURFACE source, SURFACE_RECT *source_rect, SURFACE dest, SURFACE_RECT *dest_rect){};

void load_image_bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int  cx, int cy, int mode){};
void set_palette(PALETTEENTRY *palette){}
void sleep_ms(int ms){}
