//***********************************************************************//
//                                                                       //
//      - "Talk to me like I'm a 3 year old!" Programming Lessons -      //
//                                                                       //
//      $Author:        DigiBen     digiben@gametutorials.com            //
//                                                                       //
//      $Program:       MD2 Animation                                    //
//                                                                       //
//      $Description:   Demonstrates how to animate a Quake2 MD2 Model   //
//                                                                       //
//      $Date:          2/10/02                                          //
//                                                                       //
//***********************************************************************//


#include "main.h"                                        // This includes our header file
#include "Md2.h"

#include <math.h>

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the second tutorial to loading .MD2 models.  In the previous
// tutorial, we just loaded the model's first frame of animation.  Now we
// get to load the rest of the key frames and interpolate between them to create
// the somewhat smooth animation that you see in Quake2 games.  Let's go over some
// of the more important vocabulary that you will need to understand for this tutorial:
//
// Key Frames Animation: Key frame animation is where you store only certain
// parts of an animation, then interpolate between those frames to produce a
// smooth animation.  For instance, say you have a ball that starts from one
// side of the room and rolls to the other side in a 100 frame animation.  Do
// we really need to store all 100 frames to recreate that animation?  No, we
// just need to store a key frame at frame 1 and frame 100, then interpolate
// between them.  This is how the Quake2 models work.  There is just important
// frames saved when ever the model moves it's body in a different position.
// This is great and all, but what the heck does interpolation mean??!!?
//
// Interpolation: Gamedev.net's Game Dictionary say interpolation is "using a ratio 
// to step gradually a variable from one value to another."  In our case, this
// means that we gradually move our vertices from one key frame to another key frame.
// There are many types of interpolation, but we are just going to use linear.
// The equation for linear interpolation is this:
//
//              p(t) = p0 + t(p1 - p0)
//
//              t - The current time with 0 being the start and 1 being the end
//              p(t) - The result of the equation with time t
//              p0 - The starting position
//              p1 - The ending position
//
// Let's throw in an example with numbers to test this equation.  If we have
// a vertex stored at 0 along the X axis and we wanted to move the point to
// 10 with 5 steps, see if you can fill in the equation without a time just yet.
//
// Finished?  You should have come up with:
//
//              p(t) = 0 + t(10 - 0)
//              p(t) = 0 + 10t
//              p(t) = 10t
//
// Now, all we need it a time from 0 to 1 to pass in, which will allow us to find any
// point from 0 to 10, depending on the time.  Since we wanted to find out the distance
// we need to travel each frame if we want to reach the end point in 5 steps, we just
// divide 1 by 5: 1/5 = 0.2
//
// We can then pass this into our equation:
//
//              p(0.2) = 10 * 0.2
//              p(0.2) = 2
//
// What does that tell us?  It tells us we need to move the vertex along the x
// axis each frame by a distance of 2 to reach 10 in 5 steps.  Yah yah, this isn't
// rocket science, but it's important to know that what your mind would have done
// immediately without thinking about it, is linear interpolation.  
//
// Are you starting to see how this applies to our model?  If we only read in key
// frames, then we need to interpolate every vertex between the current and next
// key frame for animation.  To get a perfect idea of what is going on, try
// taking out the interpolation and just render the key frames.  You will notice
// that you can still see what is kinda going on, but it moves at an incredible pace!
// There is not smoothness, just a really fast jumpy animation.
//
// If you are wondering, animation isn't really done this way anymore.  To perform
// more realistic and dynmaic animation, developers and artists moved to bone animation,
// otherwise known as skeletal animation.  If you look at the Quake3 .MD3 file format,
// you'll notice this change to bones instead of key frames.  With this technique, you
// can allows the player to dynamically do things that done have to be animated by
// an artists, such as having the player look in the direction of another player as
// it comes into the same room.  Also, you can have the the player reach out for things
// in the world that might be in different positions, say on a shelf or a table.  The
// arm can then move exactly to that object and pick it up.
//
// I plan to create tutorials on loading/animating Quake3 models as well, so stay tuned!
// You might take note that in the last tutorial we calculated the face normals of our
// model, but on second thought, I decided to not do it in our final MD2 tutorial because
// the model's texture maps are created for their own lighting and just make it darker
// than usual.  Quake2 and many other games don't use dynamic lighting because it incredibly
// slow, unless it's done on a graphics card with a GPU.
// 
// In this tutorial, we do not start out spinning the model as we did in the last one,
// you have to manually do it.  That way we won't get confused betwen the rotations and
// the actual animation.  The current animation is displayed in the windows title bar.
// That way you know exactly what is going on in the animation.  

