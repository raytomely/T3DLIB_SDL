// DEMOII14_2.CPP - shadow demo with scaling of the shadow
// READ THIS!
// To compile make sure to include:
// the C++ source modules T3DLIB1-12.CPP and the headers T3DLIB1-12.H
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
#include "T3DLIB10.h"
#include "T3DLIB11.h"
#include "T3DLIB12.h"

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

#define WINDOWED_APP      0 // 0 not windowed, 1 windowed

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define POINT_LIGHT2_INDEX    3 // point light index

// terrain defines
#define TERRAIN_WIDTH         4000
#define TERRAIN_HEIGHT        4000
#define TERRAIN_SCALE         700
#define MAX_SPEED             20

#define PITCH_RETURN_RATE (.33) // how fast the pitch straightens out
#define PITCH_CHANGE_RATE (.02) // the rate that pitch changes relative to height
#define CAM_HEIGHT_SCALER (.3)  // percentage cam height changes relative to height
#define VELOCITY_SCALER (.025)  // rate velocity changes with height
#define CAM_DECEL       (.25)   // deceleration/friction


#define NUM_OBJECTS          3 // number of objects per class

#define NUM_LIGHT_OBJECTS    5 // number of light models

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[2048];                        // used to print text

// initialize camera position and direction
POINT4D  cam_pos    = {0,500,-400,1};
POINT4D  cam_target = {0,0,0,1};
VECTOR4D cam_dir    = {0,0,0,1};

// all your initialization code goes here...
VECTOR4D vscale={1.0,1.0,1.0,1},
         vpos = {0,0,150,1},
         vrot = {0,0,0,1};

CAM4DV1         cam;            // the single camera
OBJECT4DV2      obj_terrain;    // the terrain object

OBJECT4DV2_PTR  obj_work;
OBJECT4DV2      obj_array[NUM_OBJECTS];

OBJECT4DV2_PTR  obj_light;
OBJECT4DV2      obj_light_array[NUM_LIGHT_OBJECTS];

OBJECT4DV2      shadow_obj;

// filenames of objects to load
char *object_filenames[NUM_OBJECTS] = {
                                        "earth01.cob",
                                        "sphere_gouraud_textured_02.cob",
                                        "fire_constant_cube01.cob",
                                      };

int curr_object  = 0;

#define INDEX_RED_LIGHT_INDEX       0
#define INDEX_GREEN_LIGHT_INDEX     1
#define INDEX_BLUE_LIGHT_INDEX      2
#define INDEX_YELLOW_LIGHT_INDEX    3
#define INDEX_WHITE_LIGHT_INDEX     4

// filenames of objects to load to represent light sources
char *object_light_filenames[NUM_LIGHT_OBJECTS] = {
                                            "cube_constant_red_01.cob",
                                            "cube_constant_green_01.cob",
                                            "cube_constant_blue_01.cob",
                                            "cube_constant_yellow_01.cob",
                                            "cube_constant_white_01.cob",
                                            };
int curr_light_object  = 0;

RENDERLIST4DV2  rend_list;      // the render list

RGBAV1          white,          // general colors
                gray,
                black,
                red,
                green,
                blue,
                yellow,
                orange;

ZBUFFERV1 zbuffer;   // our little z buffer!
RENDERCONTEXTV1  rc; // the rendering context;

// physical model defines play with these to change the feel of the vehicle
float gravity       = -.40;    // general gravity
float vel_y         = 0;       // the y velocity of camera/jeep
float cam_speed     = 0;       // speed of the camera/jeep
float sea_level     = 50;      // sea level of the simulation
float gclearance    = 125;      // clearance from the camera to the ground
float neutral_pitch = 10;   // the neutral pitch of the camera

// sounds
int wind_sound_id = -1;

