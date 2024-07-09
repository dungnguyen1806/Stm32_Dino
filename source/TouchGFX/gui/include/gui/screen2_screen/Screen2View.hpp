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
    virtual uint32_t randint();
    virtual void writeHighestScore(int);
    virtual int readHighestScore();
    uint32_t Rx_Data[5];

    int SCREEN_WIDTH;
    int BASE_DINO_Y; // original Y pos of dino
    int BASE_DINOC_Y; // original Y pos of dinoc
    int BASE_CACTUS_Y; // original Y pos of cactus
    int MIN_BIRD_Y; // minimum Y pos of bird
    int MAX_BIRD_Y; // maximum Y pos of bird
    int BASE_ENEMY_SPEED; // original speed of enemies

    bool isJumping; // flag to detect jump
    int currentDinoHeight; // amount of deviation from BASE_HEIGHT
    int timeDinoJump; // check time jump
    
    int enemySpeed; // number of pixels each enemy will move per tick
    int enemyPositionX; // horizontal position of enemies, from -50(out of screen) to 320(start appearing)
    uint currentEnemy; // current enemy type, 0 is cactus, 1 is bird

    uint32_t tick;
    int counter; // score
    int highestScore;
protected:
};

#endif // SCREEN2VIEW_HPP
