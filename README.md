# spine-sdl

Note:spine-sdl currently only support opengles 1.x-based platform: eg, iOS , Android. However it's easy to be extended to opengles 2.

The spine-sdl runtime provides functionality to load, manipulate and render Spine skeletal animation data using SDL2(>=2.0.6). spine-sdl is based on [spine-c](https://github.com/EsotericSoftware/spine-runtimes/tree/c6f31e6310833caa57da496db6bb04965d1e88c7/spine-c).

# Licensing
This Spine Runtime may only be used for personal or internal use, typically to evaluate Spine before purchasing. If you would like to incorporate a Spine Runtime into your applications, distribute software containing a Spine Runtime, or modify a Spine Runtime, then you will need a valid [Spine license](https://esotericsoftware.com/spine-purchase). Please see the [Spine Runtimes Software License](http://esotericsoftware.com/git/spine-runtimes/blob/LICENSE) for detailed information.
The Spine Runtimes are developed with the intent to be used with data exported from Spine. By purchasing Spine, Section 2 of the [Spine Software License](https://esotericsoftware.com/files/license.txt) grants the right to create and distribute derivative works of the Spine Runtimes.

# Spine version
spine-sdl works with data exported from Spine 3.6.xx.

spine-sdl supports all Spine features.

spine-sdl does not yet support loading the binary format.

# Setup

Note: You will have to compile a custom version of SDL2 to use this.

1. Create a new [SDL2](https://github.com/SDL-mirror/SDL) project and integrate [spine-c](https://github.com/EsotericSoftware/spine-runtimes/tree/c6f31e6310833caa57da496db6bb04965d1e88c7/spine-c). 

2. Add spine-sdl/spine-sdl.h and spine-sdl/spine-sdl.cpp into your own src directory as part of your source code.

3. (a) Copy SDL/src/include/SDL_opengles.h and SDL/src/include/SDL_render.h,then paste them to your projects' SDL src/include directory. (which replaced the same named files)
   
   (b) Just like we did in (a) , copy all the files in SDL/src/render (SDL_render.c,SDL_sysrender.h,and files in "opengles" and "opengl" directory) and then replace the same named files in your projects' SDL src/render direcotry with them.
   
   (c) Just like (b), copy all the files in SDL/src/dynapi to your projects SDL src/dynapi directory
   
   Note:Currently spine-sdl is based on SDL 2.0.8, if your SDL version is higher,you may manually migriate these same named files mentioned above.
   
# Usage 

1. Include spine-sdl.h and required spine-c headers then Create SDL window and Renderer like this:
```java 
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
SDL_SetHint( SDL_HINT_RENDER_DRIVER,"opengles");// easy to be extended to other drivers,Follow the "How to Extend" section below

SDL_Window* gpWindow = SDL_CreateWindow("", 0, 0, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
SDL_Renderer* gpRenderer = SDL_CreateRenderer(gpWindow, -1, SDL_RENDERER_ACCELERATED );
```
2. Create Spine Atlas and SkeletonData like this:
```java 
atlas=Atlas_createFromFile((char*)atlas_path.c_str(),0);//read atlas from file
skeletonData=readSkeletonBinaryData((char*)binary_path.c_str(), atlas, default_sclae);//using binary format
or skeletonData=readSkeletonJsonData((char*)binary_path.c_str(), atlas, default_sclae);//using json format
```
3. Create SkeletonDrawable and RenderStates then set up basic animation like this:
```java 
SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
Skeleton* skeleton = drawable->skeleton;
skeleton->x = 0;
skeleton->y = 0;
Skeleton_updateWorldTransform(skeleton);
drawable->tryAnimation("youranimation",true,false);
RenderStates* defaultState=new RenderStates(your_own_SDL_Window,your_own_SDL_Renderer);
```
4. Update and render it :
```java 
float delta=0.01666666f;
SDL_SetRenderDrawBlendMode(defaultState->renderer, spine::SkeletonDrawable::sdl_blend_normal);
while(running)
{
   SDL_SetRenderDrawColor(defaultState->renderer, 255, 255, 255, 255);
   SDL_RenderFillRect(defaultState->renderer,NULL);
   drawable->update(delta);
   drawable->draw(defaultState);
   SDL_RenderPresent(defaultState->renderer);
   SDL_Delay(delta*1000);
}
//remember to clear up memory after usage
Atlas_dispose(atlas);
SkeletonData_dispose(skeletonData);
```
# How to Extend 

Here are basic steps of how to extend to "opengl" and "opengles2":

Step 1: Based on the current "SDL_RenderCopySpine" fuction in 'SDL_render_gles.c' and 'SDL_render_gl.c' implement a new one for target opengl platform (main function is to draw the spine GL_TRIANGLES Vertex (positions,colors,texture coordinates), for more details ,please read the VertexArray::draw() function);

Step 2: Before create any SDL_Window and SDL_Renderer,setup Correct GL Version and Driver by:
```java
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
SDL_SetHint( SDL_HINT_RENDER_DRIVER,"target_opengl_platform");
```	
	
