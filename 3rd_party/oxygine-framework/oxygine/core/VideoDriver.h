#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/core/ref_counter.h"
#include "3rd_party/oxygine-framework/oxygine/core/vertex.h"
#include "3rd_party/oxygine-framework/oxygine/math/Matrix.h"
#include "3rd_party/oxygine-framework/oxygine/math/Rect.h"
#include "3rd_party/oxygine-framework/oxygine/math/Vector4.h"

namespace oxygine
{
    class IVideoDriver;
    using spIVideoDriver = oxygine::intrusive_ptr<IVideoDriver>;

    class IVideoDriver : public oxygine::ref_counter
    {
    public:

        enum PRIMITIVE_TYPE
        {
            PT_POINTS,
            PT_LINES,
            PT_LINE_LOOP,
            PT_LINE_STRIP,
            PT_TRIANGLES,
            PT_TRIANGLE_STRIP,
            PT_TRIANGLE_FAN,
            PT_COUNT,
        };


        class Stats
        {
        public:
            explicit Stats() : batches(0), start(0), duration(0) { memset(elements, 0, sizeof(elements)); }
            virtual  ~Stats() = default;
            qint32 batches;
            qint32 elements[PT_COUNT];
            timeMS start;
            timeMS duration;
        };
        static Stats m_stats;

        enum BLEND_TYPE
        {
            BT_ZERO,
            BT_ONE,
            BT_SRC_COLOR,
            BT_ONE_MINUS_SRC_COLOR,
            BT_SRC_ALPHA,
            BT_ONE_MINUS_SRC_ALPHA,
            BT_DST_ALPHA,
            BT_DST_COLOR,
            BT_ONE_MINUS_DST_ALPHA,
            BT_ONE_MINUS_DST_COLOR,
        };

        enum CULL_FACE_TYPE
        {
            CULL_FACE_FRONT_AND_BACK,
            CULL_FACE_FRONT,
            CULL_FACE_BACK,
        };

        enum STATE
        {
            STATE_BLEND,
            STATE_CULL_FACE,
            STATE_NUM
        };

        static spIVideoDriver instance;
        explicit IVideoDriver() = default;
        virtual ~IVideoDriver() = default;

        virtual void reset() = 0;
        virtual void restore() = 0;
        virtual bool isReady() const = 0;

        virtual spNativeTexture createTexture() = 0;

        virtual void clear(const QColor& color) = 0;
        virtual void begin(const Rect& viewport, const QColor* color) = 0;
        virtual void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, quint32 verticesDataSize) = 0;
        virtual void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, quint32 verticesDataSize, const unsigned short* indicesData, quint32 numIndices) = 0;


        virtual void            getViewport(Rect& r) const = 0;
        virtual bool            getScissorRect(Rect&) const = 0;
        virtual spNativeTexture getRenderTarget() const = 0;
        virtual ShaderProgram*  getShaderProgram() const = 0;
        virtual const VertexDeclaration* getVertexDeclaration(bvertex_format) const = 0;

        virtual void setScissorRect(const Rect*) = 0;
        virtual void setDefaultSettings() = 0;
        virtual void setViewport(const Rect& viewport) = 0;
        virtual void setRenderTarget(spNativeTexture) = 0;
        virtual void setShaderProgram(ShaderProgram*) = 0;
        virtual void setTexture(qint32 sampler, spNativeTexture) = 0;
        virtual void setState(STATE, quint32 value) = 0;
        virtual void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest) = 0;

        virtual void setUniform(const char* id, const Vector4* v, qint32 num) = 0;
        virtual void setUniform(const char* id, const Vector3* v, qint32 num) = 0;
        virtual void setUniform(const char* id, const Vector2* v, qint32 num) = 0;
        virtual void setUniform(const char* id, const Matrix* v, qint32 num) = 0;
        virtual void setUniform(const char* id, float v) = 0;
        virtual void setUniformInt(const char* id, qint32 v) = 0;

        void setUniform(const char* id, const Vector4& v);
        void setUniform(const char* id, const Vector3& v);
        void setUniform(const char* id, const Vector2& v);
        void setUniform(const char* id, const Matrix&  v);
    };
}
