// DEMOII8_4_8b.CPP - solid demo (no lighting), 8-bit version
// READ THIS!
// To compile make sure to include:
// the C++ source modules T3DLIB1-6.CPP and the headers T3DLIB1-6.H
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

// DEFINES ////////////////////////////////////////////////

#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      800 // size of window
#define WINDOW_HEIGHT     600

#define WINDOW_BPP        8    // bitdepth of window (8,16,24 etc.)
                                // note: if windowed and not
                                // fullscreen then bitdepth must
                                // be same as system bitdepth
                                // also if 8-bit the a pallete
                                // is created and attached

#define WINDOWED_APP      0     // 0 not windowed, 1 windowed


// defines for the game universe
#define UNIVERSE_RADIUS   4000

#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)

// defines for objects
#define NUM_TOWERS        64 // 96
#define NUM_TANKS         32 // 24
#define TANK_SPEED        15

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[2048];                        // used to print text

// initialize camera position and direction
POINT4D  cam_pos    = {0,40,0,1};
POINT4D  cam_target = {0,0,0,1};
VECTOR4D cam_dir    = {0,0,0,1};

// all your initialization code goes here...
VECTOR4D vscale={1.0,1.0,1.0,1},
         vpos = {0,0,0,1},
         vrot = {0,0,0,1};

CAM4DV1        cam;       // the single camera

OBJECT4DV1     obj_tower,    // used to hold the master tower
               obj_tank,     // used to hold the master tank
               obj_marker,   // the ground marker
               obj_player;   // the player object

POINT4D        towers[NUM_TOWERS],
               tanks[NUM_TANKS];

