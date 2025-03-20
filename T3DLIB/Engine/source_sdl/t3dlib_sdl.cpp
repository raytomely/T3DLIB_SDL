#include <SDL/SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "t3dlib_sdl.h"
#include "t3dlib1.h"

int main_loop = 1;
SDL_Event event;
SDL_Surface *screen = NULL;
Uint8 *keystate;
TTF_Font *font;
SDL_Surface *text_surf;
SDL_Color text_color = {255, 255, 255};
SDL_Rect text_pos;
MOUSESTATE mouse_state;
JOYSTATE joy_state;
int mouse_mode = MOUSE_RELATIVE_MODE;
int fps = 33;

char data_folder[512] = "t3dlib_demo/T3DIICHAP15";
char file_path[1024];

typedef struct SOUND_TYP
{
    Mix_Chunk *chunk_buffer;      // the chunk buffer containing the sound
    int state;                    // state of the sound
    int rate;                     // playback rate
    int size;                     // size of sound
    int id;                       // id number of the sound
}SOUND, *SOUND_PTR;

#define MAX_SOUNDS     256 // max number of sounds in system at once

// digital sound object state defines
#define SOUND_NULL     0 // " "
#define SOUND_LOADED   1
#define SOUND_PLAYING  2
#define SOUND_STOPPED  3

SOUND game_sounds[MAX_SOUNDS];

//////////////////////////////

typedef struct MUSIC_TYP
{
    Mix_Music *music_segment;  // the music segment
    int        id;             // the id of this segment
    int        state;          // state of midi song

}MUSIC, *MUSIC_PTR;

#define DM_NUM_SEGMENTS 64 // number of midi segments that can be cached in memory

// midi object state defines
#define MIDI_NULL     0   // this midi object is not loaded
#define MIDI_LOADED   1   // this midi object is loaded
#define MIDI_PLAYING  2   // this midi object is loaded and playing
#define MIDI_STOPPED  3   // this midi object is loaded, but stopped

MUSIC game_music[DM_NUM_SEGMENTS];

void convert_surface(SDL_Surface **surface)
{
    SDL_Surface *temp_surf = &(**surface);
    *surface = SDL_DisplayFormat (temp_surf);
    SDL_FreeSurface(temp_surf);
}

SDL_Surface *create_surface(int width, int height)
{
    SDL_Surface *surface;
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    convert_surface(&surface);
    return surface;
}

SDL_Surface *copy_surface(SDL_Surface *surface)
{
    return SDL_ConvertSurface(surface, surface->format, SDL_SWSURFACE);
}

char *getFilePath(char *filename)
{
    int len = strlen(data_folder);
    memset(file_path, 0, sizeof(file_path));
    strcpy(file_path, data_folder);
    file_path[len] = '/'; len++;
    strcpy(&file_path[len], filename);
    return file_path;
}

void font_init(void)
{
   TTF_Init();
   font = TTF_OpenFont("calibrib.ttf", 16);
}

void font_quit(void)
{
    TTF_CloseFont(font);
    TTF_Quit();
}

