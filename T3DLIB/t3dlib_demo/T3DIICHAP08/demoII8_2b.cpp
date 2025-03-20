// DEMOII8_2b.CPP - texture color modulation with base texture demo
// to compile make sure to include:
// T3DLIB1.CPP,T3DLIB2.CPP,T3DLIB3.CPP..T3DLIB6.CPP
// and only run app if desktop is in 16-bit color mode

// INCLUDES ///////////////////////////////////////////////

//#include <iostream.h> // include important C/C++ stuff
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

#include "t3dlib_sdl.h"
#include "T3DLIB1.h" // game library includes
#include "T3DLIB2.h"
#include "T3DLIB3.h"
#include "T3DLIB4.h"
#include "T3DLIB5.h"
#include "T3DLIB6.h"



// DEFINES ////////////////////////////////////////////////

// setup a 640x480 16-bit windowed mode example
#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      640   // size of window
#define WINDOW_HEIGHT     480

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
                                // note: if windowed and not
                                // fullscreen then bitdepth must
                                // be same as system bitdepth
                                // also if 8-bit the a pallete
                                // is created and attached

#define WINDOWED_APP      1     // 0 not windowed, 1 windowed

#define TEXTSIZE           128 // size of texture mxm
#define NUM_TEXT           12  // number of textures

#define NUM_LMAP           4   // number of light map textures

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE textures[NUM_TEXT],   // holds our texture library
             lightmaps[NUM_LMAP],  // holds our light map textures
             temp_text;            // temporary working texture

// FUNCTIONS //////////////////////////////////////////////

// MAIN ////////////////////////////////////////////////

int main ( int argc, char** argv )
{
    // this is the main function

    // perform all game console specific initialization
    Game_Init();

    // main game processing goes here
    Game_Main();

    // shutdown game and release all resources
    Game_Shutdown();

    return(EXIT_SUCCESS);

} // end Main