// The controls for this application are:
//
// Left Mouse Button - Changes the Render mode from normal to wireframe.
// Right Mouse Button - Cycles through the animations
// Left Arrow Key - Spins the model to the left
// Right Arrow Key - Spins the model to the right
// Escape - Quits
//
// If you decide to go find models you might find them in a format like .pk3.
// This is just a .zip format, so rename it to .zip and unzip it.  There should
// be .Md2 and texture files.  I think the textures are normally in .pcx, so you
// will have to save them as BMP's to run in here.
//
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


#define FILE_NAME  "tris.md2"                           // This is the 3D file we will load.
#define TEXTURE_NAME "hobgoblin.bmp"

unsigned int g_Texture[MAX_TEXTURES] = {0};                     // This holds the texture info, referenced by an ID

CLoadMD2 g_LoadMd2;                                     // This is MD2 class.  This should go in a good model class.
t3DModel g_3DModel;                                     // This holds the 3D Model info that we load in

int   g_ViewMode      = GL_TRIANGLES;                   // We want the default drawing mode to be normal
bool  g_bLighting     = true;                           // Turn lighting on initially
float g_RotateX       = 0.0f;                           // This is the current value at which the model is rotated
float g_RotationSpeed = 0.5f;                           // This is the speed that our model rotates.  (-speed rotates left)

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////



