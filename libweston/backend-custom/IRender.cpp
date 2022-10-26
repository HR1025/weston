#include "IRender.h"
#include "Render/PixmanRender.h"

namespace weston
{

std::shared_ptr<IRender> IRender::Create(RenderType renderType)
{
    if (renderType == RenderType::PIXMAN)
    {
        return std::make_shared<PixmanRender>();
    }
    else
    {
        return nullptr;
    }
}

} // namespace weston

