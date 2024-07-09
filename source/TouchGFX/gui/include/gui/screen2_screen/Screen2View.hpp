#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void dinoJump();
    virtual void handleJoystickEvent(uint32_t);
    virtual void checkScore();
    virtual bool checkCollision();
//    virtual int16_t rand();
//    virtual void writeHighestScore(int);
    virtual int readHighestScore();
    uint32_t tick;
    uint32_t Rx_Data[5];
    int counter; // score

    int SCREEN_WIDTH;
    int BASE_HEIGHT; // original Y pos of dino

    int currentHeight; // amount of deviation from BASE_HEIGHT
    bool isJumping; // flag to detect jump
    int timeJump; // check time jump
    
    int enemySpeed; // number of pixels each enemy will move per tick
    int enemyPositionX; // horizontal position of enemies, from -50(out of screen) to 320(start appearing)
    uint currentEnemy; // current enemy type, 0 is cactus, 1 is bird

    // int seed; // seed for random enemy
    // int jumpDis; // distance for dino to jump for each tick, negative means going up, positive means going down

    // int MAX_HEIGHT; // maximum Y pos of dino when jumping
    // uint32_t obsNumber; // number of enemies, used to increase enemy speed
    // int BASE_BIRD_HEIGHT; // base height of bird, used to add variation to bird's Y position

    int highestScore;
protected:
};

#endif // SCREEN2VIEW_HPP
