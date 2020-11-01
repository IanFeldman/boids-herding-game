#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "Random.h"

class Tile : public Actor
{
public:
    Tile(class Game* game);

private:
    SpriteComponent* mSpriteComponent;
};