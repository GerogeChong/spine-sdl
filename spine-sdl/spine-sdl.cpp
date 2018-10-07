/*
 * By GeorgeChong 2018
*/
#include "spine-sdl.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengles.h"
#include "SDL_render.h"

#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif

_SP_ARRAY_IMPLEMENT_TYPE(spColorArray, spColor)

void _AtlasPage_createTexture (AtlasPage* self, const char* path){
    
    SDL_Surface* img=IMG_Load(path);
    if (!img) return;

    /* Replace your own SDL_Renderer here! */
    SDL_Texture* texture=SDL_CreateTextureFromSurface(YOUR_OWN_SDL_Renderer, img);

    self->rendererObject = texture;
    self->width = img->w;
    self->height = img->h;
    SDL_FreeSurface(img);
}

void _AtlasPage_disposeTexture (AtlasPage* self){
    if(self->rendererObject!=NULL)
        SDL_DestroyTexture((SDL_Texture*)self->rendererObject);
}

char* _Util_readFile (const char* path, int* length){
   
    char *data;
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) return 0;
    
    SDL_RWseek(file, 0, SEEK_END);
    *length = (int)SDL_RWtell(file);
    SDL_RWseek(file, 0, SEEK_SET);
    
    data = MALLOC(char, *length);
    SDL_RWread(file,data, 1, *length);
    SDL_RWclose(file);
    
    return data;
    
}


/**/
namespace spine {
    
    SDL_BlendMode SkeletonDrawable::sdl_blend_normal=SDL_ComposeCustomBlendMode(
                                                                                SDL_BLENDFACTOR_SRC_ALPHA,
                                                                                SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                SDL_BLENDOPERATION_ADD,
                                                                                SDL_BLENDFACTOR_SRC_ALPHA,
                                                                                SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                SDL_BLENDOPERATION_ADD);
    
    SDL_BlendMode SkeletonDrawable::sdl_blend_additive=SDL_ComposeCustomBlendMode(
                                                                                  SDL_BLENDFACTOR_SRC_ALPHA,
                                                                                  SDL_BLENDFACTOR_ONE,
                                                                                  SDL_BLENDOPERATION_ADD,
                                                                                  SDL_BLENDFACTOR_SRC_ALPHA,
                                                                                  SDL_BLENDFACTOR_ONE,
                                                                                  SDL_BLENDOPERATION_ADD);
    
    SDL_BlendMode SkeletonDrawable::sdl_blend_multiply=SDL_ComposeCustomBlendMode(
                                                                                  SDL_BLENDFACTOR_DST_COLOR,
                                                                                  SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                  SDL_BLENDOPERATION_ADD,
                                                                                  SDL_BLENDFACTOR_DST_COLOR,
                                                                                  SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                  SDL_BLENDOPERATION_ADD);
    
    SDL_BlendMode SkeletonDrawable::sdl_blend_screen=SDL_ComposeCustomBlendMode(
                                                                                SDL_BLENDFACTOR_ONE,
                                                                                SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
                                                                                SDL_BLENDOPERATION_ADD,
                                                                                SDL_BLENDFACTOR_ONE,
                                                                                SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
                                                                                SDL_BLENDOPERATION_ADD);
    
    SDL_BlendMode SkeletonDrawable::sdl_blend_normalPma = SDL_ComposeCustomBlendMode(
                                                                                     SDL_BLENDFACTOR_ONE,
                                                                                     SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                     SDL_BLENDOPERATION_ADD,
                                                                                     SDL_BLENDFACTOR_ONE,
                                                                                     SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                     SDL_BLENDOPERATION_ADD);
    SDL_BlendMode SkeletonDrawable::sdl_blend_additivePma = SDL_ComposeCustomBlendMode(
                                                                                       SDL_BLENDFACTOR_ONE,
                                                                                       SDL_BLENDFACTOR_ONE,
                                                                                       SDL_BLENDOPERATION_ADD,
                                                                                       SDL_BLENDFACTOR_ONE,
                                                                                       SDL_BLENDFACTOR_ONE,
                                                                                       SDL_BLENDOPERATION_ADD);
    SDL_BlendMode SkeletonDrawable::sdl_blend_multiplyPma = SDL_ComposeCustomBlendMode(
                                                                                       SDL_BLENDFACTOR_DST_COLOR,
                                                                                       SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                       SDL_BLENDOPERATION_ADD,
                                                                                       SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                       SDL_BLENDFACTOR_ONE,
                                                                                       SDL_BLENDOPERATION_ADD);
    
