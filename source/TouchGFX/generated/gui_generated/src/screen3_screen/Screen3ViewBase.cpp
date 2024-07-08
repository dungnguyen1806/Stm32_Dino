/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/screen3_screen/Screen3ViewBase.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>

Screen3ViewBase::Screen3ViewBase()
{
    __background.setPosition(0, 0, 320, 240);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    add(__background);

    bg.setXY(-17, 0);
    bg.setBitmap(touchgfx::Bitmap(BITMAP_BG_ID));
    add(bg);

    ground.setXY(0, 186);
    ground.setBitmap(touchgfx::Bitmap(BITMAP_GROUND0_ID));
    add(ground);

    button1.setXY(142, 144);
    button1.setBitmaps(touchgfx::Bitmap(BITMAP_RETRY_ID), touchgfx::Bitmap(BITMAP_RETRY_ID));
    add(button1);

    gameover.setPosition(45, 74, 231, 80);
    g.setXY(37, 15);
    g.setBitmap(touchgfx::Bitmap(BITMAP_G_ID));
    gameover.add(g);

    a.setXY(57, 15);
    a.setBitmap(touchgfx::Bitmap(BITMAP_A_ID));
    gameover.add(a);

    m.setXY(77, 15);
    m.setBitmap(touchgfx::Bitmap(BITMAP_M_ID));
    gameover.add(m);

    e.setXY(97, 15);
    e.setBitmap(touchgfx::Bitmap(BITMAP_E_ID));
    gameover.add(e);

    o.setXY(122, 15);
    o.setBitmap(touchgfx::Bitmap(BITMAP_O_ID));
    gameover.add(o);

    v.setXY(141, 15);
    v.setBitmap(touchgfx::Bitmap(BITMAP_V_ID));
    gameover.add(v);

    e1.setXY(158, 15);
    e1.setBitmap(touchgfx::Bitmap(BITMAP_E_ID));
    gameover.add(e1);

    r.setXY(177, 15);
    r.setBitmap(touchgfx::Bitmap(BITMAP_R_ID));
    gameover.add(r);

    add(gameover);
}

Screen3ViewBase::~Screen3ViewBase()
{

}

void Screen3ViewBase::setupScreen()
{

}