int get_file_size(FILE* file)
{
    fseek (file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek (file, 0, SEEK_SET);
    return file_size;
}

int load_palette(char *filename)
{
    // this function loads a palette from disk into a palette
    // structure, but does not set the pallette

    FILE *fp_file; // working file

    // try and open file
    if ((fp_file = fopen(filename,"r"))==NULL)
        return(0);

    int r,g,b,f;

    // read in all 256 colors RGBF
    for (int index=0; index<MAX_COLORS_PALETTE; index++)
    {
        // read the next entry in
        fscanf(fp_file,"%d %d %d %d",&r, &g, &b, &f);
        palette[index].peRed =r;
        palette[index].peGreen =g;
        palette[index].peBlue =b;
        palette[index].peFlags =f;
    } // end for index

    // close the file
    fclose(fp_file);

    // return success
    return(1);
} // end Load_Palette_From_Disk

void renderer_init()
{
    if(screen->format->BitsPerPixel == 16)
    {
        if(screen->format->Gshift == 5)
        {
            RGB16Bit = RGB16Bit565;
            dd_pixel_format = DD_PIXEL_FORMAT565;
        }
        else
        {
            RGB16Bit = RGB16Bit555;
            dd_pixel_format = DD_PIXEL_FORMAT555;
        }
    }
    else if(screen->format->BitsPerPixel == 8)
    {
        load_palette(DEFAULT_PALETTE_FILE);
        SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, (SDL_Color*)palette, 0, 256);
        RGB16Bit = RGB16Bit565;
        dd_pixel_format = DD_PIXEL_FORMAT565;
    }
    back_buffer = (UCHAR*)screen->pixels;
    back_lpitch = screen->pitch;
    screen_height   = screen->h;;
    screen_width    = screen->w;;
    screen_bpp      = screen->format->BitsPerPixel;;
    screen_windowed = 1;
    min_clip_x = 0;
    max_clip_x = screen_width - 1;
    min_clip_y = 0;
    max_clip_y = screen_height - 1;
}

void display_open(int width, int height, int bpp, int windowed)
{
   SDL_Init(SDL_INIT_VIDEO);
   SDL_WM_SetCaption("T3DLIB",NULL);
   screen = SDL_SetVideoMode(width, height, bpp, SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF);
   renderer_init();
   font_init();
}

void display_close(void)
{
   font_quit();
   SDL_Quit();
   printf("Bye! from T3DLIB SDL.\n");
   printf("Thank to Raytomely for this port.\n");
}

void draw_rectangle(int x1, int y1, int x2, int y2, int color)
{
    static SDL_Rect rect;
    rect.x = x1; rect.y = y1; rect.w = x2 - x1; rect.h = y2 - y1;
    SDL_FillRect(screen, &rect, color);
}

void load_bitmap(BITMAP_FILE_PTR bitmap, char *filename)
{
    bitmap->surface = SDL_LoadBMP(getFilePath(filename));
    int bpp = bitmap->surface->format->BitsPerPixel;
    if(bpp != 16 && bpp != 8)
        convert_surface(&bitmap->surface);
    bitmap->bitmapinfoheader.biWidth =  bitmap->surface->w;
    bitmap->bitmapinfoheader.biHeight = bitmap->surface->h;
    bitmap->bitmapinfoheader.biBitCount = bitmap->surface->format->BitsPerPixel;

    //bitmap->buffer = (UCHAR *)bitmap->surface->pixels;
}

void unload_bitmap(BITMAP_FILE_PTR bitmap)
{
    if (bitmap->surface)
    {
        // release memory
        SDL_FreeSurface(bitmap->surface);

        // reset pointer
        bitmap->surface = NULL;

    } // end if
}

int load_image_bitmap16(BITMAP_IMAGE_PTR image,  // bitmap image to load with data
                        BITMAP_FILE_PTR bitmap,  // bitmap to scan image data from
                        int cx,int cy,   // cell or absolute pos. to scan image from
                        int mode)        // if 0 then cx,cy is cell position, else
                        // cx,cy are absolute coords
{
    // this function extracts a 16-bit bitmap out of a 16-bit bitmap file

    // is this a valid bitmap
    if (!image)
        return(0);

    // must be a 16bit bitmap
    USHORT *source_ptr,   // working pointers
        *dest_ptr;

    // test the mode of extraction, cell based or absolute
    if (mode==BITMAP_EXTRACT_MODE_CELL)
    {
        // re-compute x,y
        cx = cx*(image->width+1) + 1;
        cy = cy*(image->height+1) + 1;
    } // end if

    // extract bitmap data
    source_ptr = (USHORT *)bitmap->surface->pixels +
        cy*bitmap->surface->w+cx;

    // assign a pointer to the bimap image
    dest_ptr = (USHORT *)image->buffer;

    int bytes_per_line = image->width*2;

    // iterate thru each scanline and copy bitmap
    for (int index_y=0; index_y < image->height; index_y++)
    {
        // copy next line of data to destination
        memcpy(dest_ptr, source_ptr,bytes_per_line);

        // advance pointers
        dest_ptr   += image->width;
        source_ptr += bitmap->surface->w;
    } // end for index_y

    // set state to loaded
    image->attr |= BITMAP_ATTR_LOADED;

    // return success
    return(1);

} // end Load_Image_Bitmap16

