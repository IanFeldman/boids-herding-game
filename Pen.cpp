#include "Pen.h"
#include "Game.h"

Pen::Pen(Game* game)
    :Actor(game)
{
    mSpriteComponent = new SpriteComponent(this, 100);
    mSpriteComponent->SetTexture(mGame->GetTexture("Assets/pen.png"));
    SetScale(1.0f);
}

void Pen::OnUpdate(float deltaTime)
{

}