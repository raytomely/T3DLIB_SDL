// DEMOII3_4.CPP - keyboard example
// based on demo from Volume I
// system based on
// conservation of momentum and kinetic energy
// to compile make sure to include:
// the T3DLIB files

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

// DEFINES ////////////////////////////////////////////////

#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      640   // size of window
#define WINDOW_HEIGHT     480

#define WINDOW_BPP        8    // bitdepth of window (8,16,24 etc.)
                                // note: if windowed and not
                                // fullscreen then bitdepth must
                                // be same as system bitdepth
                                // also if 8-bit the a pallete
                                // is created and attached

#define WINDOWED_APP      0     // 0 not windowed, 1 windowed

// default screen size
#define SCREEN_WIDTH    640  // size of screen
#define SCREEN_HEIGHT   480
#define SCREEN_BPP      8    // bits per pixel

#define MAX_COLORS_PALETTE   256

// skelaton directions
#define SKELATON_EAST         0
#define SKELATON_NEAST        1
#define SKELATON_NORTH        2
#define SKELATON_NWEST        3
#define SKELATON_WEST         4
#define SKELATON_SWEST        5
#define SKELATON_SOUTH        6
#define SKELATON_SEAST        7

#define WALL_ANIMATION_COLOR  29

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                          // used to print text


// demo globals
BOB          skelaton;     // the player skelaton

// animation sequences for bob
int skelaton_anims[8][4] = { {0,1,0,2},
                             {0+4,1+4,0+4,2+4},
                             {0+8,1+8,0+8,2+8},
                             {0+12,1+12,0+12,2+12},
                             {0+16,1+16,0+16,2+16},
                             {0+20,1+20,0+20,2+20},
                             {0+24,1+24,0+24,2+24},
                             {0+28,1+28,0+28,2+28}, };

BOB          plasma;       // players weapon

