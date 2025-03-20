// DEMOII8_1.CPP - texture multiplication demo
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

BITMAP_IMAGE textures[12], // holds our texture library
             temp_text;    // temporary working texture

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

    int index; // looping variable

    display_open(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

    // load in the textures
    Load_Bitmap_File(&bitmap16bit, "OMPTXT128_24.BMP");

    // now extract each 64x64x16 texture from the image
    for (int itext = 0; itext < NUM_TEXT; itext++)
    {
        // create the bitmap
        Create_Bitmap(&textures[itext],(WINDOW_WIDTH/2)-(TEXTSIZE/2),(WINDOW_HEIGHT/2)-(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
        Load_Image_Bitmap16(&textures[itext], &bitmap16bit,itext % 4,itext / 4,BITMAP_EXTRACT_MODE_CELL);
    } // end for

    // create temporary working texture (load with first texture to set loaded flags)
    Create_Bitmap(&temp_text,(WINDOW_WIDTH/2)-(TEXTSIZE/2),(WINDOW_HEIGHT/2)-(TEXTSIZE/2),TEXTSIZE,TEXTSIZE,16);
    Load_Image_Bitmap16(&temp_text, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_CELL);

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

    int          index;              // looping var
    static int   curr_texture = 0;   // currently active texture
    static float scalef       = 1.0; // texture scaling factor

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // lock back buffer and copy background into it
        lock_screen_surface();

        // copy texture into temp display texture for rendering and scaling
        Copy_Bitmap(&temp_text,0,0,  &textures[curr_texture],0,0,TEXTSIZE, TEXTSIZE);

        ///////////////////////////////////////////
        // our little image processing algorithm :)
        //Cmodulated = s*C1 = (s*r1, s*g1, s*b1)

        USHORT *pbuffer = (USHORT *)temp_text.buffer;

        // perform RGB transformation on bitmap
        for (int iy = 0; iy < temp_text.height; iy++)
            for (int ix = 0; ix < temp_text.width; ix++)
            {
                 // extract pixel
                 USHORT pixel = pbuffer[iy*temp_text.width + ix];

                 int ri,gi,bi; // used to extract the rgb values

                 // extract RGB values
                 _RGB565FROM16BIT(pixel, &ri,&gi,&bi);

                 // perform scaling operation and test for overflow
                 if ((ri = (float)ri * scalef) > 255) ri=255;
                 if ((gi = (float)gi * scalef) > 255) gi=255;
                 if ((bi = (float)bi * scalef) > 255) bi=255;

                 // rebuild RGB and test for overflow
                 // and write back to buffer
                 pbuffer[iy*temp_text.width + ix] = _RGB16BIT565(ri,gi,bi);

            } // end for ix

        ////////////////////////////////////////

        // draw texture
        Draw_Bitmap16(&temp_text, back_buffer, back_lpitch,0);

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

        // is user changing scaling factor
        if (keystate[KBD_UP])
        {
           scalef+=.01;
           if (scalef > 10)
              scalef = 10;
           sleep_ms(10);
        } // end if

        if (keystate[KBD_DOWN])
        {
           scalef-=.01;
           if (scalef < 0)
              scalef = 0;

           sleep_ms(10);
        } // end if


        // draw title
        draw_text("Use <RIGHT>/<LEFT> arrows to change texture, <UP>/<DOWN> arrows to change scale factor.",10, 10,RGB(255,255,255));
        draw_text("Press <ESC> to Exit. ",10, 32,RGB(255,255,255));


        // print stats
        sprintf(buffer,"Texture #%d, Scaling Factor: %f", curr_texture, scalef);
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

