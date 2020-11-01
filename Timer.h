#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class Timer : public Actor
{
public:
    Timer(class Game* game);

    void OnUpdate(float deltaTime) override;

private:
    SpriteComponent* mSpriteComponent;
};