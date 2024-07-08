/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <simulator/mainBase.hpp>
#include <platform/hal/simulator/sdl2/HALSDL2.hpp>
#include <common/TouchGFXInit.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>
#include <touchgfx/widgets/canvas/CWRVectorRenderer.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/VectorFontRendererImpl.hpp>
#include <string.h>

#ifdef __GNUC__
#define fopen_s(pFile, filename, mode) (((*(pFile)) = fopen((filename), (mode))) == NULL)
#endif
touchgfx::LCD16bpp lcd;

namespace touchgfx
{
    VectorRenderer* VectorRenderer::getInstance()
    {
        static CWRVectorRendererRGB565 renderer;
        return &renderer;
    }
}

VectorFontRendererImpl vectorFontRendererImpl;

void setupSimulator(int argc, char** argv, touchgfx::HAL& hal)
{
    // Simulate hardware running at 60Hz generating a vsync every 16.6667 ms
    static_cast<touchgfx::HALSDL2&>(hal).setVsyncInterval(16.6667f);
    static_cast<touchgfx::HALSDL2&>(hal).setWindowTitle("dinosaurus");

    // Initialize SDL
    bool sdl_init_result = static_cast<touchgfx::HALSDL2&>(hal).sdl_init(argc, argv);
    assert(sdl_init_result && "Error during SDL initialization");
    HAL::lcd().setVectorFontRenderer(&vectorFontRendererImpl);
}

touchgfx::LCD& setupLCD()
{
    return lcd;
}
