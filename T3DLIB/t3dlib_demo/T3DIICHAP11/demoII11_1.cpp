// DEMOII11_1.CPP - Z buffering demo
// READ THIS!
// To compile make sure to include:
// the C++ source modules T3DLIB1-9.CPP and the headers T3DLIB1-9.H
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
#include "T3DLIB8.h"
#include "T3DLIB9.h"

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

#define WINDOWED_APP      0    // 0 not windowed, 1 windowed

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT1_INDEX     4 // point light index
#define SPOT_LIGHT2_INDEX     3 // spot light index

#define CAM_DECEL            (.25)   // deceleration/friction
#define MAX_SPEED             20
#define NUM_OBJECTS           5      // number of objects system loads
#define NUM_SCENE_OBJECTS     500    // number of scenery objects
#define UNIVERSE_RADIUS       2000   // size of universe

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
         vpos = {0,0,150,1},
         vrot = {0,0,0,1};

CAM4DV1         cam;                    // the single camera
OBJECT4DV2_PTR  obj_work;               // pointer to active working object
OBJECT4DV2      obj_array[NUM_OBJECTS], // array of objects
                obj_scene;              // general scenery object

// filenames of objects to load
char *object_filenames[NUM_OBJECTS] = { "cube_flat_01.cob",
                                        "cube_gouraud_01.cob",
                                        "cube_flat_textured_01.cob",
                                        "sphere02.cob",
                                        "sphere03.cob",
                                      };

int curr_object = 2;                  // currently active object index

POINT4D         scene_objects[NUM_SCENE_OBJECTS]; // positions of scenery objects

RENDERLIST4DV2  rend_list;      // the render list
RGBAV1 white, gray, black, red, green, blue; // general colors

// physical model defines
float cam_speed  = 0;       // speed of the camera/jeep