// T3D GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void *parms)
{
    // this function is where you do all the initialization
    // for your game

    int index, itext; // looping variable

    display_open(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

    // load in the textures
    Load_Bitmap_File(&bitmap16bit, "OMPTXT128_24.BMP");

    // now extract each 128x128x16 texture from the image
    for (itext = 0; itext < NUM_TEXT; itext++)
    {
        // create the bitmap
        Create_Bitmap(&textures[itext],(WINDOW_WIDTH/2)-4*(TEXTSIZE/2),(WINDOW_HEIGHT/2)-2*(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
        Load_Image_Bitmap16(&textures[itext], &bitmap16bit,itext % 4,itext / 4,BITMAP_EXTRACT_MODE_CELL);
    } // end for

    // create temporary working texture (load with first texture to set loaded flags)
    Create_Bitmap(&temp_text,(WINDOW_WIDTH/2)-(TEXTSIZE/2),(WINDOW_HEIGHT/2)+(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
    Load_Image_Bitmap16(&temp_text, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_CELL);

    // done, so unload the bitmap
    Unload_Bitmap_File(&bitmap16bit);

    // now load the lightmaps

    // load in the textures
    Load_Bitmap_File(&bitmap16bit, "LIGHTMAPS128_24.BMP");

    // now extract each 128x128x16 texture from the image
    for (itext = 0; itext < NUM_LMAP; itext++)
    {
        // create the bitmap
        Create_Bitmap(&lightmaps[itext],(WINDOW_WIDTH/2)+2*(TEXTSIZE/2),(WINDOW_HEIGHT/2)-2*(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
        Load_Image_Bitmap16(&lightmaps[itext], &bitmap16bit,itext % 4,itext / 4,BITMAP_EXTRACT_MODE_CELL);
    } // end for

    // done, so unload the bitmap
    Unload_Bitmap_File(&bitmap16bit);

    // hide the mouse
    if (!WINDOWED_APP)
       show_cursor(FALSE);

    // seed random number generate
    srand(get_clock());

    // return success
    return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
    // this function is where you shutdown your game and
    // release all resources that you allocated

    // shut everything down

    display_close();

    // return success
    return(1);
} // end Game_Shutdown

//////////////////////////////////////////////////////////

int Game_Main(void *parms)
{
    // this is the workhorse of your game it will be called
    // continuously in real-time this is like main() in C
    // all the calls for you game go here!

    int          index;               // looping var
    static int   curr_texture  = 7;   // currently active texture
    static int   curr_lightmap = 1;   // current light map
    static float scalef        = .25; // texture ambient scale factor

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // lock back buffer and copy background into it
        lock_screen_surface();

        ///////////////////////////////////////////
        // our little image processing algorithm :)

        // Pixel_dest[x,y]rgb = pixel_source[x,y]rgb * ambient +
        //                      pixel_source[x,y]rgb * light_map[x,y]rgb

        USHORT *sbuffer = (USHORT *)textures[curr_texture].buffer;
        USHORT *lbuffer = (USHORT *)lightmaps[curr_lightmap].buffer;
        USHORT *tbuffer = (USHORT *)temp_text.buffer;

        // perform RGB transformation on bitmap
        for (int iy = 0; iy < temp_text.height; iy++)
            for (int ix = 0; ix < temp_text.width; ix++)
            {
                 int rs,gs,bs;   // used to extract the source rgb values
                 int rl, gl, bl; // light map rgb values
                 int rf,gf,bf;   // the final rgb terms

                 // extract pixel from source bitmap
                 USHORT spixel = sbuffer[iy*temp_text.width + ix];

                 // extract RGB values
                 _RGB565FROM16BIT(spixel, &rs,&gs,&bs);

                 // extract pixel from lightmap bitmap
                 USHORT lpixel = lbuffer[iy*temp_text.width + ix];

                 // extract RGB values
                 _RGB565FROM16BIT(lpixel, &rl,&gl,&bl);

                 // simple formula base + scale * lightmap
                 rf = ( scalef * (float)rl ) + ( (float)rs );
                 gf = ( scalef * (float)gl ) + ( (float)gs );
                 bf = ( scalef * (float)bl ) + ( (float)bs );

                 // test for overflow
                 if (rf > 255) rf=255;
                 if (gf > 255) gf=255;
                 if (bf > 255) bf=255;

                 // rebuild RGB and test for overflow
                 // and write back to buffer
                 tbuffer[iy*temp_text.width + ix] = _RGB16BIT565(rf,gf,bf);

            } // end for ix

        ////////////////////////////////////////

        // draw textures
        Draw_Bitmap16(&temp_text, back_buffer, back_lpitch,0);
        Draw_Bitmap16(&textures[curr_texture], back_buffer, back_lpitch,0);
        Draw_Bitmap16(&lightmaps[curr_lightmap], back_buffer, back_lpitch,0);

        // unlock back surface
        unlock_screen_surface();

        // read keyboard
        read_keyboard();

        // test if user wants to change texture
        if (keystate[KBD_RIGHT])
        {
           if (++curr_texture > (NUM_TEXT-1))
              curr_texture = (NUM_TEXT-1);

           sleep_ms(100);
        } // end if

        if (keystate[KBD_LEFT])
        {
           if (--curr_texture < 0)
              curr_texture = 0;

           sleep_ms(100);
        } // end if


        // test if user wants to change ligthmap texture
        if (keystate[KBD_UP])
        {
           if (++curr_lightmap > (NUM_LMAP-1))
              curr_lightmap = (NUM_LMAP-1);

           sleep_ms(100);
        } // end if

        if (keystate[KBD_DOWN])
        {
           if (--curr_lightmap < 0)
              curr_lightmap = 0;

           sleep_ms(100);
        } // end if

        // is user changing scaling factor
        if (keystate[KBD_PAGEUP])
        {
           scalef+=.01;
           if (scalef > 10)
              scalef = 10;
           sleep_ms(10);
        } // end if

        if (keystate[KBD_PAGEDOWN])
        {
           scalef-=.01;
           if (scalef < 0)
              scalef = 0;

           sleep_ms(10);
        } // end if


        // draw title
        draw_text("Use <RIGHT>/<LEFT> arrows to change texture.",10,4,RGB(255,255,255));
        draw_text("Use <UP>/<DOWN> arrows to change the light map texture.",10,20,RGB(255,255,255));
        draw_text("Use <PAGE UP>/<PAGE DOWN> arrows to change ambient scale factor.",10, 36,RGB(255,255,255));
        draw_text("Press <ESC> to Exit. ",10, 56,RGB(255,255,255));

        // print stats
        sprintf(buffer,"Texture #%d, Ligtmap #%d, Ambient Scaling Factor: %f", curr_texture, curr_lightmap, scalef);
        draw_text(buffer, 10, WINDOW_HEIGHT - 20,RGB(255,255,255));

        // flip the surfaces
        show_screen();

        // sync to 30ish fps
        sleep_ms(30);

        // check of user is trying to exit
        if (keystate[KBD_ESCAPE])
        {
            main_loop = 0;

        } // end if

    } // end while main_loop

    // return success
    return(1);

} // end Game_Main

//////////////////////////////////////////////////////////

