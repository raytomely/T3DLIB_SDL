// DEMOII9_3_8b.CPP - 3D textured cube rotating with flat shading and lights, 8-bit version
// READ THIS!
// To compile make sure to include;
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

#define WINDOW_BPP        8    // bitdepth of window (8,16,24 etc.)
                                // note: if windowed and not
                                // fullscreen then bitdepth must
                                // be same as system bitdepth
                                // also if 8-bit the a pallete
                                // is created and attached

#define WINDOWED_APP      0 // 0 not windowed, 1 windowed

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT1_INDEX     4 // point light index
#define SPOT_LIGHT2_INDEX     3 // spot light index

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[2048];                        // used to print text

// initialize camera position and direction
POINT4D  cam_pos    = {0,0,0,1};
POINT4D  cam_target = {0,0,0,1};
VECTOR4D cam_dir    = {0,0,0,1};

// all your initialization code goes here...
VECTOR4D vscale={1.0,1.0,1.0,1},
         vpos = {0,0,0,1},
         vrot = {0,0,0,1};

CAM4DV1        cam;       // the single camera

OBJECT4DV2     obj_flat_cube;

RENDERLIST4DV2 rend_list; // the render list

RGBAV1 white, gray, black, red, green, blue;

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
    //srand(Start_Clock());
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


    // load flat shaded cube
    VECTOR4D_INITXYZ(&vscale,20.00,20.00,20.00);
    Load_OBJECT4DV2_COB(&obj_flat_cube,"cube_flat_textured_01_256.cob",
                            &vscale, &vpos, &vrot, VERTEX_FLAGS_SWAP_YZ |
                                                   VERTEX_FLAGS_TRANSFORM_LOCAL |
                                                   VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD );



    // set up lights
    Reset_Lights_LIGHTV1();

    // create some working colors
    white.rgba = _RGBA32BIT(255,255,255,0);
    gray.rgba  = _RGBA32BIT(100,100,100,0);
    black.rgba = _RGBA32BIT(0,0,0,0);
    red.rgba   = _RGBA32BIT(255,0,0,0);
    green.rgba = _RGBA32BIT(0,255,0,0);
    blue.rgba  = _RGBA32BIT(0,0,255,0);

    // ambient light
    Init_Light_LIGHTV1(AMBIENT_LIGHT_INDEX,
                       LIGHTV1_STATE_ON,      // turn the light on
                       LIGHTV1_ATTR_AMBIENT,  // ambient light type
                       gray, black, black,    // color for ambient term only
                       NULL, NULL,            // no need for pos or dir
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA

    VECTOR4D dlight_dir = {-1,0,-1,0};

    // directional light
    Init_Light_LIGHTV1(INFINITE_LIGHT_INDEX,
                       LIGHTV1_STATE_ON,      // turn the light on
                       LIGHTV1_ATTR_INFINITE, // infinite light type
                       black, gray, black,    // color for diffuse term only
                       NULL, &dlight_dir,     // need direction only
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA


    VECTOR4D plight_pos = {0,200,0,0};

    // point light
    Init_Light_LIGHTV1(POINT_LIGHT_INDEX,
                       LIGHTV1_STATE_ON,      // turn the light on
                       LIGHTV1_ATTR_POINT,    // pointlight type
                       black, green, black,   // color for diffuse term only
                       &plight_pos, NULL,     // need pos only
                       0,.001,0,              // linear attenuation only
                       0,0,1);                // spotlight info NA


    VECTOR4D slight2_pos = {0,200,0,0};
    VECTOR4D slight2_dir = {-1,0,-1,0};

    // spot light2
    Init_Light_LIGHTV1(SPOT_LIGHT2_INDEX,
                       LIGHTV1_STATE_ON,         // turn the light on
                       LIGHTV1_ATTR_SPOTLIGHT2,  // spot light type 2
                       black, red, black,      // color for diffuse term only
                       &slight2_pos, &slight2_dir, // need pos only
                       0,.001,0,                 // linear attenuation only
                       0,0,1);


    // create lookup for lighting engine
    RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
                                      palette,             // source palette
                                      rgblookup);          // lookup table

    // THIS IS ABSOLUTELY A MUST WHEN USING FLAT SHADED TEXTURES!!!!
    // YOU NEED THIS TABLE!!!!
    // create 4.4.4 to 8 bit index color lookup for texture mapping
    RGB_12_8_Lighting_Table_Builder(palette,         // source palette
                                    rgblightlookup);  // lookup table

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
    // state variables for different rendering modes and help
    static int wireframe_mode = 1;
    static int backface_mode  = 1;
    static int lighting_mode  = 1;
    static int help_mode      = 1;
    static int zsort_mode     = 1;

    char work_string[256]; // temp string

    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // read keyboard and other devices here
        read_keyboard();

        // game logic here...

        // reset the render list
        Reset_RENDERLIST4DV2(&rend_list);

        // modes and lights

        // wireframe mode
        if (keystate[KBD_W])
        {
           // toggle wireframe mode
           if (++wireframe_mode > 1)
               wireframe_mode=0;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // backface removal
        if (keystate[KBD_B])
        {
           // toggle backface removal
           backface_mode = -backface_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // lighting
        if (keystate[KBD_L])
        {
           // toggle lighting engine completely
           lighting_mode = -lighting_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // toggle ambient light
        if (keystate[KBD_A])
        {
           // toggle ambient light
           if (lights[AMBIENT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
              lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
           else
              lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // toggle infinite light
        if (keystate[KBD_I])
        {
           // toggle ambient light
           if (lights[INFINITE_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
              lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
           else
              lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // toggle point light
        if (keystate[KBD_P])
        {
           // toggle point light
           if (lights[POINT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
              lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
           else
              lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if


        // toggle spot light
        if (keystate[KBD_S])
        {
           // toggle spot light
           if (lights[SPOT_LIGHT2_INDEX].state == LIGHTV1_STATE_ON)
              lights[SPOT_LIGHT2_INDEX].state = LIGHTV1_STATE_OFF;
           else
              lights[SPOT_LIGHT2_INDEX].state = LIGHTV1_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if


        // help menu
        if (keystate[KBD_H])
        {
           // toggle help menu
           help_mode = -help_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // z-sorting
        if (keystate[KBD_Z])
        {
           // toggle z sorting
           zsort_mode = -zsort_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        static float plight_ang = 0, slight_ang = 0; // angles for light motion

        // move point light source in ellipse around game world
        lights[POINT_LIGHT_INDEX].pos.x = 1000*Fast_Cos(plight_ang);
        lights[POINT_LIGHT_INDEX].pos.y = 100;
        lights[POINT_LIGHT_INDEX].pos.z = 1000*Fast_Sin(plight_ang);

        if ((plight_ang+=3) > 360)
            plight_ang = 0;

        // move spot light source in ellipse around game world
        lights[SPOT_LIGHT2_INDEX].pos.x = 1000*Fast_Cos(slight_ang);
        lights[SPOT_LIGHT2_INDEX].pos.y = 200;
        lights[SPOT_LIGHT2_INDEX].pos.z = 1000*Fast_Sin(slight_ang);

        if ((slight_ang-=5) < 0)
            slight_ang = 360;

        // generate camera matrix
        Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

        // use these to rotate objects
        static float x_ang = 0, y_ang = 0, z_ang = 0;


        //////////////////////////////////////////////////////////////////////////
        // flat shaded textured cube

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(&obj_flat_cube);

        // set position of constant shaded cube
        obj_flat_cube.world_pos.x = 0;
        obj_flat_cube.world_pos.y = 0;
        obj_flat_cube.world_pos.z = 150;

        // generate rotation matrix around y axis
        Build_XYZ_Rotation_MATRIX4X4(x_ang, y_ang, z_ang, &mrot);

        // rotate the local coords of the object
        Transform_OBJECT4DV2(&obj_flat_cube, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(&obj_flat_cube, TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &obj_flat_cube,0);

        // update rotation angles
        if ((x_ang+=1) > 360) x_ang = 0;
        if ((y_ang+=2) > 360) y_ang = 0;
        if ((z_ang+=3) > 360) z_ang = 0;

        // remove backfaces
        if (backface_mode==1)
           Remove_Backfaces_RENDERLIST4DV2(&rend_list, &cam);

        // light scene all at once
        if (lighting_mode==1)
           Light_RENDERLIST4DV2_World(&rend_list, &cam, lights, 4);

        // apply world to camera transform
        World_To_Camera_RENDERLIST4DV2(&rend_list, &cam);

        // sort the polygon list (hurry up!)
        if (zsort_mode == 1)
           Sort_RENDERLIST4DV2(&rend_list,  SORT_POLYLIST_AVGZ);

        // apply camera to perspective transformation
        Camera_To_Perspective_RENDERLIST4DV2(&rend_list, &cam);

        // apply screen transform
        Perspective_To_Screen_RENDERLIST4DV2(&rend_list, &cam);

        sprintf(work_string,"Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, Spot=%d | Zsort [%s], BckFceRM [%s]",
                                                                                         ((lighting_mode == 1) ? "ON" : "OFF"),
                                                                                         lights[AMBIENT_LIGHT_INDEX].state,
                                                                                         lights[INFINITE_LIGHT_INDEX].state,
                                                                                         lights[POINT_LIGHT_INDEX].state,
                                                                                         lights[SPOT_LIGHT2_INDEX].state,
                                                                                         ((zsort_mode == 1) ? "ON" : "OFF"),
                                                                                         ((backface_mode == 1) ? "ON" : "OFF"));

        draw_text(work_string, 0, WINDOW_HEIGHT-34, RGB(0,255,0));

        // draw instructions
        draw_text("Press ESC to exit. Press <H> for Help.", 0, 0, RGB(0,255,0));

        // should we display help
        int text_y = 16;
        if (help_mode==1)
        {
            // draw help menu
            draw_text("<A>..............Toggle ambient light source.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<I>..............Toggle infinite light source.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<P>..............Toggle point light source.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<S>..............Toggle spot light source.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<W>..............Toggle wire frame/solid mode.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<B>..............Toggle backface removal.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<H>..............Toggle Help.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<ESC>............Exit demo.", 0, text_y+=12, RGB(255,255,255));

        } // end help

        // lock the back buffer
        lock_screen_surface();

        // reset number of polys rendered
        debug_polys_rendered_per_frame = 0;

        // render the object

        if (wireframe_mode  == 0)
           Draw_RENDERLIST4DV2_Wire(&rend_list, back_buffer, back_lpitch);
        else
        if (wireframe_mode  == 1)
           Draw_RENDERLIST4DV2_Solid(&rend_list, back_buffer, back_lpitch);

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
