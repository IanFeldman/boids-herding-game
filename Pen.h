#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class Pen : public Actor
{
public:
    Pen(class Game* game);

    void OnUpdate(float deltaTime) override;

private:
    SpriteComponent* mSpriteComponent;
};