///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This function initializes the game window.
/////
///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init()
{
    InitializeGL(SCREEN_WIDTH, SCREEN_HEIGHT);    // Init OpenGL with the global rect

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

    // First we need to actually load the .MD2 file.  We just pass in an address to
    // our t3DModel structure and the file name string we want to load ("tris.md2").
    // We also need to give the texture name we will be using.  This is because there
    // are usually a lot of textures with each character.  You choose the best one.
    // It seems that all of the Quake2 characters .md2 files are called: "tris.md2"

    g_LoadMd2.ImportMD2(&g_3DModel, FILE_NAME, TEXTURE_NAME);       

    // There is no color information for these models, as well as only one
    // texture.  If everything goes well, it should load the TEXTURE_NAME file.

    // Go through all the materials
    for(int i = 0; i < g_3DModel.numOfMaterials; i++)
    {
        // Check to see if there is a file name to load in this material
        if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
        {
            // Use the name of the texture file to load the bitmap, with a texture ID (i).
            // We pass in our global texture array, the name of the texture, and an ID to reference it. 
            cout << "CreateTexture " << endl;
            CreateTexture(g_Texture, g_3DModel.pMaterials[i].strFile, i);           
            cout << g_3DModel.pMaterials[i].strFile << endl;
        }

        // Set the texture ID for this material
        g_3DModel.pMaterials[i].texureId = i;
    }

    // Here, we turn on a lighting and enable lighting.  We don't need to
    // set anything else for lighting because we will just take the defaults.
    // We also want color, so we turn that on

    glEnable(GL_LIGHT0);                                // Turn on a light with defaults set
    glEnable(GL_LIGHTING);                              // Turn on lighting
    glEnable(GL_COLOR_MATERIAL);                        // Allow color

    // To make our model render somewhat faster, we do some front back culling.
    // It seems that Quake2 orders their polygons clock-wise.

    glEnable(GL_CULL_FACE);                             // Turn culling on
    glCullFace(GL_FRONT);                               // Quake2 uses front face culling apparently

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// RETURN CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This returns time t for the interpolation between the current and next key frame
/////
///////////////////////////////// RETURN CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float ReturnCurrentTime(t3DModel *pModel, int nextFrame)
{
    static float elapsedTime   = 0.0f;
    static float lastTime     = 0.0f;

    // This function is very similar to finding the frames per second.
    // Instead of checking when we reach a second, we check if we reach
    // 1 second / our animation speed. (1000 ms / kAnimationSpeed).
    // That's how we know when we need to switch to the next key frame.
    // In the process, we get the t value for how we are at to going to the
    // next animation key frame.  We use time to do the interpolation, that way
    // it runs the same speed on any persons computer, regardless of their specs.
    // It might look chopier on a junky computer, but the key frames still be
    // changing the same time as the other persons, it will just be not as smooth
    // of a transition between each frame.  The more frames per second we get, the
    // smoother the animation will be.

    // Get the current time in milliseconds
    float time = SDL_GetTicks();

    // Find the time that has elapsed since the last time that was stored
    elapsedTime = time - lastTime;

    // To find the current t we divide the elapsed time by the ratio of 1 second / our anim speed.
    // Since we aren't using 1 second as our t = 1, we need to divide the speed by 1000
    // milliseconds to get our new ratio, which is a 5th of a second.
    float t = elapsedTime / (1000.0f / kAnimationSpeed);
    
    // If our elapsed time goes over a 5th of a second, we start over and go to the next key frame
    if (elapsedTime >= (1000.0f / kAnimationSpeed) )
    {
        // Set our current frame to the next key frame (which could be the start of the anim)
        pModel->currentFrame = nextFrame;

        // Set our last time to the current time just like we would when getting our FPS.
        lastTime = time;
    }

    // Return the time t so we can plug this into our interpolation.
    return t;
}


///////////////////////////////// ANIMATE MD2 MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This draws and animates the .md2 model by interpoloated key frame animation
/////
///////////////////////////////// ANIMATE MD2 MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void AnimateMD2Model(t3DModel *pModel)
{
    // Now comes the juice of our tutorial.  Fear not, this is actually very intuitive
    // if you drool over it for a while (stay away from the keyboard though...).
    // What's going on here is, we are getting our current animation that we are
    // on, finding the current frame of that animation that we are on, then interpolating
    // between that frame and the next frame.  To make a smooth constant animation when
    // we get to the end frame, we interpolate between the last frame of the animation 
    // and the first frame of the animation.  That way, if we are doing the running 
    // animation let's say, when the last frame of the running animation is hit, we don't
    // have a huge jerk when going back to the first frame of that animation.  Remember,
    // because we have the texture and face information stored in the first frame of our
    // animation, we need to reference back to this frame every time when drawing the
    // model.  The only thing the other frames store is the vertices, but no information
    // about them.
    
    // Make sure we have valid objects just in case. (size() is in the vector class)
    if(pModel->pObject.size() <= 0) return;

    // Here we grab the current animation that we are on from our model's animation list
    tAnimationInfo *pAnim = &(pModel->pAnimations[pModel->currentAnim]);

    // This gives us the current frame we are on.  We mod the current frame plus
    // 1 by the current animations end frame to make sure the next frame is valid.
    // If the next frame is past our end frame, then we go back to zero.  We check this next.
    int nextFrame = (pModel->currentFrame + 1) % pAnim->endFrame;

    // If the next frame is zero, that means that we need to start the animation over.
    // To do this, we set nextFrame to the starting frame of this animation.
    if(nextFrame == 0) 
        nextFrame =  pAnim->startFrame;

    // Get the current key frame we are on
    t3DObject *pFrame =      &pModel->pObject[pModel->currentFrame];

    // Get the next key frame we are interpolating too
    t3DObject *pNextFrame =  &pModel->pObject[nextFrame];

    // Get the first key frame so we have an address to the texture and face information
    t3DObject *pFirstFrame = &pModel->pObject[0];

    // Next, we want to get the current time that we are interpolating by.  Remember,
    // if t = 0 then we are at the beginning of the animation, where if t = 1 we are at the end.
    // Anyhing from 0 to 1 can be thought of as a percentage from 0 to 100 percent complete.
    float t = ReturnCurrentTime(pModel, nextFrame);

    // Start rendering lines or triangles, depending on our current rendering mode (Lft Mouse Btn)
    glBegin(g_ViewMode);

        // Go through all of the faces (polygons) of the current frame and draw them
        for(int j = 0; j < pFrame->numOfFaces; j++)
        {
            // Go through each corner of the triangle and draw it.
            for(int whichVertex = 0; whichVertex < 3; whichVertex++)
            {
                // Get the index for each point of the face
                int vertIndex = pFirstFrame->pFaces[j].vertIndex[whichVertex];

                // Get the index for each texture coordinate for this face
                int texIndex  = pFirstFrame->pFaces[j].coordIndex[whichVertex];
                        
                // Make sure there was a UVW map applied to the object.  Notice that
                // we use the first frame to check if we have texture coordinates because
                // none of the other frames hold this information, just the first by design.
                if(pFirstFrame->pTexVerts) 
                {
                    // Pass in the texture coordinate for this vertex
                    glTexCoord2f(pFirstFrame->pTexVerts[ texIndex ].x, 
                                 pFirstFrame->pTexVerts[ texIndex ].y);
                }

                // Now we get to the interpolation part! (*Bites his nails*)
                // Below, we first store the vertex we are working on for the current
                // frame and the frame we are interpolating too.  Next, we use the
                // linear interpolation equation to smoothly transition from one
                // key frame to the next.
                
                // Store the current and next frame's vertex
                CVector3 vPoint1 = pFrame->pVerts[ vertIndex ];
                CVector3 vPoint2 = pNextFrame->pVerts[ vertIndex ];

                // By using the equation: p(t) = p0 + t(p1 - p0), with a time t
                // passed in, we create a new vertex that is closer to the next key frame.
                glVertex3f(vPoint1.x + t * (vPoint2.x - vPoint1.x), // Find the interpolated X
                           vPoint1.y + t * (vPoint2.y - vPoint1.y), // Find the interpolated Y
                           vPoint1.z + t * (vPoint2.z - vPoint1.z));// Find the interpolated Z
            }
        }

    // Stop rendering the triangles
    glEnd();    
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer
    glLoadIdentity();                                   // Reset The matrix

    // Give OpenGL our position,    then view,      then up vector
    gluLookAt(      0, 1.5f, 100,       0, 0.5f, 0,         0, 1, 0);

    //glRotatef(g_RotateX, 0, 1.0f, 0);                 // Rotate the object around the Y-Axis
    g_RotateX += g_RotationSpeed;                       // Increase the speed of rotation if any


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

    // This is where we call our animation function to draw and animate our character.
    // You can pass in any model into here and it will draw and animate it.  Of course,
    // it would be a good idea to stick this function in your model class.

    AnimateMD2Model(&g_3DModel);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


    SDL_GL_SwapBuffers();                           // Swap the backbuffers to the foreground
}

////////////////////////////////   MAIN GAME LOOP   \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////////
////////   This function handles the main game loop
////////
////////////////////////////////   MAIN GAME LOOP   \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void MainLoop(void)
{
    bool done = false;                                     // is our job done ? not yet !
    SDL_Event event;
    char strWindowTitle[255];

    while(! done)                                          // as long as our job's not done
    {
        while( SDL_PollEvent(& event) )                    // look for events (like keystrokes, resizing etc.)
        {
            switch ( event.type )                          // what kind of event have we got ?
            {
                case SDL_QUIT :                                         // if user wishes to quit
                    done = true;                                        // this implies our job is done
                    break;

                case SDL_KEYDOWN :                                      // if the user has pressed a key
                    HandleKeyPressEvent( & event. key.keysym );         // callback for handling keystrokes, arg is key pressed
                    break;

                case SDL_VIDEORESIZE :                                  // if there is a resize event
                    // request SDL to resize the window to the size and depth etc. that we specify
                    MainWindow = SDL_SetVideoMode( event.resize.w, event.resize.h, SCREEN_DEPTH, VideoFlags );
                    SizeOpenGLScreen(event.resize.w, event.resize.h);   // now resize the OpenGL viewport
    
                    if(MainWindow == NULL)                              // if window resize has failed
                    {
                        cerr << " Failed resizing SDL window : " << SDL_GetError() << endl;   // report error
                        Quit(0);
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN :
                    if( event.button.button == SDL_BUTTON_LEFT )
                    {
                        if(g_ViewMode == GL_TRIANGLES) {                // We our drawing mode is at triangles
                            g_ViewMode = GL_LINE_STRIP;                 // Go to line stips
                        } 
                        else {
                            g_ViewMode = GL_TRIANGLES;                  // Go to triangles
                        }
                    }

                    if( event.button.button == SDL_BUTTON_RIGHT )
                    {
/////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

                        // To cycle through the animations, we just increase the model's current animation
                        // by 1.  You'll notice that we also mod this result by the total number of
                        // animations in our model, to make sure we go back to the beginning once we reach
                        // the end of our animation list.  

                        // Increase the current animation and mod it by the max animations
                        g_3DModel.currentAnim = (g_3DModel.currentAnim + 1) % (g_3DModel.numOfAnimations);

                        // Set the current frame to be the starting frame of the new animation
                        g_3DModel.currentFrame = g_3DModel.pAnimations[g_3DModel.currentAnim].startFrame;

                        // Display the current animation in our window
                        sprintf(strWindowTitle, "www.GameTutorials.com - Md2 Animation: %s", 
                                g_3DModel.pAnimations[g_3DModel.currentAnim].strName);

                        SDL_WM_SetCaption(strWindowTitle,"GameTutorials");

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

                    }
                    break;

                default:                                    // any other event
                    break;                                  // nothing to do
            } // switch
        } // while( SDL_ ...
        RenderScene();                                      // draw our OpenGL scene

    } // while( ! done)
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

    // When we are done, we need to free all the model data
    // We do this by walking through all the objects and freeing their information

    // Go through all the objects in the scene
    for(int i = 0; i < g_3DModel.numOfObjects; i++)
    {
        // Free the faces, normals, vertices, and texture coordinates.
        delete [] g_3DModel.pObject[i].pFaces;
        delete [] g_3DModel.pObject[i].pNormals;
        delete [] g_3DModel.pObject[i].pVerts;
        delete [] g_3DModel.pObject[i].pTexVerts;
    }

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load and animate a .Md2 file. We learned about key frame
// and how to interpolate using the linear interpolation equation: p(t) = p0 + t(p1 - p0).
// key frames is not the best way to animate, but it's one of the easiest, besides just
// storing every single frame of animation.  When creating your 3D engine, you need to
// weight the pro's and cons of having less calculation during run-time (which doesn't
// matter much with 1.6 ghz processors) or having less of a memory print in ram and on
// the hard disk.  In my opinion, with our technology, skeletal animation is the best
// technique for animation.  There is so many possibilities with it that can make you
// game incredibly realistic.  Eternal Darkness for the GameCube is a classic example
// of this wonder technique.
// 
// In this version of our .MD2 loader, we added 2 functions:
//
// float ReturnCurrentTime(t3DModel *pModel, int nextFrame);
//
// and
//
// void AnimateMD2Model(t3DModel *pModel);
//
// These help us to use linear interpolation to draw and animate the player model
// according to time.  That way it won't animate the model faster on a slower computer,
// it will just aniamate it more choppy.  The faster the computer, the smoother the
// animation will be.
//
// In the next tutorial on Quake model loading, I will show you how to load a .MD3.
// This file format uses a skeletal/bone system to animate the players.  This is
// extremely advanced and takes a decent knowledge of quaternions and model loading.
// I have been waiting to tackle bone animation since the day GameTutorials.com started.
// It will be a great and glorious day when it's finished :)  There will be 2 tutorials
// that cover .Md3 files.  The first will be just as the first tutorial on .MD2 loading.
// There will be no animation until the second tutorial.  That way we break up the huge
// topics into smaller more easier to digest chunks.
//
// I would like to once again thank Daniel E. Schoenblum <dansch@hops.cs.jhu.edu> for help
// with explaining the file format.  You can check it out at:  
// 
// http://www.ugrad.cs.jhu.edu/~dansch/md2/#model_magic
//
// Let me know if this helps you out!
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// The Quake2 .Md2 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written conset
// from ID Software.
//
// Quake and Quake2 are trademarks of ID Software.
// All trademarks used are properties of their respective owners. 
//
//
