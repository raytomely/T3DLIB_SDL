// DEMOII3_6.CPP - joystick demo based on code
// from volume I

// READ THIS!
// To compile make sure to include:
// the C++ source modules T3DLIB1.CPP,T3DLIB2.CPP, and T3DLIB3.CPP
// and the headers T3DLIB1.H,T3DLIB2.H, and T3DLIB3.H must
// be in the working directory of the compiler

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

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

int Start_Missile(void);
int Move_Missile(void);
int Draw_Missile(void);

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                          // used to print text


// demo globals

BITMAP_IMAGE playfield;       // used to hold playfield
BITMAP_IMAGE mushrooms[4];    // holds mushrooms
BOB          blaster;         // holds bug blaster

int blaster_anim[5] = {0,1,2,1,0};  // blinking animation

// lets use a line segment for the missle
int missile_x,              // position of missle
    missile_y,
    missile_state;          // state of missle 0 off, 1 on


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

/////////////////////////////////////////////////////////

int Start_Missile(void)
{
    // this function starts the missle, if it is currently off

    if (missile_state==0)
    {
        // enable missile
        missile_state = 1;

        // computer position of missile
        missile_x = blaster.x + 16;
        missile_y = blaster.y-4;

        // return success
        return(1);
    } // end if

    // couldn't start missile
    return(0);

} // end Start_Missile

///////////////////////////////////////////////////////////

int Move_Missile(void)
{
    // this function moves the missle

    // test if missile is alive
    if (missile_state==1)
    {
       // move the missile upward
       if ((missile_y-=10) < 0)
       {
          missile_state = 0;
          return(1);
       } // end if

       // lock secondary buffer
       lock_screen_surface();

       // add missile collision here


       // unlock surface
       unlock_screen_surface();

       // return success
       return(1);

    } // end if

    // return failure
    return(0);

} // end Move_Missle

///////////////////////////////////////////////////////////

int Draw_Missile(void)
{
    // this function draws the missile

    // test if missile is alive
    if (missile_state==1)
    {
       // lock secondary buffer
       lock_screen_surface();

       // draw the missile in green
       Draw_Clip_Line(missile_x, missile_y,
                      missile_x, missile_y+6,
                      250,back_buffer, back_lpitch);

       // unlock surface
       unlock_screen_surface();

       // return success
       return(1);

    } // end if

    // return failure
    return(0);

} // end Draw_Missle

///////////////////////////////////////////////////////////

// T3D II GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void *parms)
{
    // this function is where you do all the initialization
    // for your game

    int index; // looping var

    display_open(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

    // hide the mouse
    show_cursor(FALSE);

    // seed random number generator
    srand(get_clock());

    // load the background
    Load_Bitmap_File(&bitmap8bit, "MUSH.BMP");

    // set the palette to background image palette
    set_palette(get_palette(bitmap8bit));

    // load in the four frames of the mushroom
    for (index=0; index<4; index++)
    {
        // create mushroom bitmaps
        Create_Bitmap(&mushrooms[index],0,0,32,32);
        Load_Image_Bitmap(&mushrooms[index],&bitmap8bit,index,0,BITMAP_EXTRACT_MODE_CELL);
    } // end for index

    // now create the bug blaster bob
    Create_BOB(&blaster,0,0,32,32,3,
               BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM | BOB_ATTR_ANIM_ONE_SHOT,
               0);

    // load in the four frames of the mushroom
    for (index=0; index<3; index++)
         Load_Frame_BOB(&blaster,&bitmap8bit,index,index,1,BITMAP_EXTRACT_MODE_CELL);

    // unload the bitmap file
    Unload_Bitmap_File(&bitmap8bit);

    // set the animation sequences for bug blaster
    Load_Animation_BOB(&blaster,0,5,blaster_anim);

    // set up stating state of bug blaster
    Set_Pos_BOB(&blaster,320, 400);
    Set_Anim_Speed_BOB(&blaster,3);

    // set clipping rectangle to screen extents so objects dont
    // mess up at edges
    SURFACE_RECT screen_rect = {0,0,screen_width,screen_height};

    // create mushroom playfield bitmap
    Create_Bitmap(&playfield,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    playfield.attr |= BITMAP_ATTR_LOADED;

    // fill in the background
    Load_Bitmap_File(&bitmap8bit, "GRASS.BMP");

    // load the grass bitmap image
    Load_Image_Bitmap(&playfield,&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
    Unload_Bitmap_File(&bitmap8bit);

    // create the random mushroom patch
    for (index=0; index<50; index++)
    {
        // select a mushroom
        int mush = rand()%4;

        // set mushroom to random position
        mushrooms[mush].x = rand()%(SCREEN_WIDTH-32);
        mushrooms[mush].y = rand()%(SCREEN_HEIGHT-128);

        // now draw the mushroom into playfield
        Draw_Bitmap(&mushrooms[mush], playfield.buffer, playfield.width,1);

    } // end for

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
    // kill the bug blaster
    Destroy_BOB(&blaster);

    // kill the mushroom maker
    for (int index=0; index<4; index++)
        Destroy_Bitmap(&mushrooms[index]);

    // kill the playfield bitmap
    Destroy_Bitmap(&playfield);

    // shutdown directdraw last
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
    char joyname[10] = "heyjoy!!";

    main_loop = 1;

    while(main_loop)
    {

        // get the joystick data
        read_keyboard();
        read_joystick();

        // lock the back buffer
        lock_screen_surface();

        // draw the background reactor image
        Draw_Bitmap(&playfield, back_buffer, back_lpitch, 0);

        // unlock the back buffer
        unlock_screen_surface();

        // is the player moving?
        blaster.x+=joy_state.lX;
        blaster.y+=joy_state.lY;

        // test bounds
        if (blaster.x > SCREEN_WIDTH-32)
            blaster.x = SCREEN_WIDTH-32;
        else
        if (blaster.x < 0)
            blaster.x = 0;

        if (blaster.y > SCREEN_HEIGHT-32)
            blaster.y = SCREEN_HEIGHT-32;
        else
        if (blaster.y < SCREEN_HEIGHT-128)
            blaster.y = SCREEN_HEIGHT-128;

        // is player firing?
        if (joy_state.rgbButtons[0])
           Start_Missile();

        // move and draw missle
        Move_Missile();
        Draw_Missile();

        // is it time to blink eyes
        if ((rand()%100)==50)
           Set_Animation_BOB(&blaster,0);

        // draw blaster
        Animate_BOB(&blaster);
        Draw_BOB(&blaster,screen);

        // draw some text
        draw_text("Make My Centipede!",0,0,RGB(255,255,255));

        // display joystick and buttons 0-7
        sprintf(buffer,"Joystick Stats: X-Axis=%d, Y-Axis=%d, buttons(%d,%d,%d,%d,%d,%d,%d,%d)",
                                                                              joy_state.lX,joy_state.lY,
                                                                              joy_state.rgbButtons[0],
                                                                              joy_state.rgbButtons[1],
                                                                              joy_state.rgbButtons[2],
                                                                              joy_state.rgbButtons[3],
                                                                              joy_state.rgbButtons[4],
                                                                              joy_state.rgbButtons[5],
                                                                              joy_state.rgbButtons[6],
                                                                              joy_state.rgbButtons[7]);

        draw_text(buffer,0,SCREEN_HEIGHT-20,RGB(255,255,50));

        // print out name of joystick
        sprintf(buffer, "Joystick Name & Vendor: %s",joyname);
        draw_text(buffer,0,SCREEN_HEIGHT-40,RGB(255,255,50));


        // flip the surfaces
        show_screen();

        // sync to 30 fps
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
