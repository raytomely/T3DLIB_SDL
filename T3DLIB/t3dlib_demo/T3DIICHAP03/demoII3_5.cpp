// DEMOII3_5.CPP - mouse demo based on code from Volume I

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


// gui defines
#define BUTTON_SPRAY    0    // defines for each button
#define BUTTON_PENCIL   1
#define BUTTON_ERASE    2
#define BUTTON_EXIT     3

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                          // used to print text


// demo globals
BOB          buttons,           // a bob with all the buttons
             pointer;           // a pointer bob
BITMAP_IMAGE cpanel;            // the control panel
BITMAP_IMAGE canvas;            // off screen drawing canvas

int mouse_x,                    // used to track mouse
    mouse_y;

UCHAR mouse_color=100;          // color of mouse brush

int command_state=0;            // state of user command

// position of control buttons
int buttons_x[] = {509, 559, 509, 559};
int buttons_y[] = {344, 344, 383, 383};

// on/off state of buttons
int buttons_state[] = {0,1,0,0};

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

    mouse_init(MOUSE_RELATIVE_MODE);

    /////////////////////////////////////////////////////////////////

    // set the global mouse position
    mouse_x = screen_height/2;
    mouse_y = screen_height/2;

    // load the master bitmap in with all the graphics
    Load_Bitmap_File(&bitmap8bit, "PAINT.BMP");
    set_palette(get_palette(bitmap8bit));

    // make sure all the surfaces are clean before starting
    fill_screen(0);

    // create the pointer bob
    Create_BOB(&pointer,mouse_x,mouse_y,32,34,1,
               BOB_ATTR_VISIBLE | BOB_ATTR_SINGLE_FRAME,0);

    // load the image for the pointer in
    Load_Frame_BOB(&pointer,&bitmap8bit,0,0,2,BITMAP_EXTRACT_MODE_CELL);

    // create the button bob
    Create_BOB(&buttons,0,0,32,34,8,
               BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,0);

    // load buttons in, two banks of 4, all the off's, then all the on's
    for (index=0; index<8; index++)
         Load_Frame_BOB(&buttons,&bitmap8bit,index, index%4,index/4,BITMAP_EXTRACT_MODE_CELL);

    // create the bitmap to hold the control panel
    Create_Bitmap(&cpanel,500,0,104,424);
    Load_Image_Bitmap(&cpanel, &bitmap8bit,150,0,BITMAP_EXTRACT_MODE_ABS);

    // create the drawing canvas bitmap
    Create_Bitmap(&canvas,0,0,500,SCREEN_HEIGHT);
    memset(canvas.buffer,0,canvas.width*canvas.height);
    canvas.attr = BITMAP_ATTR_LOADED;

    // clear out the canvas
    // memset(canvas.buffer,0,canvas.width*canvas.height);


    // hide the mouse
    show_cursor(FALSE);

    // seed random number generator
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

    // release all your resources created for the game here....

    // unload the bitmap file
    Unload_Bitmap_File(&bitmap8bit);

    // delete all bobs and bitmaps
    Destroy_BOB(&buttons);
    Destroy_BOB(&pointer);
    Destroy_Bitmap(&cpanel);
    Destroy_Bitmap(&canvas);

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

    int index; // looping var

    main_loop = 1;

    while(main_loop)
    {

        // clear the drawing surface
        fill_screen(0);

        // get the mouse data
        read_keyboard();
        read_mouse();

        // move the mouse cursor
        mouse_x+=(mouse_state.lX);
        mouse_y+=(mouse_state.lY);

        // test bounds

        // first x boundaries
        if (mouse_x >= screen_width)
           mouse_x = screen_width-1;
        else
        if (mouse_x < 0)
           mouse_x = 0;

        // now the y boundaries
        if (mouse_y >= screen_height)
           mouse_y= screen_height-1;
        else
        if (mouse_y < 0)
           mouse_y = 0;

        // position the pointer bob to the mouse coords
        pointer.x = mouse_x - 16;
        pointer.y = mouse_y - 16;

        // test what the user is doing with the mouse
        if ((mouse_x > 3) && (mouse_x < 500-3) &&
            (mouse_y > 3) && (mouse_y < SCREEN_HEIGHT-3))
        {
           // mouse is within canvas region

           // if left button is down then draw
           if (mouse_state.rgbButtons[0])
           {
              // test drawing mode
              if (buttons_state[BUTTON_PENCIL])
              {
                 // draw a pixel
                 Draw_Pixel(mouse_x, mouse_y, mouse_color, canvas.buffer, canvas.width);
                 Draw_Pixel(mouse_x+1, mouse_y, mouse_color, canvas.buffer, canvas.width);
                 Draw_Pixel(mouse_x, mouse_y+1, mouse_color, canvas.buffer, canvas.width);
                 Draw_Pixel(mouse_x+1, mouse_y+1, mouse_color, canvas.buffer, canvas.width);
              }
              else
              {
                 // draw spray
                 for (index=0; index<10; index++)
                 {
                     // get next particle
                     int sx=mouse_x-8+rand()%16;
                     int sy=mouse_y-8+rand()%16;

                     // make sure particle is in bounds
                     if (sx > 0 && sx < 500 && sy > 0 && sy < screen_height)
                        Draw_Pixel(sx, sy, mouse_color, canvas.buffer, canvas.width);
                 } // end for index

              } // end else

           } // end if left button
           else // right button is eraser
           if (mouse_state.rgbButtons[1])
           {
               // test drawing mode
               if (buttons_state[BUTTON_PENCIL])
               {
                  // erase a pixel
                  Draw_Pixel(mouse_x, mouse_y, 0, canvas.buffer, canvas.width);
                  Draw_Pixel(mouse_x+1, mouse_y, 0, canvas.buffer, canvas.width);
                  Draw_Pixel(mouse_x, mouse_y+1, 0, canvas.buffer, canvas.width);
                  Draw_Pixel(mouse_x+1, mouse_y+1, 0, canvas.buffer, canvas.width);
               } // end if
               else
               {
                  // erase spray
                  for (index=0; index<20; index++)
                  {
                      // get next particle
                      int sx=mouse_x-8+rand()%16;
                      int sy=mouse_y-8+rand()%16;

                      // make sure particle is in bounds
                      if (sx > 0 && sx < 500 && sy > 0 && sy < screen_height)
                         Draw_Pixel(sx, sy, 0, canvas.buffer, canvas.width);
                  } // end for index

               } // end else

           } // end if left button

        } // end if
        else
        if ( (mouse_x > 500+16) && (mouse_x < 500+16+8*9) &&
             (mouse_y > 8)      && (mouse_y < 8+32*9))
        {
           // within palette

           // test if button left button is down
           if (mouse_state.rgbButtons[0])
           {
              // see what color cell user is pointing to
              int cell_x = (mouse_x - (500+16))/9;
              int cell_y = (mouse_y - (8))/9;

              // change color
              mouse_color = cell_x + cell_y*8;

           } // end if
        } // end if
        else
        if ((mouse_x > 500) && (mouse_x < (500+100)) &&
            (mouse_y > 344) && (mouse_y < (383+34)) )
        {
           // within button area
           // test for each button
           for (index=0; index<4; index++)
           {
               if ((mouse_x > buttons_x[index]) && (mouse_x < (buttons_x[index]+32)) &&
                   (mouse_y > buttons_y[index]) && (mouse_y < (buttons_y[index]+34)) )
                   break;

           } // end for

           // at this point we know where the user is, now determine what he
           // is doing with the buttons
           switch(index)
           {
                 case BUTTON_SPRAY:
                 {
                     // if left button is down simply activate spray mode
                     if (mouse_state.rgbButtons[0])
                     {
                        // depress button
                        buttons_state[index] = 1;

                       // de-activate pencil mode
                        buttons_state[BUTTON_PENCIL] = 0;
                     } // end if
                     else
                     {
                        // make sure button is up
                        // buttons_state[index] = 0;
                     } // end else

                 } break;

                 case BUTTON_PENCIL:
                 {
                     // if left button is down activate spray mode
                     if (mouse_state.rgbButtons[0])
                     {
                        // depress button
                        buttons_state[index] = 1;

                        // de-activate spray mode
                        buttons_state[BUTTON_SPRAY] = 0;

                     } // end if
                     else
                     {
                        // make sure button is up
                        // buttons_state[index] = 0;
                     } // end else

                } break;

                 case BUTTON_ERASE:
                 {
                     // test if left button is down, if so clear screen
                     if (mouse_state.rgbButtons[0])
                     {
                        // clear memory
                        memset(canvas.buffer,0,canvas.width*canvas.height);

                        // depress button
                        buttons_state[index] = 1;
                     } // end if
                     else
                     {
                        // make sure button is up
                        buttons_state[index] = 0;
                     } // end else
                } break;

                 case BUTTON_EXIT:
                 {
                     // test if left button down, if so bail
                     if (mouse_state.rgbButtons[0])
                          //PostMessage(main_window_handle, WM_DESTROY,0,0);
                          main_loop = 0;

                 } break;

           } // end switch

        } // end if
        else
        {
           // no mans land

        } // end else

        // lock back buffer
        lock_screen_surface();

        // draw the canvas
        Draw_Bitmap(&canvas, back_buffer, back_lpitch,0);

        // draw control panel
        Draw_Bitmap(&cpanel,back_buffer,back_lpitch,0);

        // unlock back buffer
        unlock_screen_surface();

        // draw the color palette
        for (int col=0; col < 256; col++)
        {
            draw_rectangle(500+16+(col%8)*9,   8+(col/8)*9,
               500+16+(col%8)*9+8, 8+(col/8)*9+8,
               col);

        } // end for col

        // draw the current color selected
        draw_rectangle(533,306,533+34,306+34,mouse_color);

        // draw the buttons
        for (index=0; index<4; index++)
        {
            // set position of button bob
            buttons.x = buttons_x[index];
            buttons.y = buttons_y[index];

            // now select the on/off frame based on if the
            // button is off
            if (buttons_state[index]==0)
                buttons.curr_frame = index;
            else // button is on
                buttons.curr_frame = index+4;

            // draw the button
            Draw_BOB(&buttons, screen);

        } // end for index

        // display coords
        sprintf(buffer,"Pointer (%d,%d)",mouse_x,mouse_y);
        draw_text(buffer, 8,screen_height - 16,RGB(0,255,0));
        draw_text("T3D Paint Version 1.0 - Press <ESC> to Exit.",0,0,RGB(255,0,0));

        // draw the cursor last
        Draw_BOB(&pointer,screen);

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
