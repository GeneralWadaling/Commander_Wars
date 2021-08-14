#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/closure/closure.h"
#include "3rd_party/oxygine-framework/oxygine/core/Restorable.h"
#include "3rd_party/oxygine-framework/oxygine/core/Texture.h"
#include <QOpenGLShader>
#include <QtGlobal>

namespace oxygine
{
    class NativeTexture;
    using spNativeTexture = intrusive_ptr<NativeTexture>;
    class NativeTexture: public Texture, public Restorable
    {
    public:
        explicit NativeTexture() = default;
        virtual ~NativeTexture() = default;
        virtual void init(GLuint, qint32 w, qint32 h, ImageData::TextureFormat tf) = 0;
        virtual void init(qint32 w, qint32 h, ImageData::TextureFormat tf) = 0;
        virtual void init(const ImageData& src) = 0;
        //virtual void release() = 0;
        virtual GLuint getId() = 0;
        virtual ImageData lock(lock_flags, const Rect* src) = 0;
        virtual void unlock() = 0;
        virtual void swap(NativeTexture*) = 0;

        /**Disabled/Enables bilinear texture filter*/
        virtual void setLinearFilter(quint32 filter) = 0;
        /**set wrap mode as clamp2edge or repeat (tiling)*/
        virtual void setClamp2Edge(bool clamp2edge) = 0;
        virtual void updateRegion(qint32 x, qint32 y, const ImageData& data) = 0;
        virtual void apply(const Rect* rect = 0) = 0;
        /**returns handle (ptr) to HW texture ID*/
        virtual GLuint getHandle() const = 0;
        static QVector<spNativeTexture> getCreatedTextures();        
    };
}
