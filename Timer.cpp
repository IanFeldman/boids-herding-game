#include "Timer.h"
#include "Game.h"

Timer::Timer(Game* game)
    :Actor(game)
{
    mSpriteComponent = new SpriteComponent(this, 150);
    mSpriteComponent->SetTexture(mGame->GetTexture("Assets/timer.png"));
    SetScale(1.0f);
}

void Timer::OnUpdate(float deltaTime)
{

}