RENDERLIST4DV1 rend_list; // the render list


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

    // initialize the camera with 90 FOV, normalized coordinates
    Init_CAM4DV1(&cam,      // the camera object
                 CAM_MODEL_EULER, // the euler model
                 &cam_pos,  // initial camera position
                 &cam_dir,  // initial camera angles
                 &cam_target,      // no target
                 200.0,      // near and far clipping planes
                 12000.0,
                 120.0,      // field of view in degrees
                 WINDOW_WIDTH,   // size of final screen viewport
                 WINDOW_HEIGHT);

    // load the master tank object
    VECTOR4D_INITXYZ(&vscale,0.75,0.75,0.75);
    Load_OBJECT4DV1_PLG(&obj_tank, "tank3_8b.plg",&vscale, &vpos, &vrot);

    // load player object for 3rd person view
    VECTOR4D_INITXYZ(&vscale,0.75,0.75,0.75);
    Load_OBJECT4DV1_PLG(&obj_player, "tank2_8b.plg",&vscale, &vpos, &vrot);

    // load the master tower object
    VECTOR4D_INITXYZ(&vscale,1.0, 2.0, 1.0);
    Load_OBJECT4DV1_PLG(&obj_tower, "tower1_8b.plg",&vscale, &vpos, &vrot);

    // load the master ground marker
    VECTOR4D_INITXYZ(&vscale,3.0,3.0,3.0);
    Load_OBJECT4DV1_PLG(&obj_marker, "marker1_8b.plg",&vscale, &vpos, &vrot);

    // position the tanks
    for (index = 0; index < NUM_TANKS; index++)
    {
        // randomly position the tanks
        tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
        tanks[index].y = 0; // obj_tank.max_radius;
        tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
        tanks[index].w = RAND_RANGE(0,360);
    } // end for

    // position the towers
    for (index = 0; index < NUM_TOWERS; index++)
    {
        // randomly position the tower
        towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
        towers[index].y = 0; // obj_tower.max_radius;
        towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
    } // end for

    // set up lights

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

    static MATRIX4X4 mrot;   // general rotation matrix

    // these are used to create a circling camera
    static float view_angle = 0;
    static float camera_distance = 6000;
    static VECTOR4D pos = {0,0,0,0};
    static float tank_speed;
    static float turning = 0;

    char work_string[256]; // temp string
    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // draw the sky
        draw_rectangle(0,0, WINDOW_WIDTH-1, WINDOW_HEIGHT/2, 166);

        // draw the ground
        draw_rectangle(0,WINDOW_HEIGHT/2, WINDOW_WIDTH-1, WINDOW_HEIGHT-1, 28);

        // read keyboard and other devices here
        read_keyboard();

        // game logic here...

        // reset the render list
        Reset_RENDERLIST4DV1(&rend_list);

        // allow user to move camera

        // turbo
        if (keystate[KBD_SPACE])
            tank_speed = 5*TANK_SPEED;
        else
            tank_speed = TANK_SPEED;

        // forward/backward
        if (keystate[KBD_UP])
        {
           // move forward
           cam.pos.x += tank_speed*Fast_Sin(cam.dir.y);
           cam.pos.z += tank_speed*Fast_Cos(cam.dir.y);
        } // end if

        if (keystate[KBD_DOWN])
        {
           // move backward
           cam.pos.x -= tank_speed*Fast_Sin(cam.dir.y);
           cam.pos.z -= tank_speed*Fast_Cos(cam.dir.y);
        } // end if

        // rotate
        if (keystate[KBD_RIGHT])
        {
           cam.dir.y+=3;

           // add a little turn to object
           if ((turning+=2) > 15)
              turning=15;
        } // end if

        if (keystate[KBD_LEFT])
        {
           cam.dir.y-=3;

           // add a little turn to object
           if ((turning-=2) < -15)
              turning=-15;

        } // end if
        else // center heading again
        {
           if (turning > 0)
               turning-=1;
           else
           if (turning < 0)
               turning+=1;

        } // end else

        // generate camera matrix
        Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

        // insert the player into the world
        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV1(&obj_player);

        // set position of tank
        obj_player.world_pos.x = cam.pos.x+300*Fast_Sin(cam.dir.y);
        obj_player.world_pos.y = cam.pos.y-70;
        obj_player.world_pos.z = cam.pos.z+300*Fast_Cos(cam.dir.y);

        // generate rotation matrix around y axis
        Build_XYZ_Rotation_MATRIX4X4(0, cam.dir.y+turning, 0, &mrot);

        // rotate the local coords of the object
        Transform_OBJECT4DV1(&obj_player, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV1(&obj_player, TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &obj_player,0,0);

        //////////////////////////////////////////////////////////


        // insert the tanks in the world
        for (index = 0; index < NUM_TANKS; index++)
        {
            // reset the object (this only matters for backface and object removal)
            Reset_OBJECT4DV1(&obj_tank);

            // generate rotation matrix around y axis
            Build_XYZ_Rotation_MATRIX4X4(0, tanks[index].w, 0, &mrot);

            // rotate the local coords of the object
            Transform_OBJECT4DV1(&obj_tank, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

            // set position of tank
            obj_tank.world_pos.x = tanks[index].x;
            obj_tank.world_pos.y = tanks[index].y;
            obj_tank.world_pos.z = tanks[index].z;

            // attempt to cull object
            if (!Cull_OBJECT4DV1(&obj_tank, &cam, CULL_OBJECT_XYZ_PLANES))
            {
               // if we get here then the object is visible at this world position
               // so we can insert it into the rendering list
               // perform local/model to world transform
               Model_To_World_OBJECT4DV1(&obj_tank, TRANSFORM_TRANS_ONLY);

               // insert the object into render list
               Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &obj_tank,0,0);
            } // end if

        } // end for

        ////////////////////////////////////////////////////////

        // insert the towers in the world
        for (index = 0; index < NUM_TOWERS; index++)
        {
            // reset the object (this only matters for backface and object removal)
            Reset_OBJECT4DV1(&obj_tower);

            // set position of tower
            obj_tower.world_pos.x = towers[index].x;
            obj_tower.world_pos.y = towers[index].y;
            obj_tower.world_pos.z = towers[index].z;

            // attempt to cull object
            if (!Cull_OBJECT4DV1(&obj_tower, &cam, CULL_OBJECT_XYZ_PLANES))
            {
               // if we get here then the object is visible at this world position
               // so we can insert it into the rendering list
               // perform local/model to world transform
               Model_To_World_OBJECT4DV1(&obj_tower);

               // insert the object into render list
               Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &obj_tower,0,0);

            } // end if

        } // end for

        ///////////////////////////////////////////////////////////////

        // seed number generator so that modulation of markers is always the same
        srand(13);

        // insert the ground markers into the world
        for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
            for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
            {
                // reset the object (this only matters for backface and object removal)
                Reset_OBJECT4DV1(&obj_marker);

                // set position of tower
                obj_marker.world_pos.x = RAND_RANGE(-100,100)-UNIVERSE_RADIUS+index_x*POINT_SIZE;
                obj_marker.world_pos.y = obj_marker.max_radius;
                obj_marker.world_pos.z = RAND_RANGE(-100,100)-UNIVERSE_RADIUS+index_z*POINT_SIZE;

                // attempt to cull object
                if (!Cull_OBJECT4DV1(&obj_marker, &cam, CULL_OBJECT_XYZ_PLANES))
                {
                   // if we get here then the object is visible at this world position
                   // so we can insert it into the rendering list
                   // perform local/model to world transform
                   Model_To_World_OBJECT4DV1(&obj_marker);

                   // insert the object into render list
                   Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &obj_marker,0,0);

                } // end if

            } // end for

        ////////////////////////////////////////////////////////////////////////

        // remove backfaces
        Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);

        // apply world to camera transform
        World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);

        // apply camera to perspective transformation
        Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

        // apply screen transform
        Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

        sprintf(work_string,"pos:[%f, %f, %f] heading:[%f] elev:[%f], polys[%d]",
                cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.y, cam.dir.x, debug_polys_rendered_per_frame);

        draw_text(work_string, 0, WINDOW_HEIGHT-20, RGB(0,255,0));

        // draw instructions
        draw_text("Press ESC to exit. Press Arrow Keys to Move. Space for TURBO.", 0, 0, RGB(0,255,0));

        // lock the back buffer
        lock_screen_surface();

        // reset number of polys rendered
        debug_polys_rendered_per_frame = 0;

        // render the object
        Draw_RENDERLIST4DV1_Solid(&rend_list, back_buffer, back_lpitch);

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