int load_image_bitmap(BITMAP_IMAGE_PTR image,  // bitmap image to load with data
                      BITMAP_FILE_PTR bitmap,  // bitmap to scan image data from
                      int cx,int cy,   // cell or absolute pos. to scan image from
                      int mode)        // if 0 then cx,cy is cell position, else
                      // cx,cy are absolute coords
{
    // this function extracts a bitmap out of a bitmap file

    // is this a valid bitmap
    if (!image)
        return(0);

    UCHAR *source_ptr,   // working pointers
        *dest_ptr;

    // test the mode of extraction, cell based or absolute
    if (mode==BITMAP_EXTRACT_MODE_CELL)
    {
        // re-compute x,y
        cx = cx*(image->width+1) + 1;
        cy = cy*(image->height+1) + 1;
    } // end if

    // extract bitmap data
    source_ptr = (UCHAR *)bitmap->surface->pixels +
        cy*bitmap->surface->w+cx;

    // assign a pointer to the bimap image
    dest_ptr = (UCHAR *)image->buffer;

    // iterate thru each scanline and copy bitmap
    for (int index_y=0; index_y<image->height; index_y++)
    {
        // copy next line of data to destination
        memcpy(dest_ptr, source_ptr,image->width);

        // advance pointers
        dest_ptr   += image->width;
        source_ptr += bitmap->surface->w;
    } // end for index_y

    // set state to loaded
    image->attr |= BITMAP_ATTR_LOADED;

    // return success
    return(1);

} // end Load_Image_Bitmap


