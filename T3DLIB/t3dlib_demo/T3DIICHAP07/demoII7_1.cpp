// DEMOII7_1.CPP
// READ THIS!
// To compile make sure to include:
// the C++ source modules T3DLIB1-5.CPP and the headers T3DLIB1-5.H
// in the working directory of the compiler

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


// DEFINES ////////////////////////////////////////////////

// defines for windows interface
#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      400   // size of window
#define WINDOW_HEIGHT     400

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
                                // note: if windowed and not
                                // fullscreen then bitdepth must
                                // be same as system bitdepth
                                // also if 8-bit the a pallete
                                // is created and attached

#define WINDOWED_APP      1     // 0 not windowed, 1 windowed


// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                         // used to print text

// initialize camera position and direction
POINT4D  cam_pos = {0,0,-100,1};
VECTOR4D cam_dir = {0,0,0,1};

// all your initialization code goes here...
VECTOR4D vscale={.5,.5,.5,1},
         vpos = {0,0,0,1},
         vrot = {0,0,0,1};

CAM4DV1        cam;                     // the single camera
RENDERLIST4DV1 rend_list;               // the single renderlist
POLYF4DV1      poly1;                   // our lonely polygon
POINT4D        poly1_pos = {0,0,100,1}; // world position of polygon

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

    display_open(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

    // hide the mouse
    if (!WINDOWED_APP)
       show_cursor(FALSE);

    // seed random number generator
    srand(get_clock());

    Open_Error_File("ERROR.TXT");

    // initialize math engine
    Build_Sin_Cos_Tables();

    // initialize a single polygon
    poly1.state  = POLY4DV1_STATE_ACTIVE;
    poly1.attr   =  0;
    poly1.color  = RGB16Bit(0,255,0);

    poly1.vlist[0].x = 0;
    poly1.vlist[0].y = 50;
    poly1.vlist[0].z = 0;
    poly1.vlist[0].w = 1;

    poly1.vlist[1].x = 50;
    poly1.vlist[1].y = -50;
    poly1.vlist[1].z = 0;
    poly1.vlist[1].w = 1;

    poly1.vlist[2].x = -50;
    poly1.vlist[2].y = -50;
    poly1.vlist[2].z = 0;
    poly1.vlist[2].w = 1;

    poly1.next = poly1.prev = NULL;

    // initialize the camera with 90 FOV, normalized coordinates
    Init_CAM4DV1(&cam,      // the camera object
                 CAM_MODEL_EULER, // euler camera model
                 &cam_pos,  // initial camera position
                 &cam_dir,  // initial camera angles
                 NULL,      // no initial target
                 50.0,      // near and far clipping planes
                 500.0,
                 90.0,      // field of view in degrees
                 WINDOW_WIDTH,   // size of final screen viewport
                 WINDOW_HEIGHT);

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

    static MATRIX4X4 mrot; // general rotation matrix
    static float ang_y = 0;      // rotation angle

    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // read keyboard and other devices here
        read_keyboard();

        // game logic here...

        // initialize the renderlist
        Reset_RENDERLIST4DV1(&rend_list);

        // insert polygon into the renderlist
        Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1);

        // generate rotation matrix around y axis
        Build_XYZ_Rotation_MATRIX4X4(0, ang_y, 0, &mrot);

        // rotate polygon slowly
        if (++ang_y >= 360.0) ang_y = 0;

        // rotate the local coords of single polygon in renderlist
        Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_ONLY);

        // perform local/model to world transform
        Model_To_World_RENDERLIST4DV1(&rend_list, &poly1_pos);

        // generate camera matrix
        Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

        // apply world to camera transform
        World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);

        // apply camera to perspective transformation
        Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

        // apply screen transform
        Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

        // draw instructions
        draw_text("Press ESC to exit.", 0, 0, RGB(0,255,0));

        // lock the back buffer
        lock_screen_surface();

        // render the polygon list
        Draw_RENDERLIST4DV1_Wire16(&rend_list, back_buffer, back_lpitch);

        // unlock the back buffer
        unlock_screen_surface();

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
