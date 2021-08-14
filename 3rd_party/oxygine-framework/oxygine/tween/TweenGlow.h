#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/PostProcess.h"

namespace oxygine
{
    class TweenGlowImpl;
    using spTweenGlowImpl = intrusive_ptr<TweenGlowImpl>;

    class TweenGlowImpl : public TweenPostProcess
    {
    public:
        explicit TweenGlowImpl(const QColor& c, const PostProcessOptions& opt);
        virtual ~TweenGlowImpl() = default;
        virtual void render(Actor* actor, const RenderState& rs) override;
        virtual void _renderPP() override;
    private:
        QColor m_color;
        qint32 m_downsample;
    };

    class TweenGlow : public TweenProxy<TweenGlowImpl>
    {
    public:
        explicit TweenGlow(const QColor& color, const PostProcessOptions& opt = PostProcessOptions());
        virtual ~TweenGlow() = default;
    };
}