int load_bitmap_file(BITMAP_FILE_PTR bitmap, char *filename)
{

    // this function opens a bitmap file and loads the data into bitmap

    FILE* file_handle;  // the file handle
    int index;        // looping index

    UCHAR   *temp_buffer = NULL; // used to convert 24 bit images to 16 bit

    // open the file if it exists
    if ((file_handle = fopen(filename,"r"))==NULL)
        return(0);

    // now load the bitmap file header
    //fread(&bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER), 1, file_handle);
    fread(&bitmap->bitmapfileheader,14, 1, file_handle);

    // test if this is a bitmap file
    if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
    {
        // close the file
        fclose(file_handle);

        // return error
        return(0);
    } // end if

    // now we know this is a bitmap, so read in all the sections

    // first the bitmap infoheader

    // now load the bitmap file header
    fread(&bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER), 1, file_handle);

    // now load the color palette if there is one
    if (bitmap->bitmapinfoheader.biBitCount == 8)
    {
        fread(&bitmap->palette,MAX_COLORS_PALETTE*sizeof(PALETTEENTRY), 1, file_handle);

        // now set all the flags in the palette correctly and fix the reversed
        // BGR RGBQUAD data format
        for (index=0; index < MAX_COLORS_PALETTE; index++)
        {
            // reverse the red and green fields
            int temp_color                = bitmap->palette[index].peRed;
            bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
            bitmap->palette[index].peBlue = temp_color;

            // always set the flags word to this
            bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
        } // end for index

    } // end if

    // finally the image data itself
    fseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

    // now read in the image
    if (bitmap->bitmapinfoheader.biBitCount==8 || bitmap->bitmapinfoheader.biBitCount==16)
    {
        // delete the last image if there was one
        if (bitmap->buffer)
            free(bitmap->buffer);

        // allocate the memory for the image
        if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
        {
            // close the file
            fclose(file_handle);

            // return error
            return(0);
        } // end if

        // now read it in
        fread(bitmap->buffer,bitmap->bitmapinfoheader.biSizeImage, 1, file_handle);

    } // end if
    else
        if (bitmap->bitmapinfoheader.biBitCount==24)
        {
            // allocate temporary buffer to load 24 bit image
            if (!(temp_buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
            {
                // close the file
                fclose(file_handle);

                // return error
                return(0);
            } // end if

            // allocate final 16 bit storage buffer
            if (!(bitmap->buffer=(UCHAR *)malloc(2*bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight)))
            {
                // close the file
                fclose(file_handle);

                // release working buffer
                free(temp_buffer);

                // return error
                return(0);
            } // end if

            // now read the file in
            fread(temp_buffer,bitmap->bitmapinfoheader.biSizeImage, 1, file_handle);

            // now convert each 24 bit RGB value into a 16 bit value
            for (index=0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
            {
                // build up 16 bit color word
                USHORT color;

                // build pixel based on format of directdraw surface
                if (dd_pixel_format==DD_PIXEL_FORMAT555)
                {
                    // extract RGB components (in BGR order), note the scaling
                    UCHAR blue  = (temp_buffer[index*3 + 0] >> 3),
                        green = (temp_buffer[index*3 + 1] >> 3),
                        red   = (temp_buffer[index*3 + 2] >> 3);
                    // use the 555 macro
                    color = _RGB16BIT555(red,green,blue);
                } // end if 555
                else
                    if (dd_pixel_format==DD_PIXEL_FORMAT565)
                    {
                        // extract RGB components (in BGR order), note the scaling
                        UCHAR blue  = (temp_buffer[index*3 + 0] >> 3),
                            green = (temp_buffer[index*3 + 1] >> 2),
                            red   = (temp_buffer[index*3 + 2] >> 3);

                        // use the 565 macro
                        color = _RGB16BIT565(red,green,blue);

                    } // end if 565

                    // write color to buffer
                    ((USHORT *)bitmap->buffer)[index] = color;

            } // end for index

            // finally write out the correct number of bits
            bitmap->bitmapinfoheader.biBitCount=16;

            // release working buffer
            free(temp_buffer);

        } // end if 24 bit
        else
        {
            // serious problem
            return(0);

        } // end else

#if 0
        // write the file info out
        printf("\nfilename:%s \nsize=%d \nwidth=%d \nheight=%d \nbitsperpixel=%d \ncolors=%d \nimpcolors=%d",
            filename,
            bitmap->bitmapinfoheader.biSizeImage,
            bitmap->bitmapinfoheader.biWidth,
            bitmap->bitmapinfoheader.biHeight,
            bitmap->bitmapinfoheader.biBitCount,
            bitmap->bitmapinfoheader.biClrUsed,
            bitmap->bitmapinfoheader.biClrImportant);
#endif

        // close the file
        fclose(file_handle);

        // flip the bitmap
        Flip_Bitmap(bitmap->buffer,
            bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8),
            bitmap->bitmapinfoheader.biHeight);

        // return success
        return(1);

} // end Load_Bitmap_File

void set_palette(PALETTEENTRY *palette)
{
    // first save the new palette in shadow
    memcpy(palette, palette,MAX_COLORS_PALETTE*sizeof(PALETTEENTRY));

    // now set the new palette
    SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, (SDL_Color*)palette, 0, 256);
}

void set_palette_entry(int color_index, PALETTEENTRY *color)
{
    // set the palette color in the palette
    SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, (SDL_Color*)color, color_index, color_index);

    // set data in shadow palette
    memcpy(&palette[color_index],color,sizeof(PALETTEENTRY));
}

void draw_text(char *text, int x, int y, Uint32 color)
{
    text_pos.x = x; text_pos.y = y;
    memcpy(&text_color, &color, 4);
    text_surf = TTF_RenderText_Blended(font, text, text_color);
    SDL_BlitSurface(text_surf, NULL, screen, &text_pos);
    SDL_FreeSurface(text_surf);
}

void draw_white_text(char *text, int x, int y)
{
    static SDL_Color white_color = {255, 255, 255};
    text_pos.x = x; text_pos.y = y;
    text_surf = TTF_RenderText_Blended(font, text, white_color);
    SDL_BlitSurface(text_surf, NULL, screen, &text_pos);
    SDL_FreeSurface(text_surf);
}