ZBUFFERV1 zbuffer;          // out little z buffer!

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
                 15.0,        // near and far clipping planes
                 12000.0,
                 120.0,      // field of view in degrees
                 WINDOW_WIDTH,   // size of final screen viewport
                 WINDOW_HEIGHT);

    // set a scaling vector
    VECTOR4D_INITXYZ(&vscale,20.00,20.00,20.00);

    // load all the objects in
    for (int index_obj=0; index_obj < NUM_OBJECTS; index_obj++)
    {
        Load_OBJECT4DV2_COB(&obj_array[index_obj], object_filenames[index_obj],
                            &vscale, &vpos, &vrot, VERTEX_FLAGS_SWAP_YZ  |
                                                   VERTEX_FLAGS_TRANSFORM_LOCAL
                                                   /* VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD*/ );

    } // end for index_obj

    // set current object
    curr_object = 2;
    obj_work = &obj_array[curr_object];

    // load in the scenery object that we will place all over the place
    Load_OBJECT4DV2_COB(&obj_scene, "cube_gouraud_01.cob",
                            &vscale, &vpos, &vrot, VERTEX_FLAGS_SWAP_YZ  |
                                                   VERTEX_FLAGS_TRANSFORM_LOCAL
                                                   /* VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD*/ );

    // position the scenery objects randomly
    for (index = 0; index < NUM_SCENE_OBJECTS; index++)
    {
        // randomly position object
        scene_objects[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
        scene_objects[index].y = RAND_RANGE(-200, 200);
        scene_objects[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
    } // end for

    // set up lights
    Reset_Lights_LIGHTV2(lights2, MAX_LIGHTS);

    // create some working colors
    white.rgba = _RGBA32BIT(255,255,255,0);
    gray.rgba  = _RGBA32BIT(100,100,100,0);
    black.rgba = _RGBA32BIT(0,0,0,0);
    red.rgba   = _RGBA32BIT(255,0,0,0);
    green.rgba = _RGBA32BIT(0,255,0,0);
    blue.rgba  = _RGBA32BIT(0,0,255,0);

    // ambient light
    Init_Light_LIGHTV2(lights2,               // array of lights to work with
                       AMBIENT_LIGHT_INDEX,
                       LIGHTV2_STATE_ON,      // turn the light on
                       LIGHTV2_ATTR_AMBIENT,  // ambient light type
                       gray, black, black,    // color for ambient term only
                       NULL, NULL,            // no need for pos or dir
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA

    VECTOR4D dlight_dir = {-1,0,-1,1};

    // directional light
    Init_Light_LIGHTV2(lights2,               // array of lights to work with
                       INFINITE_LIGHT_INDEX,
                       LIGHTV2_STATE_ON,      // turn the light on
                       LIGHTV2_ATTR_INFINITE, // infinite light type
                       black, gray, black,    // color for diffuse term only
                       NULL, &dlight_dir,     // need direction only
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA


    VECTOR4D plight_pos = {0,200,0,1};

    // point light
    Init_Light_LIGHTV2(lights2,               // array of lights to work with
                       POINT_LIGHT_INDEX,
                       LIGHTV2_STATE_ON,      // turn the light on
                       LIGHTV2_ATTR_POINT,    // pointlight type
                       black, green, black,    // color for diffuse term only
                       &plight_pos, NULL,     // need pos only
                       0,.002,0,              // linear attenuation only
                       0,0,1);                // spotlight info NA


    VECTOR4D slight2_pos = {0,1000,0,1};
    VECTOR4D slight2_dir = {-1,0,-1,1};

    // spot light2
    Init_Light_LIGHTV2(lights2,                  // array of lights to work with
                       SPOT_LIGHT2_INDEX,
                       LIGHTV2_STATE_ON,         // turn the light on
                       LIGHTV2_ATTR_SPOTLIGHT2,  // spot light type 2
                       black, red, black,        // color for diffuse term only
                       &slight2_pos, &slight2_dir, // need pos only
                       0,.001,0,                 // linear attenuation only
                       0,0,1);


    // create lookup for lighting engine
    RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
                                      palette,             // source palette
                                      rgblookup);          // lookup table

    // create the z buffer
    Create_Zbuffer(&zbuffer,
                   WINDOW_WIDTH,
                   WINDOW_HEIGHT,
                   ZBUFFER_ATTR_32BIT);

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

    Delete_Zbuffer(&zbuffer);

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

    static float plight_ang = 0,
                 slight_ang = 0; // angles for light motion

    // use these to rotate objects
    static float x_ang = 0, y_ang = 0, z_ang = 0;

    // state variables for different rendering modes and help
    static int wireframe_mode = 1;
    static int backface_mode  = 1;
    static int lighting_mode  = 1;
    static int help_mode      = 1;
    static int zsort_mode     = 1;
    static int x_clip_mode    = 1;
    static int y_clip_mode    = 1;
    static int z_clip_mode    = 1;
    static int z_buffer_mode  = 1;
    static int display_mode    = 1;

    char work_string[256]; // temp string

    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        // draw the sky
        draw_rectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT*.5, RGB16Bit(180,180,220));

        // draw the ground
        draw_rectangle(0,WINDOW_HEIGHT*.5, WINDOW_WIDTH, WINDOW_HEIGHT, RGB16Bit(190,190,230));

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
           if (lights2[AMBIENT_LIGHT_INDEX].state == LIGHTV2_STATE_ON)
              lights2[AMBIENT_LIGHT_INDEX].state = LIGHTV2_STATE_OFF;
           else
              lights2[AMBIENT_LIGHT_INDEX].state = LIGHTV2_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // toggle infinite light
        if (keystate[KBD_I])
        {
           // toggle ambient light
           if (lights2[INFINITE_LIGHT_INDEX].state == LIGHTV2_STATE_ON)
              lights2[INFINITE_LIGHT_INDEX].state = LIGHTV2_STATE_OFF;
           else
              lights2[INFINITE_LIGHT_INDEX].state = LIGHTV2_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // toggle point light
        if (keystate[KBD_P])
        {
           // toggle point light
           if (lights2[POINT_LIGHT_INDEX].state == LIGHTV2_STATE_ON)
              lights2[POINT_LIGHT_INDEX].state = LIGHTV2_STATE_OFF;
           else
              lights2[POINT_LIGHT_INDEX].state = LIGHTV2_STATE_ON;

           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if


        // toggle spot light
        if (keystate[KBD_S])
        {
           // toggle spot light
           if (lights2[SPOT_LIGHT2_INDEX].state == LIGHTV2_STATE_ON)
              lights2[SPOT_LIGHT2_INDEX].state = LIGHTV2_STATE_OFF;
           else
              lights2[SPOT_LIGHT2_INDEX].state = LIGHTV2_STATE_ON;

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
        if (keystate[KBD_S])
        {
           // toggle z sorting
           zsort_mode = -zsort_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // z buffer
        if (keystate[KBD_Z])
        {
           // toggle z buffer
           z_buffer_mode = -z_buffer_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // display mode
        if (keystate[KBD_D])
        {
           // toggle display mode
           display_mode = -display_mode;
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if

        // forward/backward
        if (keystate[KBD_UP])
        {
           // move forward
           if ( (cam_speed+=1) > MAX_SPEED) cam_speed = MAX_SPEED;
        } // end if
        else
        if (keystate[KBD_DOWN])
        {
           // move backward
           if ((cam_speed-=1) < -MAX_SPEED) cam_speed = -MAX_SPEED;
        } // end if

        // rotate around y axis or yaw
        if (keystate[KBD_RIGHT])
        {
           cam.dir.y+=5;
        } // end if

        if (keystate[KBD_LEFT])
        {
           cam.dir.y-=5;
        } // end if


        // move to next object
        if (keystate[KBD_N])
        {
           if (++curr_object >= NUM_OBJECTS)
              curr_object = 0;

           // update pointer
           obj_work = &obj_array[curr_object];
           // wait, so keyboard doesn't bounce
           sleep_ms(100);
        } // end if


        // decelerate camera
        if (cam_speed > (CAM_DECEL) ) cam_speed-=CAM_DECEL;
        else
        if (cam_speed < (-CAM_DECEL) ) cam_speed+=CAM_DECEL;
        else
           cam_speed = 0;

        // move camera
        cam.pos.x += cam_speed*Fast_Sin(cam.dir.y);
        cam.pos.z += cam_speed*Fast_Cos(cam.dir.y);

        // move point light source in ellipse around game world
        lights2[POINT_LIGHT_INDEX].pos.x = 1000*Fast_Cos(plight_ang);
        lights2[POINT_LIGHT_INDEX].pos.y = 100;
        lights2[POINT_LIGHT_INDEX].pos.z = 1000*Fast_Sin(plight_ang);

        if ((plight_ang+=3) > 360)
            plight_ang = 0;

        // move spot light source in ellipse around game world
        lights2[SPOT_LIGHT2_INDEX].pos.x = 1000*Fast_Cos(slight_ang);
        lights2[SPOT_LIGHT2_INDEX].pos.y = 200;
        lights2[SPOT_LIGHT2_INDEX].pos.z = 1000*Fast_Sin(slight_ang);

        if ((slight_ang-=5) < 0)
            slight_ang = 360;

        obj_work->world_pos.x = cam.pos.x + 150*Fast_Sin(cam.dir.y);
        obj_work->world_pos.y = cam.pos.y + 0;
        obj_work->world_pos.z = cam.pos.z + 150*Fast_Cos(cam.dir.y);

        // generate camera matrix
        Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

        ////////////////////////////////////////////////////////
        // insert the scenery into universe
        for (index = 0; index < NUM_SCENE_OBJECTS; index++)
        {
            // reset the object (this only matters for backface and object removal)
            Reset_OBJECT4DV2(&obj_scene);

            // set position of tower
            obj_scene.world_pos.x = scene_objects[index].x;
            obj_scene.world_pos.y = scene_objects[index].y;
            obj_scene.world_pos.z = scene_objects[index].z;

            // attempt to cull object
            if (!Cull_OBJECT4DV2(&obj_scene, &cam, CULL_OBJECT_XYZ_PLANES))
            {
               MAT_IDENTITY_4X4(&mrot);

               // rotate the local coords of the object
               Transform_OBJECT4DV2(&obj_scene, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

               // perform world transform
               Model_To_World_OBJECT4DV2(&obj_scene, TRANSFORM_TRANS_ONLY);

               // insert the object into render list
               Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &obj_scene,0);

            } // end if

        } // end for

        ///////////////////////////////////////////////////////////////
        // insert the player object into universe

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(obj_work);

        // generate rotation matrix around y axis
        Build_XYZ_Rotation_MATRIX4X4(x_ang, cam.dir.y + y_ang, z_ang, &mrot);

        //MAT_IDENTITY_4X4(&mrot);

        // rotate the local coords of the object
        Transform_OBJECT4DV2(obj_work, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(obj_work, TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, obj_work,0);

        // update rotation angles
        if ((x_ang+=.2) > 360) x_ang = 0;
        if ((y_ang+=.4) > 360) y_ang = 0;
        if ((z_ang+=.8) > 360) z_ang = 0;

        // reset number of polys rendered
        debug_polys_rendered_per_frame = 0;
        debug_polys_lit_per_frame = 0;

        // remove backfaces
        if (backface_mode==1)
           Remove_Backfaces_RENDERLIST4DV2(&rend_list, &cam);

        // apply world to camera transform
        World_To_Camera_RENDERLIST4DV2(&rend_list, &cam);

        // clip the polygons themselves now
        Clip_Polys_RENDERLIST4DV2(&rend_list, &cam, ((x_clip_mode == 1) ? CLIP_POLY_X_PLANE : 0) |
                                                    ((y_clip_mode == 1) ? CLIP_POLY_Y_PLANE : 0) |
                                                    ((z_clip_mode == 1) ? CLIP_POLY_Z_PLANE : 0) );

        // light scene all at once
        if (lighting_mode==1)
        {
           Transform_LIGHTSV2(lights2, 4, &cam.mcam, TRANSFORM_LOCAL_TO_TRANS);
           Light_RENDERLIST4DV2_World2_16(&rend_list, &cam, lights2, 4);
        } // end if

        // sort the polygon list (hurry up!)
        if (zsort_mode == 1)
           Sort_RENDERLIST4DV2(&rend_list,  SORT_POLYLIST_AVGZ);

        // apply camera to perspective transformation
        Camera_To_Perspective_RENDERLIST4DV2(&rend_list, &cam);

        // apply screen transform
        Perspective_To_Screen_RENDERLIST4DV2(&rend_list, &cam);

        // lock the back buffer
        lock_screen_surface();

        // reset number of polys rendered
        debug_polys_rendered_per_frame = 0;

        // render the renderinglist
        if (wireframe_mode  == 0)
           Draw_RENDERLIST4DV2_Wire16(&rend_list, back_buffer, back_lpitch);
        else
        if (wireframe_mode  == 1)
        {
           if (z_buffer_mode == 1)
           {
               // initialize zbuffer to 16000 fixed point
               Clear_Zbuffer(&zbuffer, (16000 << FIXP16_SHIFT));
               Draw_RENDERLIST4DV2_SolidZB16(&rend_list, back_buffer, back_lpitch, (UCHAR *)zbuffer.zbuffer, WINDOW_WIDTH*4);
           }
           else
           {
               // initialize zbuffer to 32000 fixed point
               Draw_RENDERLIST4DV2_Solid16(&rend_list, back_buffer, back_lpitch);
           }

        } // end if

        // dislay z buffer graphically (sorta)
        if (display_mode==-1)
        {
            // use z buffer visualization mode
            // copy each line of the z buffer into the back buffer and translate each z pixel
            // into a color
            USHORT *screen_ptr = (USHORT *)back_buffer;
            UINT   *zb_ptr    =  (UINT *)zbuffer.zbuffer;

            for (int y = 0; y < WINDOW_HEIGHT; y++)
            {
                for (int x = 0; x < WINDOW_WIDTH; x++)
                {
                    // z buffer is 32 bit, so be carefull
                    UINT zpixel = zb_ptr[x + y*WINDOW_WIDTH];
                    zpixel = (zpixel/4096 & 0xffff);
                    screen_ptr[x + y* (back_lpitch >> 1)] = (USHORT)zpixel;
                } // end for
            } // end for y

        } // end if

        // unlock the back buffer
        unlock_screen_surface();

        sprintf(work_string,"Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, Spot=%d, BckFceRM [%s], Zsort [%s], Zbuffer [%s]",
                                                                                         ((lighting_mode == 1) ? "ON" : "OFF"),
                                                                                         lights[AMBIENT_LIGHT_INDEX].state,
                                                                                         lights[INFINITE_LIGHT_INDEX].state,
                                                                                         lights[POINT_LIGHT_INDEX].state,
                                                                                         lights[SPOT_LIGHT2_INDEX].state,
                                                                                         ((backface_mode == 1) ? "ON" : "OFF"),
                                                                                         ((zsort_mode == 1) ? "ON" : "OFF"),
                                                                                         ((z_buffer_mode == 1) ? "ON" : "OFF"));

        draw_text(work_string, 0, WINDOW_HEIGHT-34-16, RGB(0,255,0));

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
            draw_text("<N>..............Next object.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<W>..............Toggle wire frame/solid mode.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<B>..............Toggle backface removal.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<S>..............Toggle Z sorting.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<Z>..............Toggle Z buffering.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<D>..............Toggle Normal 3D display / Z buffer visualization mode.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<H>..............Toggle Help.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<ESC>............Exit demo.", 0, text_y+=12, RGB(255,255,255));

        } // end help

        sprintf(work_string,"Polys Rendered: %d, Polys lit: %d", debug_polys_rendered_per_frame, debug_polys_lit_per_frame);
        draw_text(work_string, 0, WINDOW_HEIGHT-34-16-16, RGB(0,255,0));

        sprintf(work_string,"CAM [%5.2f, %5.2f, %5.2f]",  cam.pos.x, cam.pos.y, cam.pos.z);

        draw_text(work_string, 0, WINDOW_HEIGHT-34-16-16-16, RGB(0,255,0));

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
