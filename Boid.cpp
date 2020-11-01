#include "Boid.h"
#include "Game.h"

Boid::Boid(Game* game)
    :Actor(game)
{
    mSpriteComponent = new SpriteComponent(this, 120);
    // pick texture
    int randInt = Random::GetIntRange(0, 2);
    switch (randInt)
    {
    case 0:
        mSpriteComponent->SetTexture(mGame->GetTexture("Assets/sheep.png"));
        break;
    case 1:
        mSpriteComponent->SetTexture(mGame->GetTexture("Assets/cow.png"));
        break;
    case 2:
        mSpriteComponent->SetTexture(mGame->GetTexture("Assets/pig.png"));
        break;
    default:
        break;
    }

    SetScale(0.75f);
    mWindowSize.x = mGame->GetWindowSize().x;
    mWindowSize.y = mGame->GetWindowSize().y;
}

void Boid::OnUpdate(float deltaTime)
{
    CalculateVelocity();

    // loop around edges
    Vector2 pos = mPosition + mVelocity * deltaTime;
    if (pos.x < -25)
        pos.x = mWindowSize.x + 25;
    else if (pos.x > mWindowSize.x + 25)
        pos.x = -25;
    if (pos.y < -25)
        pos.y = mWindowSize.y + 25;
    else if (pos.y > mWindowSize.y + 25)
        pos.y = -25;
    // set pos
    SetPosition(pos);
    
    SetRotation(CalculateAngle());
}

void Boid::CalculateVelocity()
{
    mNeighbors.clear();

    // find nearby boids
    for (Boid* b : mGame->GetBoids())
    {
        if (b != this)
        {
            float dist = (b->GetPosition() - mPosition).Length();
            if (dist < mRadius)
                mNeighbors.push_back(b);
        }
    }

    // add rules
    Vector2 coherence = Vector2::Zero;
    Vector2 separation = Vector2::Zero;
    Vector2 alignment = Vector2::Zero;
    Vector2 avoidance = Vector2::Zero;

    for (Boid* b : mNeighbors)
    {
        // coherence (flying towards other boids)
        Vector2 toBoid = b->GetPosition() - mPosition;
        coherence += toBoid * mCoherenceFac;

        // separation (avoids other boids)
        if (toBoid.Length() < mSepDist)
            separation += Vector2::Normalize(toBoid) * -mSeparationFac;

        // alignment (match dir of other boids)
        Vector2 otherVel = b->GetVelocity();
        alignment += otherVel;

        // avoidance (avoid mouse)
        Vector2 toCursor = mGame->GetMousePos() - mPosition;
        if (toCursor.Length() < mAvoidanceRadius)
            avoidance = Vector2::Normalize(toCursor) * -mAvoidanceFac;
    }
    if (alignment.x != 0.0f && alignment.y != 0.0f)
        alignment = Vector2::Normalize(alignment) * mAlignmentFac;

    // set accel
    mAccel = coherence + separation + alignment + avoidance;
    // set vel
    mVelocity += mAccel;
    // adjust speed
    if (mVelocity.Length() > 200.0f)
        mVelocity *= 0.9f;
}

float Boid::CalculateAngle()
{
    float angle = atan2(abs(mVelocity.y), abs(mVelocity.x));
    // quadrantals (special cases)
    if (mVelocity.x == 0.0f && mVelocity.y != 0.0f)
    {
        if (mVelocity.y > 0.0f) // (0, -1) bc the y is inverted
            angle = Math::TwoPi - Math::PiOver2; // 3pi/2
        else // (0, 1)
            angle = Math::PiOver2;
    }
    else if (mVelocity.x != 0.0f && mVelocity.y == 0.0f)
    {
        if (mVelocity.x > 0.0f) // (1, 0)
            angle = 0.0f;
        else // (-1, 0)
            angle = Math::Pi;
    }
    // finding quadrant for regular angles
    else if (mVelocity.x > 0.0f && mVelocity.y > 0.0f) // quad IV
        angle = (2.0f * Math::Pi) - angle;
    else if (mVelocity.x < 0.0f && mVelocity.y > 0.0f) // quad III
        angle = Math::Pi + angle;
    else if (mVelocity.x < 0.0f && mVelocity.y < 0.0f) // quad II
        angle = Math::Pi - angle;
    else if (mVelocity.x > 0.0f && mVelocity.y < 0.0f) /// quad I
        angle = angle;

    return (angle - Math::PiOver2); // always subtract pi / 2 bc the computer thinks straight up is 0 radians
}