void blit_scaled_16b(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
    float x, y; int xd;
    SDL_Rect srect = {0, 0, src->w, src->h};
    SDL_Rect drect = {0, 0, dst->w, dst->h};
    Uint32 alpha_color = src->format->colorkey;

    if(srcrect)
    {
        srect.x = srcrect->x; srect.y = srcrect->y;
        srect.w = srcrect->w; srect.h = srcrect->h;
    }
    if(dstrect)
    {
        drect.x = dstrect->x; drect.y = dstrect->y;
        drect.w = dstrect->w; drect.h = dstrect->h;
    }

    float x_inc = (float)src->w / drect.w;
    float y_inc = (float)src->h / drect.h;

    // compute starting address in dst surface
    USHORT  *dst_buffer = (USHORT *)dst->pixels + drect.y * dst->w + drect.x;
    USHORT  *src_bitmap = (USHORT *)src->pixels + srect.y * src->w + srect.x;

    for(y = 0; y < src->h; y += y_inc)
    {
        for(x = 0, xd = 0; x < src->w; x += x_inc, xd++)
        {
            if(src_bitmap[(int)x] != alpha_color)
                dst_buffer[xd] = src_bitmap[(int)x];;
        }
        dst_buffer += dst->w;
        src_bitmap = (USHORT *)src->pixels + (int)(srect.y + y) * src->w + srect.x;
    }
}

void blit_scaled_8b(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
    float x, y; int xd;
    SDL_Rect srect = {0, 0, src->w, src->h};
    SDL_Rect drect = {0, 0, dst->w, dst->h};
    Uint32 alpha_color = src->format->colorkey;

    if(srcrect)
    {
        srect.x = srcrect->x; srect.y = srcrect->y;
        srect.w = srcrect->w; srect.h = srcrect->h;
    }
    if(dstrect)
    {
        drect.x = dstrect->x; drect.y = dstrect->y;
        drect.w = dstrect->w; drect.h = dstrect->h;
    }

    float x_inc = (float)src->w / drect.w;
    float y_inc = (float)src->h / drect.h;

    // compute starting address in dst surface
    UCHAR  *dst_buffer = (UCHAR *)dst->pixels + drect.y * dst->w + drect.x;
    UCHAR  *src_bitmap = (UCHAR *)src->pixels + srect.y * src->w + srect.x;

    for(y = 0; y < src->h; y += y_inc)
    {
        for(x = 0, xd = 0; x < src->w; x += x_inc, xd++)
        {
            if(src_bitmap[(int)x] != alpha_color)
                dst_buffer[xd] = src_bitmap[(int)x];;
        }
        dst_buffer += dst->w;
        src_bitmap = (UCHAR *)src->pixels + (int)(srect.y + y) * src->w + srect.x;
    }
}

void sound_init(void)
{
    // this function initializes the sound system
    static int first_time = 1; // used to track the first time the function
    // is entered

    // test for very first time
    if (first_time)
    {
        // clear everything out
        memset(game_sounds,0,sizeof(SOUND)*MAX_SOUNDS);

        // reset first time
        first_time = 0;

        // initializes SDL_mixer
        Mix_Init(0);
        Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

    } // end if

    // first turn all sounds off
    Mix_HaltChannel(-1);

    // initialize the sound fx array
    for (int index=0; index<MAX_SOUNDS; index++)
    {
        // test if this sound has been loaded
        if (game_sounds[index].chunk_buffer)
        {
            // release the buffer
            Mix_FreeChunk(game_sounds[index].chunk_buffer);

        } // end if

        // clear the record out
        memset(&game_sounds[index],0,sizeof(SOUND));

        // now set up the fields
        game_sounds[index].state = SOUND_NULL;
        game_sounds[index].id    = index;

    } // end for index

}