// game imagery assets
BOB cockpit;               // the cockpit image

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

    sound_init();

    // hide the mouse
    if (!WINDOWED_APP)
        show_cursor(FALSE);

    // seed random number generator
    srand(get_clock());

    Open_Error_File("ERROR.TXT");

    // initialize math engine
    Build_Sin_Cos_Tables();

    // initialize the camera with 90 FOV, normalized coordinates
    Init_CAM4DV1(&cam,            // the camera object
                 CAM_MODEL_EULER, // the euler model
                 &cam_pos,        // initial camera position
                 &cam_dir,        // initial camera angles
                 &cam_target,     // no target
                 10.0,            // near and far clipping planes
                 12000.0,
                 90.0,            // field of view in degrees
                 WINDOW_WIDTH,    // size of final screen viewport
                 WINDOW_HEIGHT);

    VECTOR4D terrain_pos = {0,0,0,0};

    Generate_Terrain_OBJECT4DV2(&obj_terrain,            // pointer to object
                                TERRAIN_WIDTH,           // width in world coords on x-axis
                                TERRAIN_HEIGHT,          // height (length) in world coords on z-axis
                                TERRAIN_SCALE,           // vertical scale of terrain
                                "height_grass_40_40_01.bmp",  // filename of height bitmap encoded in 256 colors
                                "grass256_256_01.bmp", //"checker2562562.bmp",   // filename of texture map
                                 RGB16Bit(255,255,255),  // color of terrain if no texture
                                 &terrain_pos,           // initial position
                                 NULL,                   // initial rotations
                                 POLY4DV2_ATTR_RGB16
                                 //| POLY4DV2_ATTR_SHADE_MODE_FLAT
                                 | POLY4DV2_ATTR_SHADE_MODE_GOURAUD
                                 | POLY4DV2_ATTR_SHADE_MODE_TEXTURE);

    // set a scaling vector
    VECTOR4D_INITXYZ(&vscale, 40, 40, 40);

    // load all the objects in
    for (int index_obj=0; index_obj < NUM_OBJECTS; index_obj++)
    {
        Load_OBJECT4DV2_COB2(&obj_array[index_obj], object_filenames[index_obj],
                             &vscale, &vpos, &vrot, VERTEX_FLAGS_INVERT_WINDING_ORDER
                             | VERTEX_FLAGS_TRANSFORM_LOCAL
                            | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD
                                                   ,0 );

        // angle for circular rotation
        obj_array[index_obj].ivar1 = 0;

        // set initial position
        obj_array[index_obj].world_pos.x = 0;
        obj_array[index_obj].world_pos.y = 200;
        obj_array[index_obj].world_pos.z = 0;

    } // end for index_obj

    // set current object
    curr_object = 0;
    obj_work    = &obj_array[curr_object];

    // set a scaling vector
    VECTOR4D_INITXYZ(&vscale, 20, 20, 20);

    // load all the light objects in
    for (int index_obj=0; index_obj < NUM_LIGHT_OBJECTS; index_obj++)
    {
        Load_OBJECT4DV2_COB2(&obj_light_array[index_obj], object_light_filenames[index_obj],
                             &vscale, &vpos, &vrot, VERTEX_FLAGS_INVERT_WINDING_ORDER
                             | VERTEX_FLAGS_TRANSFORM_LOCAL
                             | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD
                             ,0 );
    } // end for index

    // set current object
    curr_light_object = 0;
    obj_light    = &obj_light_array[curr_light_object];

    // load the shadow object in, basically a single plane with a circular
    // texture on it for the shadow
    VECTOR4D_INITXYZ(&vscale, 1, 1, 1);
    Load_OBJECT4DV2_COB2(&shadow_obj,"shadow_poly_01.cob",
                         &vscale, &vpos, &vrot, VERTEX_FLAGS_INVERT_WINDING_ORDER
                         | VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_TRANSFORM_LOCAL
                         | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD
                         ,0);

    // set up lights
    Reset_Lights_LIGHTV2(lights2, MAX_LIGHTS);

    // create some working colors
    white.rgba = _RGBA32BIT(255,255,255,0);
    gray.rgba  = _RGBA32BIT(100,100,100,0);
    black.rgba = _RGBA32BIT(0,0,0,0);
    red.rgba   = _RGBA32BIT(255,0,0,0);
    green.rgba = _RGBA32BIT(0,255,0,0);
    blue.rgba  = _RGBA32BIT(0,0,255,0);
    orange.rgba = _RGBA32BIT(255,128,0,0);
    yellow.rgba  = _RGBA32BIT(255,255,0,0);

    // ambient light
    Init_Light_LIGHTV2(lights2,
                       AMBIENT_LIGHT_INDEX,
                       LIGHTV2_STATE_ON,      // turn the light on
                       LIGHTV2_ATTR_AMBIENT,  // ambient light type
                       gray, black, black,    // color for ambient term only
                       NULL, NULL,            // no need for pos or dir
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA

    VECTOR4D dlight_dir = {-1,1,-1,1};

    // directional light
    Init_Light_LIGHTV2(lights2,
                       INFINITE_LIGHT_INDEX,
                       LIGHTV2_STATE_ON,      // turn the light on
                       LIGHTV2_ATTR_INFINITE, // infinite light type
                       black, gray, black,    // color for diffuse term only
                       NULL, &dlight_dir,     // need direction only
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA

    VECTOR4D plight_pos = {0,500,0,1};

    // point light
    Init_Light_LIGHTV2(lights2,
                       POINT_LIGHT_INDEX,
                       LIGHTV2_STATE_ON,      // turn the light on
                       LIGHTV2_ATTR_POINT,    // pointlight type
                       black, green, black,   // color for diffuse term only
                       &plight_pos, NULL,     // need pos only
                       0,.001,0,              // linear attenuation only
                       0,0,1);                // spotlight info NA

    // point light
    Init_Light_LIGHTV2(lights2,
                       POINT_LIGHT2_INDEX,
                       LIGHTV2_STATE_ON,     // turn the light on
                       LIGHTV2_ATTR_POINT,   // pointlight type
                       black, white, black,  // color for diffuse term only
                       &plight_pos, NULL,    // need pos only
                       0,.002,0,             // linear attenuation only
                       0,0,1);               // spotlight info NA

    VECTOR4D slight2_pos = {0,200,0,1};
    VECTOR4D slight2_dir = {-1,1,-1,1};


    // create lookup for lighting engine
    RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
                                      palette,             // source palette
                                      rgblookup);          // lookup table

    // create the z buffer
    Create_Zbuffer(&zbuffer,
                   WINDOW_WIDTH,
                   WINDOW_HEIGHT,
                   ZBUFFER_ATTR_32BIT);


    // build alpha lookup table
    RGB_Alpha_Table_Builder(NUM_ALPHA_LEVELS, rgb_alpha_table);

    // load background sounds
    wind_sound_id = load_wav("STATIONTHROB.WAV");

    // start the sounds
    play_sound(wind_sound_id, -1);
    sound_set_volume(wind_sound_id, 100);

    #if 0
    // load in the cockpit image
    Load_Bitmap_File(&bitmap16bit, "lego01.BMP");
    Create_BOB(&cockpit, 0,0,800,600,1, BOB_ATTR_VISIBLE | BOB_ATTR_SINGLE_FRAME, DDSCAPS_SYSTEMMEMORY, 0, 16);
    Load_Frame_BOB16(&cockpit, &bitmap16bit,0,0,0,BITMAP_EXTRACT_MODE_ABS);
    Unload_Bitmap_File(&bitmap16bit);
    #endif

    // set single precission
    //_control87( _PC_24, MCW_PC );

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

    sound_quit();

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

    static float plight_ang = 0,
                 slight_ang = 0; // angles for light motion

    // use these to rotate objects
    static float x_ang = 0, y_ang = 0, z_ang = 0;

    // state variables for different rendering modes and help
    static int wireframe_mode   = 1;
    static int backface_mode    = 1;
    static int lighting_mode    = 1;
    static int help_mode        = 1;
    static int zsort_mode       = 1;
    static int x_clip_mode      = 1;
    static int y_clip_mode      = 1;
    static int z_clip_mode      = 1;

    static float hl = 300, // artificial light height
                 ks = 1.0; // generic scaling factor to make things look good

    char work_string[256]; // temp string

    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // draw the sky
        draw_rectangle(0,0, WINDOW_WIDTH-1, WINDOW_HEIGHT-1, RGB16Bit(50,50,200));

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

           // toggle point light
           if (lights2[POINT_LIGHT2_INDEX].state == LIGHTV2_STATE_ON)
              lights2[POINT_LIGHT2_INDEX].state = LIGHTV2_STATE_OFF;
           else
              lights2[POINT_LIGHT2_INDEX].state = LIGHTV2_STATE_ON;

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

        // move to next object
        if (keystate[KBD_O])
        {
           VECTOR4D old_pos;
           old_pos = obj_work->world_pos;

           if (++curr_object >= NUM_OBJECTS)
              curr_object = 0;

           // update pointer
           obj_work = &obj_array[curr_object];
           obj_work->world_pos = old_pos;

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

        // change height of point light source 1
        if (keystate[KBD_1])
        {
           lights2[POINT_LIGHT_INDEX].pos.y-=10;
        } // end if

        if (keystate[KBD_2])
        {
           lights2[POINT_LIGHT_INDEX].pos.y+=10;
        } // end if

        // motion section /////////////////////////////////////////////////////////

        // terrain following, simply find the current cell we are over and then
        // index into the vertex list and find the 4 vertices that make up the
        // quad cell we are hovering over and then average the values, and based
        // on the current height and the height of the terrain push the player upward

        // the terrain generates and stores some results to help with terrain following
        //ivar1 = columns;
        //ivar2 = rows;
        //fvar1 = col_vstep;
        //fvar2 = row_vstep;

        int cell_x = (cam.pos.x  + TERRAIN_WIDTH/2) / obj_terrain.fvar1;
        int cell_y = (cam.pos.z  + TERRAIN_HEIGHT/2) / obj_terrain.fvar1;

        static float terrain_height, delta;

        // test if we are on terrain
        if ( (cell_x >=0) && (cell_x < obj_terrain.ivar1) && (cell_y >=0) && (cell_y < obj_terrain.ivar2) )
        {
           // compute vertex indices into vertex list of the current quad
           int v0 = cell_x + cell_y*obj_terrain.ivar2;
           int v1 = v0 + 1;
           int v2 = v1 + obj_terrain.ivar2;
           int v3 = v0 + obj_terrain.ivar2;

           // now simply index into table
           terrain_height = 0.25 * (obj_terrain.vlist_trans[v0].y + obj_terrain.vlist_trans[v1].y +
                                    obj_terrain.vlist_trans[v2].y + obj_terrain.vlist_trans[v3].y);

           // compute height difference
           delta = terrain_height - (cam.pos.y - gclearance);

           // test for penetration
           if (delta > 0)
           {
              // apply force immediately to camera (this will give it a springy feel)
              vel_y+=(delta * (VELOCITY_SCALER));

              // test for pentration, if so move up immediately so we don't penetrate geometry
              cam.pos.y+=(delta*CAM_HEIGHT_SCALER);

              // now this is more of a hack than the physics model :) let move the front
              // up and down a bit based on the forward velocity and the gradient of the
              // hill
              cam.dir.x -= (delta*PITCH_CHANGE_RATE);

           } // end if

        } // end if

        // decelerate camera
        if (cam_speed > (CAM_DECEL) ) cam_speed-=CAM_DECEL;
        else
        if (cam_speed < (-CAM_DECEL) ) cam_speed+=CAM_DECEL;
        else
           cam_speed = 0;

        // force camera to seek a stable orientation
        if (cam.dir.x > (neutral_pitch+PITCH_RETURN_RATE)) cam.dir.x -= (PITCH_RETURN_RATE);
        else
        if (cam.dir.x < (neutral_pitch-PITCH_RETURN_RATE)) cam.dir.x += (PITCH_RETURN_RATE);
         else
           cam.dir.x = neutral_pitch;

        // apply gravity
        vel_y+=gravity;

        // test for absolute sea level and push upward..
        if (cam.pos.y < sea_level)
        {
           vel_y = 0;
           cam.pos.y = sea_level;
        } // end if

        // move camera
        cam.pos.x += cam_speed*Fast_Sin(cam.dir.y);
        cam.pos.z += cam_speed*Fast_Cos(cam.dir.y);
        cam.pos.y += vel_y;

        // move point light source in ellipse around game world in x-z plane
        lights2[POINT_LIGHT_INDEX].pos.x = 1000*Fast_Cos(plight_ang);
        //lights2[POINT_LIGHT_INDEX].pos.y = 200;
        lights2[POINT_LIGHT_INDEX].pos.z = 1000*Fast_Sin(plight_ang);

        // move point light source in ellipse around game world in x-z plane
        lights2[POINT_LIGHT2_INDEX].pos.x = 500*Fast_Cos(-2*plight_ang);
        //lights2[POINT_LIGHT2_INDEX].pos.y = 400;
        lights2[POINT_LIGHT2_INDEX].pos.z = 1000*Fast_Sin(-2*plight_ang);

        // increment angle
        if ((plight_ang+=1) > 360)
            plight_ang = 0;

        // generate camera matrix
        Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

        //////////////////////////////////////////////////////////////////////////
        // the terrain

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(&obj_terrain);

        // generate rotation matrix around y axis
        //Build_XYZ_Rotation_MATRIX4X4(x_ang, y_ang, z_ang, &mrot);

        MAT_IDENTITY_4X4(&mrot);

        // rotate the local coords of the object
        Transform_OBJECT4DV2(&obj_terrain, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(&obj_terrain, TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &obj_terrain,0);

        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // shaded object

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(obj_work);

        // update rotation angle of object
        obj_work->ivar1+=3.0;

        if (obj_work->ivar1 >= 360)
            obj_work->ivar1 = 0;

        // set position of object
        obj_work->world_pos.x = 250*Fast_Cos(obj_work->ivar1);
        obj_work->world_pos.y = 200+125*Fast_Sin(3*obj_work->ivar1);
        obj_work->world_pos.z = 250*Fast_Sin(obj_work->ivar1);

        // create identity matrix
        MAT_IDENTITY_4X4(&mrot);

        // generate rotation matrix around y axis
        //Build_XYZ_Rotation_MATRIX4X4(x_ang, y_ang, z_ang, &mrot);

        // transform the local coords of the object
        Transform_OBJECT4DV2(obj_work, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(obj_work, TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, obj_work,0);

        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // draw all the light objects to represent the position of light sources

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(&obj_light_array[INDEX_GREEN_LIGHT_INDEX]);

        // set position of object to light
        obj_light_array[INDEX_GREEN_LIGHT_INDEX].world_pos = lights2[POINT_LIGHT_INDEX].pos;

        // create identity matrix
        MAT_IDENTITY_4X4(&mrot);

        // rotate the local coords of the object
        Transform_OBJECT4DV2(&obj_light_array[INDEX_GREEN_LIGHT_INDEX], &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(&obj_light_array[INDEX_GREEN_LIGHT_INDEX], TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &obj_light_array[INDEX_GREEN_LIGHT_INDEX],0);

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(&obj_light_array[INDEX_WHITE_LIGHT_INDEX]);

        // set position of object to light
        obj_light_array[INDEX_WHITE_LIGHT_INDEX].world_pos = lights2[POINT_LIGHT2_INDEX].pos;

        // create identity matrix
        MAT_IDENTITY_4X4(&mrot);

        // transform the local coords of the object
        Transform_OBJECT4DV2(&obj_light_array[INDEX_WHITE_LIGHT_INDEX], &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(&obj_light_array[INDEX_WHITE_LIGHT_INDEX], TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &obj_light_array[INDEX_WHITE_LIGHT_INDEX],0);

        ////////////////////////////////////////////////////////////////////////////////////

        // reset number of polys rendered
        debug_polys_rendered_per_frame = 0;
        debug_polys_lit_per_frame = 0;

        // prepare for rendering pass one, so rendering pass two can alpha blend the
        // shadows on top of this pass

        // remove backfaces
        if (backface_mode==1)
           Remove_Backfaces_RENDERLIST4DV2(&rend_list, &cam);

        // apply world to camera transform
        World_To_Camera_RENDERLIST4DV2(&rend_list, &cam);

        // clip the polygons themselves now
        Clip_Polys_RENDERLIST4DV2(&rend_list, &cam, CLIP_POLY_X_PLANE | CLIP_POLY_Y_PLANE | CLIP_POLY_Z_PLANE );

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

        // render the object

        if (wireframe_mode  == 0)
           Draw_RENDERLIST4DV2_Wire16(&rend_list, back_buffer, back_lpitch);
        else
        if (wireframe_mode  == 1)
        {
           // perspective mode affine texturing
              // set up rendering context
              rc.attr =    RENDER_ATTR_ZBUFFER
                      // | RENDER_ATTR_ALPHA
                      // | RENDER_ATTR_MIPMAP
                      // | RENDER_ATTR_BILERP
                         | RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE;

           // initialize zbuffer to 0 fixed point
           Clear_Zbuffer(&zbuffer, (16000 << FIXP16_SHIFT));

           // set up remainder of rendering context
           rc.video_buffer   = back_buffer;
           rc.lpitch         = back_lpitch;
           rc.mip_dist       = 0;
           rc.zbuffer        = (UCHAR *)zbuffer.zbuffer;
           rc.zpitch         = WINDOW_WIDTH*4;
           rc.rend_list      = &rend_list;
           rc.texture_dist   = 0;
           rc.alpha_override = -1;

           // render scene
           Draw_RENDERLIST4DV2_RENDERCONTEXTV1_16_2(&rc);
        } // end if


        // now make second rendering pass and draw shadow(s)

        // reset the render list
        Reset_RENDERLIST4DV2(&rend_list);

        //////////////////////////////////////////////////////////////////////////
        // shadow object

        // reset the object (this only matters for backface and object removal)
        Reset_OBJECT4DV2(&shadow_obj);

        // compute terrain cell shadow is over
        cell_x = (obj_work->world_pos.x  + TERRAIN_WIDTH/2) / obj_terrain.fvar1;
        cell_y = (obj_work->world_pos.z  + TERRAIN_HEIGHT/2) / obj_terrain.fvar1;

        // compute vertex indices into vertex list of the current quad
        int v0 = cell_x + cell_y*obj_terrain.ivar2;
        int v1 = v0 + 1;
        int v2 = v1 + obj_terrain.ivar2;
        int v3 = v0 + obj_terrain.ivar2;

        // now simply index into table
        terrain_height = MAX(    MAX(obj_terrain.vlist_trans[v0].y, obj_terrain.vlist_trans[v1].y),
                                 MAX(obj_terrain.vlist_trans[v2].y, obj_terrain.vlist_trans[v3].y) );

        // update position
        shadow_obj.world_pos   = obj_work->world_pos;
        shadow_obj.world_pos.y = terrain_height+25;

        // now compute the size of the shadow radius based on the calculations shown in the book
        // the size of the projected shadow, assuming a sphere as the occluder with a point lightsource
        // height hl above the ground plane, a spherical object with radius ro, and height above
        // the groundplane ho, the radius of the shadow is rs:
        //
        //    rs = ro * (hl)/(hl - ho)

        // set height to altitude of green light for fun
        hl = lights2[POINT_LIGHT_INDEX].pos.y;

        // use the average radius of the object to base calcs on, better than nothing
        float rs = ks * ( obj_work->avg_radius[0] * (hl/ (hl - obj_work->world_pos.y)) );

        // generate scaling matrix bases on shadow scale
        MAT_IDENTITY_4X4(&mrot);

        // set scaling values to scale in X-Z plane
        mrot.M00 = rs;
        mrot.M11 = 1.0;
        mrot.M22 = rs;

        // scale the local coords of the object via the matrix
        Transform_OBJECT4DV2(&shadow_obj, &mrot, TRANSFORM_LOCAL_TO_TRANS,1);

        // perform world transform
        Model_To_World_OBJECT4DV2(&shadow_obj, TRANSFORM_TRANS_ONLY);

        // insert the object into render list
        Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &shadow_obj,0);

        //////////////////////////////////////////////////////////////////////////

        // remove backfaces
        if (backface_mode==1)
           Remove_Backfaces_RENDERLIST4DV2(&rend_list, &cam);

        // apply world to camera transform
        World_To_Camera_RENDERLIST4DV2(&rend_list, &cam);

        // clip the polygons themselves now
        Clip_Polys_RENDERLIST4DV2(&rend_list, &cam, CLIP_POLY_X_PLANE | CLIP_POLY_Y_PLANE | CLIP_POLY_Z_PLANE );

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

        // render the rendering list
        if (wireframe_mode  == 0)
           Draw_RENDERLIST4DV2_Wire16(&rend_list, back_buffer, back_lpitch);
        else
        if (wireframe_mode  == 1)
        {
           // perspective mode affine texturing
              // set up rendering context
              rc.attr =    RENDER_ATTR_ZBUFFER
                         | RENDER_ATTR_ALPHA
                      // | RENDER_ATTR_MIPMAP
                      // | RENDER_ATTR_BILERP
                         | RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE;

           // initialize zbuffer to 0 fixed point
           //Clear_Zbuffer(&zbuffer, (16000 << FIXP16_SHIFT));

           // set up remainder of rendering context
           rc.video_buffer   = back_buffer;
           rc.lpitch         = back_lpitch;
           rc.mip_dist       = 0;
           rc.zbuffer        = (UCHAR *)zbuffer.zbuffer;
           rc.zpitch         = WINDOW_WIDTH*4;
           rc.rend_list      = &rend_list;
           rc.texture_dist   = 0;
           rc.alpha_override = -1;

           // render scene
           Draw_RENDERLIST4DV2_RENDERCONTEXTV1_16_3(&rc);
        } // end if

        // unlock the back buffer
        unlock_screen_surface();

        // draw cockpit
        //Draw_BOB16(&cockpit, lpddsback);

        sprintf(work_string,"Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, BckFceRM [%s], Light Altitude=%f",
                                                                                         ((lighting_mode == 1) ? "ON" : "OFF"),
                                                                                         lights2[AMBIENT_LIGHT_INDEX].state,
                                                                                         lights2[INFINITE_LIGHT_INDEX].state,
                                                                                         lights2[POINT_LIGHT_INDEX].state,
                                                                                         ((backface_mode == 1) ? "ON" : "OFF"),
                                                                                         lights2[POINT_LIGHT_INDEX].pos.y);

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
            draw_text("<W>..............Toggle wire frame/solid mode.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<B>..............Toggle backface removal.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<O>..............Select different objects.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<1>,<2>..........Change height of green point light.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<H>..............Toggle Help.", 0, text_y+=12, RGB(255,255,255));
            draw_text("<ESC>............Exit demo.", 0, text_y+=12, RGB(255,255,255));

        } // end help

        sprintf(work_string,"Polys Rendered: %d, Polys lit: %d", debug_polys_rendered_per_frame, debug_polys_lit_per_frame);
        draw_text(work_string, 0, WINDOW_HEIGHT-34-16-16, RGB(0,255,0));

        sprintf(work_string,"CAM [%5.2f, %5.2f, %5.2f], CELL [%d, %d]",  cam.pos.x, cam.pos.y, cam.pos.z, cell_x, cell_y);

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