BITMAP_IMAGE reactor;      // the background

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

    char filename[80]; // used to read files

    display_open(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

    // hide the mouse
    show_cursor(FALSE);

    // seed random number generator
    srand(get_clock());

    // all your initialization code goes here...

    ///////////////////////////////////////////////////////////

    // load the background
    Load_Bitmap_File(&bitmap8bit, "REACTOR.BMP");

    // set the palette to background image palette
    set_palette(get_palette(bitmap8bit));

    // create and load the reactor bitmap image
    Create_Bitmap(&reactor, 0,0, 640, 480);
    Load_Image_Bitmap(&reactor,&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
    Unload_Bitmap_File(&bitmap8bit);

    // now let's load in all the frames for the skelaton!!!

    // create skelaton bob
    if (!Create_BOB(&skelaton,0,0,56,72,32,
               BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,0))
       return(0);

    // load the frames in 8 directions, 4 frames each
    // each set of frames has a walk and a fire, frame sets
    // are loaded in counter clockwise order looking down
    // from a birds eys view or the x-z plane
    for (int direction = 0; direction < 8; direction++)
    {
        // build up file name
        sprintf(filename,"SKELSP%d.BMP",direction);

        // load in new bitmap file
        Load_Bitmap_File(&bitmap8bit,filename);

        Load_Frame_BOB(&skelaton,&bitmap8bit,0+direction*4,0,0,BITMAP_EXTRACT_MODE_CELL);
        Load_Frame_BOB(&skelaton,&bitmap8bit,1+direction*4,1,0,BITMAP_EXTRACT_MODE_CELL);
        Load_Frame_BOB(&skelaton,&bitmap8bit,2+direction*4,2,0,BITMAP_EXTRACT_MODE_CELL);
        Load_Frame_BOB(&skelaton,&bitmap8bit,3+direction*4,0,1,BITMAP_EXTRACT_MODE_CELL);

        // unload the bitmap file
        Unload_Bitmap_File(&bitmap8bit);

        // set the animation sequences for skelaton
        Load_Animation_BOB(&skelaton,direction,4,skelaton_anims[direction]);

    } // end for direction

    // set up stating state of skelaton
    Set_Animation_BOB(&skelaton, 0);
    Set_Anim_Speed_BOB(&skelaton, 4);
    Set_Vel_BOB(&skelaton, 0,0);
    Set_Pos_BOB(&skelaton, 0, 128);

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

    // kill the reactor
    Destroy_Bitmap(&reactor);

    // kill skelaton
    Destroy_BOB(&skelaton);

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

    int          index;             // looping var
    int          dx,dy;             // general deltas used in collision detection

    static int   player_moving = 0; // tracks player motion
    static PALETTEENTRY glow = {0,0,0,PC_NOCOLLAPSE};  // used to animation red border
    static int glow_count = 0, glow_dx = 5;

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // lock the back buffer
        lock_screen_surface();

        // draw the background reactor image
        Draw_Bitmap(&reactor, back_buffer, back_lpitch, 0);

        // unlock the back buffer
        unlock_screen_surface();

        // read keyboard and other devices here
        read_keyboard();

        // reset motion flag
        player_moving = 0;

        // test direction of motion, this is a good example of testing the keyboard
        // although the code could be optimized this is more educational

        if (keystate[KBD_RIGHT] && keystate[KBD_UP])
        {
           // move skelaton
           skelaton.x+=2;
           skelaton.y-=2;
           dx=2; dy=-2;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_NEAST)
              Set_Animation_BOB(&skelaton,SKELATON_NEAST);

        } // end if
        else
        if (keystate[KBD_LEFT] && keystate[KBD_UP])
        {
           // move skelaton
           skelaton.x-=2;
           skelaton.y-=2;
           dx=-2; dy=-2;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_NWEST)
              Set_Animation_BOB(&skelaton,SKELATON_NWEST);

        } // end if
        else
        if (keystate[KBD_LEFT] && keystate[KBD_DOWN])
        {
           // move skelaton
           skelaton.x-=2;
           skelaton.y+=2;
           dx=-2; dy=2;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_SWEST)
              Set_Animation_BOB(&skelaton,SKELATON_SWEST);

        } // end if
        else
        if (keystate[KBD_RIGHT] && keystate[KBD_DOWN])
        {
           // move skelaton
           skelaton.x+=2;
           skelaton.y+=2;
           dx=2; dy=2;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_SEAST)
              Set_Animation_BOB(&skelaton,SKELATON_SEAST);

        } // end if
        else
        if (keystate[KBD_RIGHT])
        {
           // move skelaton
           skelaton.x+=2;
           dx=2; dy=0;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_EAST)
              Set_Animation_BOB(&skelaton,SKELATON_EAST);

        } // end if
        else
        if (keystate[KBD_LEFT])
        {
           // move skelaton
           skelaton.x-=2;
           dx=-2; dy=0;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_WEST)
              Set_Animation_BOB(&skelaton,SKELATON_WEST);

        } // end if
        else
        if (keystate[KBD_UP])
        {
           // move skelaton
           skelaton.y-=2;
           dx=0; dy=-2;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_NORTH)
              Set_Animation_BOB(&skelaton,SKELATON_NORTH);

        } // end if
        else
        if (keystate[KBD_DOWN])
        {
           // move skelaton
           skelaton.y+=2;
           dx=0; dy=+2;

           // set motion flag
           player_moving = 1;

           // check animation needs to change
           if (skelaton.curr_animation != SKELATON_SOUTH)
              Set_Animation_BOB(&skelaton,SKELATON_SOUTH);

        } // end if

        // only animate if player is moving
        if (player_moving)
        {
           // animate skelaton
           Animate_BOB(&skelaton);


           // see if skelaton hit a wall

           // lock surface, so we can scan it
           lock_screen_surface();

           // call the color scanner with WALL_ANIMATION_COLOR, the color of the glowing wall
           // try to center the scan in the center of the object to make it
           // more realistic
           if (Color_Scan(skelaton.x+16, skelaton.y+16,
                          skelaton.x+skelaton.width-16, skelaton.y+skelaton.height-16,
                          WALL_ANIMATION_COLOR, WALL_ANIMATION_COLOR, back_buffer,back_lpitch))
           {
              // back the skelaton up along its last trajectory
              skelaton.x-=dx;
              skelaton.y-=dy;
           } // end if

           // done, so unlock
           unlock_screen_surface();

           // check if skelaton is off screen
           if (skelaton.x < 0 || skelaton.x > (screen_width - skelaton.width))
              skelaton.x-=dx;

           if (skelaton.y < 0 || skelaton.y > (screen_height - skelaton.height))
              skelaton.y-=dy;

        } // end if

        // draw the skelaton
        Draw_BOB(&skelaton, screen);

        // animate color
        glow.peGreen+=glow_dx;

        // test boundary
        if (glow.peGreen == 0 || glow.peGreen == 255)
           glow_dx = -glow_dx;

        set_palette_entry(WALL_ANIMATION_COLOR, &glow);

        // draw some text
        draw_text("I STILL HAVE A BONE TO PICK!",0,screen_height - 32,WALL_ANIMATION_COLOR);

        draw_text("USE ARROW KEYS TO MOVE, <ESC> TO EXIT.",0,0,RGB(32,32,32));


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
