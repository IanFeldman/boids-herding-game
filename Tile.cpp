#include "Tile.h"
#include "Game.h"

Tile::Tile(Game* game)
    :Actor(game)
{
    mSpriteComponent = new SpriteComponent(this, 1);
    float randFloat = Random::GetFloat();
    if (randFloat <= 0.5f)
        mSpriteComponent->SetTexture(mGame->GetTexture("Assets/grass1.png"));
    else
    {
        int randInt = Random::GetIntRange(0, 2);
        switch(randInt)
        {
        case 0:
            mSpriteComponent->SetTexture(mGame->GetTexture("Assets/grass2.png"));
            break;
        case 1:
            mSpriteComponent->SetTexture(mGame->GetTexture("Assets/grass3.png"));
            break;
        case 2:
            mSpriteComponent->SetTexture(mGame->GetTexture("Assets/grass4.png"));
            break;
        default:
            break;
        }
    }
}