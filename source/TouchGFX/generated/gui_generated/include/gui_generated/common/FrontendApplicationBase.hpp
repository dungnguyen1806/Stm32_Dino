/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef FRONTENDAPPLICATIONBASE_HPP
#define FRONTENDAPPLICATIONBASE_HPP

#include <mvp/MVPApplication.hpp>
#include <gui/model/Model.hpp>

class FrontendHeap;

class FrontendApplicationBase : public touchgfx::MVPApplication
{
public:
    FrontendApplicationBase(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplicationBase() { }

    virtual void changeToStartScreen()
    {
        gotoScreen1ScreenNoTransition();
    }

    // Screen1
    void gotoScreen1ScreenNoTransition();

    // Screen2
    void gotoScreen2ScreenWipeTransitionSouth();

    void gotoScreen2ScreenWipeTransitionEast();

    void gotoScreen2ScreenNoTransition();

    // Screen3
    void gotoScreen3ScreenSlideTransitionEast();

protected:
    touchgfx::Callback<FrontendApplicationBase> transitionCallback;
    FrontendHeap& frontendHeap;
    Model& model;

    // Screen1
    void gotoScreen1ScreenNoTransitionImpl();

    // Screen2
    void gotoScreen2ScreenWipeTransitionSouthImpl();

    void gotoScreen2ScreenWipeTransitionEastImpl();

    void gotoScreen2ScreenNoTransitionImpl();

    // Screen3
    void gotoScreen3ScreenSlideTransitionEastImpl();
};

#endif // FRONTENDAPPLICATIONBASE_HPP
