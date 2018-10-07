/*
 * By GeorgeChong 2018
*/
#ifndef SPINE_SDL_H
#define SPINE_SDL_H

#define SPINE_SHORT_NAMES
#include "spine.h"
#include "extension.h"
#include "SDL.h"
#include <string>
#include <vector>

#ifdef __IPHONEOS__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

_SP_ARRAY_DECLARE_TYPE(spColorArray, spColor)

struct SDL_Renderer;
typedef SDL_Texture Texture;
typedef SDL_Color Color;

namespace spine {
    
    class RenderStates
    {
    public:
        RenderStates(SDL_Window* awindow,SDL_Renderer* arenderer);
        ~RenderStates();
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_BlendMode blendMode;
    };

    class Vector2i
    {
        public:
        Vector2i();
        Vector2i(int X, int Y);
        int x; ///< X coordinate of the vector
        int y; ///< Y coordinate of the vector
    };
    
    class Vector2u
    {
    public:
        Vector2u();
        Vector2u(unsigned int X, unsigned int Y);
        unsigned int x; ///< X coordinate of the vector
        unsigned int y; ///< Y coordinate of the vector
    };
    
    class Vector2f
    {
    public:
        Vector2f();
        Vector2f(float X, float Y);
        float x; ///< X coordinate of the vector
        float y; ///< Y coordinate of the vector
    };
    
    

    class Vertex
    {
    public:
        
        ////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        ////////////////////////////////////////////////////////////
        Vertex();
        
        
        Vertex(const Vector2f& thePosition);
        Vertex(const Vector2f& thePosition, const Color& theColor);
        Vertex(const Vector2f& thePosition, const Vector2f& theTexCoords);
        Vertex(const Vector2f& thePosition, const Color& theColor, const Vector2f& theTexCoords);
        
        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        Vector2f  position;  
        Color    color;     
        Vector2f  texCoords; 
    };
    
    enum PrimitiveType
    {
        Points,        ///< List of individual points
        Lines,         ///< List of individual lines
        LineStrip,     ///< List of connected lines, a point uses the previous point to form a line
        Triangles,     ///< List of individual triangles
        TriangleStrip, ///< List of connected triangles, a point uses the two previous points to form a triangle
        TriangleFan,   ///< List of connected triangles, a point uses the common center and the previous point to form a triangle
    };
   
    class VertexArray
    {
    public:
        
        VertexArray();
        
        explicit VertexArray(PrimitiveType type, std::size_t vertexCount = 0);

        void clear();
        
        void append(const Vertex& vertex);
        
        void setPrimitiveType(PrimitiveType type);

        PrimitiveType getPrimitiveType() const;

    public:

        void draw(RenderStates* state);
        
    private:
        
        // Member data
        std::vector<Vertex> m_vertices;      ///< Vertices contained in the array
        PrimitiveType       m_primitiveType; ///< Type of primitives to draw
    };
    
 
        
 class SkeletonDrawable{
     
 public:
        static SDL_BlendMode sdl_blend_normal;
        static SDL_BlendMode sdl_blend_additive;
        static SDL_BlendMode sdl_blend_multiply;
        static SDL_BlendMode sdl_blend_screen;
     
        static SDL_BlendMode sdl_blend_normalPma;
        static SDL_BlendMode sdl_blend_additivePma;
        static SDL_BlendMode sdl_blend_multiplyPma;
        static SDL_BlendMode sdl_blend_screenPma;

        Atlas* atlas;
        SkeletonData* skeletonData;
     
        public:
            Skeleton* skeleton;
            AnimationState* state;
            float timeScale;
            VertexArray* vertexArray;
            spVertexEffect* vertexEffect;
     
            SkeletonDrawable (SkeletonData* skeleton, AnimationStateData* stateData = 0);
     
            ~SkeletonDrawable ();
     
            spTrackEntry* tryAnimation(char* animation,bool reset,bool repeat);
     
            void update (float deltaTime);
            
            void draw (RenderStates* states);
            
            void setUsePremultipliedAlpha(bool usePMA) { usePremultipliedAlpha = usePMA; };
            bool getUsePremultipliedAlpha() { return usePremultipliedAlpha; };
     
            static SkeletonData* readSkeletonJsonData (const char* filename, Atlas* atlas, float scale) ;
     
            static SkeletonData* readSkeletonBinaryData (const char* filename, Atlas* atlas, float scale);
     
            static void listenAnimations(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event);
    private:
            bool ownsAnimationStateData;
            float* worldVertices;
            spFloatArray* tempUvs;
            spColorArray* tempColors;
            spSkeletonClipping* clipper;
            bool usePremultipliedAlpha;
        };
        
    } /* namespace spine */

#endif
