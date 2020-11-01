#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "Random.h" 

class Boid : public Actor
{
public:
    Boid(class Game* game);

    void OnUpdate(float deltaTime) override;
    void CalculateVelocity();
    float CalculateAngle();

    Vector2 GetVelocity() { return mVelocity; }
    
private:
    SpriteComponent* mSpriteComponent;
    Vector2 mWindowSize;

    Vector2 mVelocity;
    Vector2 mAccel;

    std::vector<Boid*> mNeighbors;
    float mRadius = 200.0f;

    float mCoherenceFac = 0.001f;
    float mSepDist = 75.0f;
    float mSeparationFac = 1.0f;
    float mAlignmentFac = 1.0f;
    float mAvoidanceFac = 50.0f;
    float mAvoidanceRadius = 100.0f;
};