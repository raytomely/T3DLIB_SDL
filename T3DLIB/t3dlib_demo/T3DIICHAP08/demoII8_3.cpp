// DEMOII8_3.CPP - Alpha blending demo
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

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE textures1[NUM_TEXT],  // holds source texture library  1
             textures2[NUM_TEXT],  // holds source texture library  2
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
        Create_Bitmap(&textures1[itext],(WINDOW_WIDTH/2)-4*(TEXTSIZE/2),(WINDOW_HEIGHT/2)-2*(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
        Load_Image_Bitmap16(&textures1[itext], &bitmap16bit,itext % 4,itext / 4,BITMAP_EXTRACT_MODE_CELL);
    } // end for

    // create temporary working texture (load with first texture to set loaded flags)
    Create_Bitmap(&temp_text,(WINDOW_WIDTH/2)-(TEXTSIZE/2),(WINDOW_HEIGHT/2)+(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
    Load_Image_Bitmap16(&temp_text, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_CELL);

    // done, so unload the bitmap
    Unload_Bitmap_File(&bitmap16bit);


    // load in the textures
    Load_Bitmap_File(&bitmap16bit, "SEXTXT128_24.BMP");

    // now extract each 128x128x16 texture from the image
    for (itext = 0; itext < NUM_TEXT; itext++)
    {
        // create the bitmap
        Create_Bitmap(&textures2[itext],(WINDOW_WIDTH/2)+2*(TEXTSIZE/2),(WINDOW_HEIGHT/2)-2*(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
        Load_Image_Bitmap16(&textures2[itext], &bitmap16bit,itext % 4,itext / 4,BITMAP_EXTRACT_MODE_CELL);
    } // end for

    // done, so unload the bitmap
    Unload_Bitmap_File(&bitmap16bit);

    // hide the mouse
    if (!WINDOWED_APP)
       show_cursor(FALSE);

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
    static int   curr_texture1  = 0;  // source texture 1
    static int   curr_texture2  = 5;  // source texture 2
    static float alphaf        = .5;  // alpha blending factor

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // lock back buffer and copy background into it
        lock_screen_surface();

        ///////////////////////////////////////////
        // our little image processing algorithm :)

        //Pixel_dest[x][y]rgb = alpha    * pixel_source1[x][y]rgb +
        //                      (1-alpha)* pixel_source2[x][y]rgb

        USHORT *s1buffer = (USHORT *)textures1[curr_texture1].buffer;
        USHORT *s2buffer = (USHORT *)textures2[curr_texture2].buffer;
        USHORT *tbuffer  = (USHORT *)temp_text.buffer;

        // perform RGB transformation on bitmap
        for (int iy = 0; iy < temp_text.height; iy++)
            for (int ix = 0; ix < temp_text.width; ix++)
            {
                 int rs1,gs1,bs1;   // used to extract the source rgb values
                 int rs2, gs2, bs2; // light map rgb values
                 int rf,gf,bf;   // the final rgb terms

                 // extract pixel from source bitmap
                 USHORT s1pixel = s1buffer[iy*temp_text.width + ix];

                 // extract RGB values
                 _RGB565FROM16BIT(s1pixel, &rs1,&gs1,&bs1);

                 // extract pixel from lightmap bitmap
                 USHORT s2pixel = s2buffer[iy*temp_text.width + ix];

                 // extract RGB values
                 _RGB565FROM16BIT(s2pixel, &rs2,&gs2,&bs2);

                 // alpha blend them
                 rf = (alphaf * (float)rs1 ) + ((1 - alphaf) * (float)rs2 );
                 gf = (alphaf * (float)gs1 ) + ((1 - alphaf) * (float)gs2 );
                 bf = (alphaf * (float)bs1 ) + ((1 - alphaf) * (float)bs2 );

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

        Draw_Bitmap16(&textures1[curr_texture1], back_buffer, back_lpitch,0);

        Draw_Bitmap16(&textures2[curr_texture2], back_buffer, back_lpitch,0);

        // unlock back surface
        unlock_screen_surface();

        // read keyboard
        read_keyboard();

        // test if user wants to change texture
        if (keystate[KBD_RIGHT])
        {
           if (++curr_texture1 > (NUM_TEXT-1))
              curr_texture1 = (NUM_TEXT-1);

           sleep_ms(100);
        } // end if

        if (keystate[KBD_LEFT])
        {
           if (--curr_texture1 < 0)
              curr_texture1 = 0;

           sleep_ms(100);
        } // end if


        // test if user wants to change ligthmap texture
        if (keystate[KBD_UP])
        {
           if (++curr_texture2 > (NUM_TEXT-1))
              curr_texture2 = (NUM_TEXT-1);

           sleep_ms(100);
        } // end if

        if (keystate[KBD_DOWN])
        {
           if (--curr_texture2 < 0)
              curr_texture2 = 0;

           sleep_ms(100);
        } // end if

        // is user changing scaling factor
        if (keystate[KBD_PAGEUP])
        {
           alphaf+=.01;
           if (alphaf > 1)
              alphaf = 1;
           sleep_ms(10);
        } // end if

        if (keystate[KBD_PAGEDOWN])
        {
           alphaf-=.01;
           if (alphaf < 0)
              alphaf = 0;

           sleep_ms(10);
        } // end if


        // draw title
        draw_text("Use <RIGHT>/<LEFT> arrows to change texture 1.",10,4,RGB(255,255,255));
        draw_text("Use <UP>/<DOWN> arrows to change the texture 2.",10,20,RGB(255,255,255));
        draw_text("Use <PAGE UP>/<PAGE DOWN> arrows to change blending factor alpha.",10, 36,RGB(255,255,255));
        draw_text("Press <ESC> to Exit. ",10, 56,RGB(255,255,255));

        // print stats
        sprintf(buffer,"Texture 1: %d, Texture 2: %d, Blending factor: %f", curr_texture1, curr_texture2, alphaf);
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