void sound_quit(void)
{
    // this function releases all the memory allocated by the sound system

    int index; // loop var

    // first turn all sounds off
    Mix_HaltChannel(-1);

    // now release all sound buffers
    for (index=0; index<MAX_SOUNDS; index++)
        if (game_sounds[index].chunk_buffer)
            Mix_FreeChunk(game_sounds[index].chunk_buffer);

    // If there is any music playing, stop it
     Mix_HaltMusic();

    // free up all the segments
    for (index = 0; index < DM_NUM_SEGMENTS; index++)
    {
        if (game_music[index].music_segment)
        {
            // Release the segment and set to null
            Mix_FreeMusic(game_music[index].music_segment);
            game_music[index].state = MIDI_NULL;
        }
    }

    // close SDL_mixer
    Mix_CloseAudio();
    Mix_Quit();
}

int load_wav(char *filename)
{
    // this function loads a .wav file
    // the function returns the id number of the sound

    int	sound_id = -1,       // id of sound to be loaded
        index;               // looping variable

        // step one: are there any open id's ?
    for (index=0; index < MAX_SOUNDS; index++)
    {
        // make sure this sound is unused
        if (game_sounds[index].state==SOUND_NULL)
        {
            sound_id = index;
            break;
        } // end if

    } // end for index

    // did we get a free id?
    if (sound_id==-1)
        return(-1);

    game_sounds[sound_id].chunk_buffer = Mix_LoadWAV(getFilePath(filename));

    if(!game_sounds[sound_id].chunk_buffer)
        return(-1);

    Mix_VolumeChunk(game_sounds[sound_id].chunk_buffer, 64);

    // set rate and size in data structure
    game_sounds[sound_id].rate  = 22050;
    game_sounds[sound_id].size  = game_sounds[sound_id].chunk_buffer->alen;
    game_sounds[sound_id].state = SOUND_LOADED;

    // return id
    return(sound_id);
}

void play_sound(int id, int loop)
{
    // this function plays a sound
    // the only parameter is the sound id

    if (game_sounds[id].chunk_buffer)
    {
        // play sound
        Mix_PlayChannel(-1, game_sounds[id].chunk_buffer, loop);
    }

}

void sound_set_volume(int sound_id, int volume)
{
    Mix_VolumeChunk(game_sounds[sound_id].chunk_buffer, (128/100)*volume);
}

int sound_playing(void)
{
    return Mix_Playing(-1);
}

int copy_sound_buffer(Mix_Chunk *source, Mix_Chunk *dest)
{
    if(source == dest) dest = NULL;
    else if(dest)Mix_FreeChunk(dest);
    dest = (Mix_Chunk*)malloc(sizeof(Mix_Chunk));
    *dest = *source;
    dest->abuf = (Uint8*)malloc(source->alen);
    memcpy(dest->abuf, source->abuf, source->alen);
    return(1);
}

int replicate_sound(int source_id)
{
    // this function replicates the sent sound and sends back the
    // id of the replicated sound, you would use this function
    // to make multiple copies of a gunshot or something that
    // you want to play multiple times simulataneously, but you
    // only want to load once

    if (source_id!=-1)
    {
        // duplicate the sound buffer
        // first hunt for an open id

        for (int id=0; id < MAX_SOUNDS; id++)
        {
            // is this sound open?
            if (game_sounds[id].state==SOUND_NULL)
            {
                // first make an identical copy
                game_sounds[id] = game_sounds[source_id];

                // now actually replicate the sound buffer
                if (!copy_sound_buffer(game_sounds[source_id].chunk_buffer,
                    game_sounds[id].chunk_buffer))
                {
                    // reset sound to NULL
                    game_sounds[id].chunk_buffer = NULL;
                    game_sounds[id].state    = SOUND_NULL;

                    // return error
                    return(-1);
                } // end if

                // now fix up id
                game_sounds[id].id = id;

                // return replicated sound
                return(id);

            } // end if found

        } // end for id

    } // end if
    else
        return(-1);

    // else failure
    return(-1);

} // end replicate_sound

