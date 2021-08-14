#include "3rd_party/oxygine-framework/oxygine/actor/MaskedSprite.h"
#include "3rd_party/oxygine-framework/oxygine/MaskedRenderer.h"
#include "3rd_party/oxygine-framework/oxygine/RenderDelegate.h"
#include "3rd_party/oxygine-framework/oxygine/RenderState.h"

namespace oxygine
{
    Sprite* MaskedSprite::getMask() const
    {
        return m_mask.get();
    }

    bool    MaskedSprite::getUseRChannel() const
    {
        return m_useRChannel;
    }

    void MaskedSprite::setMask(spSprite mask, bool useRChannel)
    {
        m_mask = mask;
        m_useRChannel = useRChannel;
    }


    void MaskedSprite::render(const RenderState& parentRS)
    {
        m_rdelegate->render(this, parentRS);
    }
}