    SDL_BlendMode SkeletonDrawable::sdl_blend_screenPma = SDL_ComposeCustomBlendMode(
                                                                                     SDL_BLENDFACTOR_ONE,
                                                                                     SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
                                                                                     SDL_BLENDOPERATION_ADD,
                                                                                     SDL_BLENDFACTOR_ONE,
                                                                                     SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
                                                                                     SDL_BLENDOPERATION_ADD);
    
    SkeletonDrawable::SkeletonDrawable (SkeletonData* skeletonData, AnimationStateData* stateData)
    {
        this->skeletonData=skeletonData;
        timeScale=1,
        vertexArray=new VertexArray(Triangles, skeletonData->bonesCount * 4);
        
        //spSwirlVertexEffect* newEffect=spSwirlVertexEffect_create(1);
        //vertexEffect=&newEffect->super;
        vertexEffect=0;
        
        worldVertices=0;
        clipper=0;
        
        Bone_setYDown(true);
        worldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);
        skeleton = Skeleton_create(skeletonData);
        tempUvs = spFloatArray_create(16);
        tempColors = spColorArray_create(16);
        
        ownsAnimationStateData = stateData == 0;
        if (ownsAnimationStateData) stateData = AnimationStateData_create(skeletonData);
        
        state = AnimationState_create(stateData);
        
        state->listener=SkeletonDrawable::listenAnimations;
        
        clipper = spSkeletonClipping_create();
        