int load_music(char *filename)
{
    // this function loads a midi segment

    int index; // loop var

    // look for open slot for midi segment
    int id = -1;

    for (index = 0; index < DM_NUM_SEGMENTS; index++)
    {
        // is this one open
        if (game_music[index].state == MIDI_NULL)
        {
            // validate id, but don't validate object until loaded
            id = index;
            break;
        } // end if

    } // end for index

    // found good id?
    if (id==-1)
        return(-1);

    game_music[index].music_segment = Mix_LoadMUS(getFilePath(filename));

    if(!game_music[index].music_segment)
        return(-1);

    Mix_VolumeMusic(64);

    // at this point we have MIDI loaded and a valid object

    game_music[id].state = MIDI_LOADED;

    // return id
    return(id);
}

void play_music(int id)
{
    // play sound based on id

    if (game_music[id].music_segment && game_music[id].state!=MIDI_NULL)
    {
        // if there is an active midi then stop it
        Mix_HaltMusic();

        // play music
        Mix_PlayMusic(game_music[id].music_segment, -1);
        game_music[id].state = MIDI_PLAYING;

    }

}

void read_mouse(void)
{
    static int old_x, old_y, x, y, button;
    button = SDL_GetMouseState(&x, &y);
    if(mouse_mode == MOUSE_RELATIVE_MODE)
        {mouse_state.lX = x - old_x; mouse_state.lY = y - old_y;} // relative mode
    else
        {mouse_state.lX = x; mouse_state.lY = y;}  // absolute mode
    old_x = x; old_y = y;
    mouse_state.rgbButtons[0] = (button & SDL_BUTTON(1));
    mouse_state.rgbButtons[2] = (button & SDL_BUTTON(3));
}

void read_joystick(void)
{
    joy_state.lX = 0; joy_state.lY = 0;
    if(keystate[KBD_RIGHT]) joy_state.lX = 1;
    else if(keystate[KBD_LEFT]) joy_state.lX = -1;
    if(keystate[KBD_UP]) joy_state.lY = -1;
    else if(keystate[KBD_DOWN]) joy_state.lY = 1;
    if(keystate[KBD_SPACE]) joy_state.rgbButtons[0] = 1;
    else joy_state.rgbButtons[0] = 0;
}

void T3DLIB_SDL_init(void)
{
   SDL_Init(SDL_INIT_VIDEO);
   SDL_WM_SetCaption("T3DLIB",NULL);
   screen = SDL_SetVideoMode(640, 480, 16, SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF);
   renderer_init();
   font_init();
   sound_init();
}

void T3DLIB_SDL_quit(void)
{
   font_quit();
   sound_quit();
   SDL_Quit();
   printf("Bye! from T3DLIB SDL.\n");
   printf("Thank to Raytomely for this port.\n");
}

void processInput(void)
{
    if(SDL_PollEvent(&event) == 1)
    {
        switch(event.type)
        {
            case SDL_QUIT:
                main_loop = 0;
                break;
        }
    }
    keystate = SDL_GetKeyState(NULL);
}

void sleep(void)
{
    static int old_time = 0,  actual_time = 0;
    actual_time = SDL_GetTicks();
    if (actual_time - old_time < fps) // if less than fps ms has passed
    {
        //game_time += (fps - (actual_time - old_time)) / 7;
        SDL_Delay(fps - (actual_time - old_time));
        old_time = SDL_GetTicks();
    }
    else
    {
        //game_time += (actual_time - old_time) / 7;
        old_time = actual_time;
    }
}

void sleep_ms(int ms)
{
    static int old_time = 0,  actual_time = 0;
    actual_time = SDL_GetTicks();
    if (actual_time - old_time < ms) // if less than ms has passed
    {
        SDL_Delay(ms - (actual_time - old_time));
        old_time = SDL_GetTicks();
    }
    else
    {
        old_time = actual_time;
    }
}

void init_clock(int speed)
{
    if(speed > 0)
        fps = 16 + (speed * 10);
}

void main_event_loop(void)
{
    main_loop = 1;
    while(main_loop)
    {
        processInput();
        sleep();
    }
}

