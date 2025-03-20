// DEMOII9_1.CPP - Gouraud triangle rendering demo 16-bit version
// READ THIS!
// To compile make sure to include:
// the C++ source modules T3DLIB1-7.CPP and the headers T3DLIB1-7.H
// in the working directory of the compiler

// INCLUDES ///////////////////////////////////////////////

#define DEBUG_ON

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
#include "T3DLIB7.h"

// DEFINES ////////////////////////////////////////////////

#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      800  // size of window
#define WINDOW_HEIGHT     600

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
                                // note: if windowed and not
                                // fullscreen then bitdepth must
                                // be same as system bitdepth
                                // also if 8-bit the a pallete
                                // is created and attached

#define WINDOWED_APP      1     // 0 not windowed, 1 windowed

// note if you make it windowed you will see a bizzare
// behavior in this demo since I am NOT clearing the
// backbuffer!

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[2048];                        // used to print text

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

// T3D II GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void *parms)
{
    // this function is where you do all the initialization
    // for your game

    int index; // looping var

    display_open(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

    // hide the mouse
    if (!WINDOWED_APP)
        show_cursor(FALSE);

    // seed random number generator
    srand(get_clock());

    Open_Error_File("ERROR.TXT");

    // initialize math engine
    Build_Sin_Cos_Tables();

    // clear the drawing surface
    fill_screen(0);

    // return success
    return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
    // this function is where you shutdown your game and
    // release all resources that you allocated

    // shut everything down

    // release all your resources created for the game here....

    Close_Error_File();

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

    char work_string[256]; // temp string

    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        read_keyboard();

        // game logic here...

        // lock the back buffer
        lock_screen_surface();

        // draw a randomly positioned gouraud triangle with 3 random vertex colors
        POLYF4DV2 face;

        // set the vertices
        face.tvlist[0].x  = (int)RAND_RANGE(0, screen_width - 1);
        face.tvlist[0].y  = (int)RAND_RANGE(0, screen_height - 1);
        face.lit_color[0] = RGB16Bit(RAND_RANGE(0,255), RAND_RANGE(0,255), RAND_RANGE(0,255));

        face.tvlist[1].x  = (int)RAND_RANGE(0, screen_width - 1);
        face.tvlist[1].y  = (int)RAND_RANGE(0, screen_height - 1);
        face.lit_color[1] = RGB16Bit(RAND_RANGE(0,255), RAND_RANGE(0,255), RAND_RANGE(0,255));

        face.tvlist[2].x  = (int)(int)RAND_RANGE(0, screen_width - 1);
        face.tvlist[2].y  = (int)(int)RAND_RANGE(0, screen_height - 1);
        face.lit_color[2] = RGB16Bit(RAND_RANGE(0,255), RAND_RANGE(0,255), RAND_RANGE(0,255));

        // draw the gouraud shaded triangle
        Draw_Gouraud_Triangle16(&face, back_buffer, back_lpitch);

        // unlock the back buffer
        unlock_screen_surface();

        // draw instructions
        draw_text("Press ESC to exit.", 0, 0, RGB(0,255,0));

        // flip the surfaces
        show_screen();

        // wait a sec to see pretty triangle
        sleep_ms(100);

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