        //pre multiply alpha or not
        usePremultipliedAlpha=false;
    }
    
    SkeletonDrawable::~SkeletonDrawable () {
        delete vertexArray;
        FREE(worldVertices);
        if (ownsAnimationStateData) AnimationStateData_dispose(state->data);
        AnimationState_dispose(state);
        Skeleton_dispose(skeleton);
        spSkeletonClipping_dispose(clipper);
        spFloatArray_dispose(tempUvs);
        spColorArray_dispose(tempColors);
       
    }
    
    SkeletonData* SkeletonDrawable::readSkeletonJsonData (const char* filename, Atlas* atlas, float scale) {
        SkeletonJson* json = SkeletonJson_create(atlas);
        json->scale = scale;
        SkeletonData* skeletonData = SkeletonJson_readSkeletonDataFile(json, filename);
        if (!skeletonData) {
            printf("%s\n", json->error);
            exit(0);
        }
        SkeletonJson_dispose(json);
        return skeletonData;
    }
    
    SkeletonData* SkeletonDrawable::readSkeletonBinaryData (const char* filename, Atlas* atlas, float scale) {
        SkeletonBinary* binary = SkeletonBinary_create(atlas);
        binary->scale = scale;
        SkeletonData *skeletonData = SkeletonBinary_readSkeletonDataFile(binary, filename);
        if (!skeletonData) {
            printf("%s\n", binary->error);
            exit(0);
        }
        SkeletonBinary_dispose(binary);
        return skeletonData;
    }
    spTrackEntry* SkeletonDrawable::tryAnimation(char* animation,bool resetdrawstatus,bool repeat)
    {
        spTrackEntry* entry=NULL;
        if(SkeletonData_findAnimation(skeletonData,animation))
        {
            if(resetdrawstatus)
            {
                Skeleton_setBonesToSetupPose(skeleton);
                Skeleton_setSlotsToSetupPose(skeleton);
                AnimationState_clearTracks(state);
            }

            spTrackEntry* entry= AnimationState_setAnimationByName(state, 0, animation, repeat);
            if(entry)
            {
                entry->listener=SkeletonDrawable::listenAnimations;
                entry->userData=this;
            }
        }
        return entry;
    }
    
    //static listen animation status
    void SkeletonDrawable::listenAnimations(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event)
    {
        switch(type)
        {
            case SP_ANIMATION_START:
            {
                
            }
                break;
            case SP_ANIMATION_COMPLETE:
            {
                
            }
                break;
            default:
                
                break;
        }
    }
    
    void SkeletonDrawable::update (float deltaTime) {
        Skeleton_update(skeleton, deltaTime);
        AnimationState_update(state, deltaTime * timeScale);
        AnimationState_apply(state, skeleton);
        Skeleton_updateWorldTransform(skeleton);
    }
    
    void SkeletonDrawable::draw (RenderStates* states) 
    {
        vertexArray->clear();
        states->texture = 0;
        unsigned short quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
        
        if (vertexEffect != 0)
            vertexEffect->begin(vertexEffect, skeleton);
        
        Vertex vertex;
        Texture* texture = 0;
        
        for (int i = 0; i < skeleton->slotsCount; ++i) {
            Slot* slot = skeleton->drawOrder[i];
            Attachment* attachment = slot->attachment;
            if (!attachment) continue;
            
            float* vertices = worldVertices;
            int verticesCount = 0;
            float* uvs = 0;
            unsigned short* indices = 0;
            int indicesCount = 0;
            spColor* attachmentColor;
            
            if (attachment->type == ATTACHMENT_REGION) {
                RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
                spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, vertices, 0, 2);
                verticesCount = 4;
                uvs = regionAttachment->uvs;
                indices = quadIndices;
                indicesCount = 6;
                texture = (Texture*)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
                attachmentColor = &regionAttachment->color;
                
            } else if (attachment->type == ATTACHMENT_MESH) {
                MeshAttachment* mesh = (MeshAttachment*)attachment;
                if (mesh->super.worldVerticesLength > SPINE_MESH_VERTEX_COUNT_MAX) continue;
                texture = (Texture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject;
                spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, worldVertices, 0, 2);
                verticesCount = mesh->super.worldVerticesLength >> 1;
                uvs = mesh->uvs;
                indices = mesh->triangles;
                indicesCount = mesh->trianglesCount;
                attachmentColor = &mesh->color;
            } else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
                spClippingAttachment* clip = (spClippingAttachment*)slot->attachment;
                spSkeletonClipping_clipStart(clipper, slot, clip);
                continue;
            } else continue;
            
            Uint8 r = static_cast<Uint8>(skeleton->color.r * slot->color.r * attachmentColor->r * 255);
            Uint8 g = static_cast<Uint8>(skeleton->color.g * slot->color.g * attachmentColor->g * 255);
            Uint8 b = static_cast<Uint8>(skeleton->color.b * slot->color.b * attachmentColor->b * 255);
            Uint8 a = static_cast<Uint8>(skeleton->color.a * slot->color.a * attachmentColor->a * 255);
            vertex.color.r = r;
            vertex.color.g = g;
            vertex.color.b = b;
            vertex.color.a = a;
            
            spColor light;
            light.r = r / 255.0f;
            light.g = g / 255.0f;
            light.b = b / 255.0f;
            light.a = a / 255.0f;
            
            SDL_BlendMode blend;
            if (!usePremultipliedAlpha) {
                switch (slot->data->blendMode) {
                    case BLEND_MODE_NORMAL:
                        blend = SkeletonDrawable::sdl_blend_normal;
                        break;
                    case BLEND_MODE_ADDITIVE:
                        blend = SkeletonDrawable::sdl_blend_additive;
                        break;
                    case BLEND_MODE_MULTIPLY:
                        blend = SkeletonDrawable::sdl_blend_multiply;
                        break;
                    case BLEND_MODE_SCREEN:
                        blend = SkeletonDrawable::sdl_blend_screen;
                        break;
                    default:
                        blend = SkeletonDrawable::sdl_blend_normal;
                        break;
                }
            } else {
                switch (slot->data->blendMode) {
                    case BLEND_MODE_NORMAL:
                        blend = SkeletonDrawable::sdl_blend_normalPma;
                        break;
                    case BLEND_MODE_ADDITIVE:
                        blend = SkeletonDrawable::sdl_blend_additivePma;
                        break;
                    case BLEND_MODE_MULTIPLY:
                        blend = SkeletonDrawable::sdl_blend_multiplyPma;
                        break;
                    case BLEND_MODE_SCREEN:
                        blend = SkeletonDrawable::sdl_blend_screenPma;
                        break;
                    default:
                        blend = SkeletonDrawable::sdl_blend_normalPma;
                        break;
                }
            }
            
            if (states->texture == 0) states->texture = texture;
            
            if (states->blendMode != blend || states->texture != texture) {
                //SDL——Rendering
                vertexArray->draw(states);
                vertexArray->clear();
                states->blendMode = blend;
                states->texture = texture;
            }
            
            if (spSkeletonClipping_isClipping(clipper)) {
                spSkeletonClipping_clipTriangles(clipper, vertices, verticesCount << 1, indices, indicesCount, uvs, 2);
                vertices = clipper->clippedVertices->items;
                verticesCount = clipper->clippedVertices->size >> 1;
                uvs = clipper->clippedUVs->items;
                indices = clipper->clippedTriangles->items;
                indicesCount = clipper->clippedTriangles->size;
            }
            
            if (vertexEffect != 0) {
                spFloatArray_clear(tempUvs);
                spColorArray_clear(tempColors);
                for (int i = 0; i < verticesCount; i++) {
                    spColor vertexColor = light;
                    spColor dark;
                    dark.r = dark.g = dark.b = dark.a = 0;
                    int index = i << 1;
                    float x = vertices[index];
                    float y = vertices[index + 1];
                    float u = uvs[index];
                    float v = uvs[index + 1];
                    vertexEffect->transform(vertexEffect, &x, &y, &u, &v, &vertexColor, &dark);
                    vertices[index] = x;
                    vertices[index + 1] = y;
                    spFloatArray_add(tempUvs, u);
                    spFloatArray_add(tempUvs, v);
                    spColorArray_add(tempColors, vertexColor);
                }
                
                for (int i = 0; i < indicesCount; ++i) {
                    int index = indices[i] << 1;
                    vertex.position.x = vertices[index];
                    vertex.position.y = vertices[index + 1];
                    vertex.texCoords.x = uvs[index];
                    vertex.texCoords.y = uvs[index + 1];
                    spColor vertexColor = tempColors->items[index >> 1];
                    vertex.color.r = static_cast<Uint8>(vertexColor.r * 255);
                    vertex.color.g = static_cast<Uint8>(vertexColor.g * 255);
                    vertex.color.b = static_cast<Uint8>(vertexColor.b * 255);
                    vertex.color.a = static_cast<Uint8>(vertexColor.a * 255);
                    vertexArray->append(vertex);
                }
            } else {
                for (int i = 0; i < indicesCount; ++i) {
                    int index = indices[i] << 1;
                    vertex.position.x = vertices[index];
                    vertex.position.y = vertices[index + 1];
                    vertex.texCoords.x = uvs[index];
                    vertex.texCoords.y = uvs[index + 1];
                    vertexArray->append(vertex);
                }
            }
            
            spSkeletonClipping_clipEnd(clipper, slot);
        }
        
        //SDL——Rendering
        vertexArray->draw(states);
        
        spSkeletonClipping_clipEnd2(clipper);
        
        if (vertexEffect != 0)
            vertexEffect->end(vertexEffect);
    }
    
    
    void VertexArray::draw(RenderStates* states)
    {
        PrimitiveType type=this->m_primitiveType;
        unsigned int firstVertex=0;
        std::size_t vertexCount=m_vertices.size();
        if(vertexCount==0)
            return ;//Nothing to draw

        
        //set BLEND MODE
        if(states->texture)
        {
            SDL_SetTextureBlendMode(states->texture, states->blendMode);
        }

        const char* src_data = reinterpret_cast<const char*>(&m_vertices[0]);
        
        SDL_RenderCopySpine(states->renderer, states->texture, type, sizeof(Vertex), firstVertex, (unsigned int)vertexCount, src_data);
       
    }

    void VertexArray::clear()
    {
        m_vertices.clear();
    }
    
    Vertex::Vertex()
    {
        
    }

    Vector2f::Vector2f(){};
    Vector2i::Vector2i(){};
    Vector2u::Vector2u(){};
    Vector2f::Vector2f(float ax,float ay):x(ax),y(ay){};
    Vector2i::Vector2i(int ax,int ay):x(ax),y(ay){};
    Vector2u::Vector2u(unsigned int ax,unsigned int ay):x(ax),y(ay){};
    
    VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount)
    :m_primitiveType(type),m_vertices(vertexCount)
    {
        
    }
    
    void VertexArray::append(const Vertex& vertex)
    {
        m_vertices.push_back(vertex);
    }
    
    RenderStates::RenderStates(SDL_Window* awd,SDL_Renderer* ard):window(awd),renderer(ard)
    {
        
    }
    RenderStates::~RenderStates()
    {
        
    }
    
} /* namespace spine */
