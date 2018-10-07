/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 *  \file SDL_opengles.h
 *
 *  This is a simple file to encapsulate the OpenGL ES 1.X API headers.
 */
#include "SDL_config.h"
#include "SDL_video.h"

#ifdef __IPHONEOS__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef SDL_OPENGL_ES1_STRUCTS
#define SDL_OPENGL_ES1_STRUCTS

typedef struct GLES_FBOList GLES_FBOList;

struct GLES_FBOList
{
    Uint32 w, h;
    GLuint FBO;
    GLES_FBOList *next;
};

typedef struct
{
    SDL_GLContext context;
    struct {
        Uint32 color;
        SDL_BlendMode blendMode;
        SDL_bool tex_coords;
        SDL_bool col_array;
    } current;
    
#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;
#define SDL_PROC_OES SDL_PROC
    SDL_PROC(void, glBindTexture, (GLenum, GLuint))
    SDL_PROC(void, glBlendFunc, (GLenum, GLenum))
    SDL_PROC_OES(void, glBlendEquationOES, (GLenum))
    SDL_PROC_OES(void, glBlendEquationSeparateOES, (GLenum, GLenum))
    SDL_PROC_OES(void, glBlendFuncSeparateOES, (GLenum, GLenum, GLenum, GLenum))
    SDL_PROC(void, glClear, (GLbitfield))
    SDL_PROC(void, glClearColor, (GLclampf, GLclampf, GLclampf, GLclampf))
    SDL_PROC(void, glColor4f, (GLfloat, GLfloat, GLfloat, GLfloat))
    SDL_PROC(void, glDeleteTextures, (GLsizei, const GLuint *))
    SDL_PROC(void, glDisable, (GLenum))
    SDL_PROC(void, glDisableClientState, (GLenum array))
    SDL_PROC(void, glDrawArrays, (GLenum, GLint, GLsizei))
    SDL_PROC_OES(void, glDrawTexfOES, (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat))
    SDL_PROC(void, glEnable, (GLenum))
    SDL_PROC(void, glEnableClientState, (GLenum))
    SDL_PROC(void, glFinish, (void))
    SDL_PROC_OES(void, glGenFramebuffersOES, (GLsizei, GLuint *))
    SDL_PROC(void, glGenTextures, (GLsizei, GLuint *))
    SDL_PROC(GLenum, glGetError, (void))
    SDL_PROC(void, glGetIntegerv, (GLenum, GLint *))
    SDL_PROC(void, glLoadIdentity, (void))
    SDL_PROC(void, glMatrixMode, (GLenum))
    SDL_PROC(void, glOrthof, (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat))
    SDL_PROC(void, glPixelStorei, (GLenum, GLint))
    SDL_PROC(void, glReadPixels, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*))
    SDL_PROC(void, glScissor, (GLint, GLint, GLsizei, GLsizei))
    SDL_PROC(void, glTexCoordPointer, (GLint, GLenum, GLsizei, const GLvoid *))
    SDL_PROC(void, glTexEnvf, (GLenum, GLenum, GLfloat))
    SDL_PROC(void, glTexImage2D, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))
    SDL_PROC(void, glTexParameteri, (GLenum, GLenum, GLint))
    SDL_PROC(void, glTexParameteriv, (GLenum, GLenum, const GLint *))
    SDL_PROC(void, glTexSubImage2D, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))
    SDL_PROC(void, glVertexPointer, (GLint, GLenum, GLsizei, const GLvoid *))
    SDL_PROC(void, glViewport, (GLint, GLint, GLsizei, GLsizei))
    SDL_PROC_OES(void, glBindFramebufferOES, (GLenum, GLuint))
    SDL_PROC_OES(void, glFramebufferTexture2DOES, (GLenum, GLenum, GLenum, GLuint, GLint))
    SDL_PROC_OES(GLenum, glCheckFramebufferStatusOES, (GLenum))
    SDL_PROC(void, glPushMatrix, (void))
    SDL_PROC(void, glTranslatef, (GLfloat, GLfloat, GLfloat))
    SDL_PROC(void, glRotatef, (GLfloat, GLfloat, GLfloat, GLfloat))
    SDL_PROC(void, glPopMatrix, (void))
    SDL_PROC_OES(void, glDeleteFramebuffersOES, (GLsizei, const GLuint*))
    SDL_PROC(void, glColorPointer, (GLint, GLenum, GLsizei, const GLvoid *))
    SDL_PROC(void, glFlush, (void))
#undef SDL_PROC
#undef SDL_PROC_OES
    SDL_bool GL_OES_framebuffer_object_supported;
    GLES_FBOList *framebuffers;
    GLuint window_framebuffer;
    
    SDL_bool GL_OES_blend_func_separate_supported;
    SDL_bool GL_OES_blend_equation_separate_supported;
    SDL_bool GL_OES_blend_subtract_supported;
} GLES_RenderData;

typedef struct
{
    GLuint texture;
    GLenum type;
    GLfloat texw;
    GLfloat texh;
    GLenum format;
    GLenum formattype;
    void *pixels;
    int pitch;
    GLES_FBOList *fbo;
} GLES_TextureData;

#endif
