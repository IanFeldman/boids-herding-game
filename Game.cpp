#include "SDL2/SDL_image.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include "Game.h"
#include "Math.h"
#include "Random.h"
#include "Boid.h"
#include "Pen.h"
#include "Timer.h" 
#include "Tile.h"

Game::Game()
    :mRunning(true)
    ,mWindowSize({1200, 600})
    ,mCameraPos(Vector2::Zero)
{

}

bool Game::Initialize() {
    int initialized = SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO);
    if (initialized != 0) {
        return false;
    }
    mWindow = SDL_CreateWindow("Big Wingus' Herding Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowSize.x, mWindowSize.y, 0);
    if (mWindow == NULL) {
        return false;
    }
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    int imgInitialized = IMG_Init(IMG_INIT_PNG);
    if ((imgInitialized & IMG_INIT_PNG) != IMG_INIT_PNG) {
        return false;
    }
    if (mRenderer == NULL) {
        return false;
    }
    SDL_Surface* surface;
    surface = IMG_Load("Assets/sheep-face.png");
    SDL_SetWindowIcon(mWindow, surface);

    SDL_Surface* cursorSurf;
    cursorSurf = IMG_Load("Assets/cursor.png");
    SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurf, 8, 8);
    SDL_SetCursor(cursor);

    LoadData();
    mPreviousTime = SDL_GetTicks();
    Random::Init();
    
    return true;
};

void Game::Shutdown() {
    std::cout << "Final Score: " + std::to_string(mScore) << std::endl;
    UnloadData();
    IMG_Quit();
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
};

void Game::Runloop() {
    while (mRunning) {
        ProcessUpdate();
        UpdateGame();
        GenerateOutput();
    }
};

void Game::ProcessUpdate() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                mRunning = false;
                break;

            case SDL_MOUSEMOTION:
                mMousePos = Vector2(event.motion.x, event.motion.y);
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                
                case SDLK_RETURN:
                    if (!mNeeru)
                    {
                        for (Boid* b : mBoids)
                        {
                            b->GetComponent<SpriteComponent>()->SetTexture(GetTexture("Assets/neeru-small.png"));
                        }
                        mNeeru = true;
                    }
                    break;
                
                }
                break;
                
            default:
                break;
        }
    }
    
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_ESCAPE]) {
        mRunning = false;
    }
    
    std::vector<Actor*> copyActors = mActors;
    for (Actor* a : copyActors) {
        a->ProcessInput(keyboardState);
    }
}

void Game::UpdateGame() {
    while ((int)SDL_GetTicks() < mPreviousTime + 16) {}
    int currentTime = SDL_GetTicks();
    float deltaTime = (float)(currentTime - mPreviousTime) / 1000;
    if (deltaTime > 0.033f) {
        deltaTime = 0.033f;
    }
    mPreviousTime = currentTime;
    
    std::vector<Actor*> actors = mActors;
    for (Actor* a : actors) {
        a->Update(deltaTime);
    }
    
    std::vector<Actor*> tempActors;
    for (Actor* a : mActors) {
        if (a->GetState() == ActorState::Destroy) {
            tempActors.emplace_back(a);
        }
    }
    
    if (!tempActors.empty()) {
        for (Actor* tempActor : tempActors) {
            delete tempActor;
        }
    }

    // update timer
    float percentage = 1.2f - (float)(currentTime - mLastResetTime) / mRoundTime;
    float newScale = 0.25f * percentage;
    mTimer->SetScale(newScale);
    // end round
    if (currentTime - mLastResetTime >= mRoundTime)
    {
        mRoundTime *= 0.95f;
        SetPen();
        mLastResetTime = currentTime;

        mTimer->SetScale(0.25f);
    }
}

void Game::GenerateOutput() {
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mRenderer);
    
    for (SpriteComponent* sprite : mSprites) {
        if (sprite->IsVisible()) {
            sprite->Draw(mRenderer);
        }
    }
    
    SDL_RenderPresent(mRenderer);
}

void Game::AddActor(Actor* actor) {
    mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor) {
    std::vector<Actor*>::iterator it;
    it = std::find(mActors.begin(), mActors.end(), actor);
    
    if (it != mActors.end()) {
        mActors.erase(it);
    }
}

void Game::LoadData() {
    mMousePos = Vector2(-100.0f, -100.0f);

    // tiles
    for (int j = 64; j < mWindowSize.y; j += 128)
    {
        for (int i = 0; i < mWindowSize.x; i += 128)
        {
            Tile* t = new Tile(this);
            Vector2 tilePos = Vector2(i, j);
            t->SetPosition(tilePos);
            t->SetScale(2.0f);
        }
    }

    // sheep
    for (int j = 0; j < mWindowSize.y; j += 75)
    {
        for (int i = 0; i < mWindowSize.x; i += 75)
        {
            Boid* b = new Boid(this);
            Vector2 pos = Vector2(i, j);
            b->SetPosition(pos);
            mBoids.push_back(b);
        }
    }
    // wolves
    for (int j = 0; j < mWindowSize.y; j += 200)
    {
        for (int i = 0; i < mWindowSize.x; i += 400)
        {
            Boid* b = new Boid(this);
            b->GetComponent<SpriteComponent>()->SetTexture(GetTexture("Assets/eyeball.png"));
            b->SetScale(1.0f);
            Vector2 pos = Vector2(i, j);
            b->SetPosition(pos);
            mBaddies.push_back(b);
        }
    }

    Vector2 timerPos = Vector2(mWindowSize.x * 0.9f, mWindowSize.y * 0.15f);
    mTimer = new Timer(this);
    mTimer->SetScale(0.25f);
    mTimer->SetPosition(timerPos);

    Timer* timerCenter = new Timer(this);
    timerCenter->GetComponent<SpriteComponent>()->SetTexture(GetTexture("Assets/timer-center.png"));
    timerCenter->SetScale(0.24f);
    Vector2 centerOffset = Vector2(1.0f, 1.0f);
    timerCenter->SetPosition(timerPos);

    mLastResetTime = SDL_GetTicks();
    mPen = new Pen(this);
    SetPen();
}

void Game::UnloadData() {
    if (!mActors.empty()) {
        mActors.erase(mActors.begin(), mActors.end());
    }

    for (auto it = mTextureCache.begin(); it != mTextureCache.end(); it++) {
        SDL_DestroyTexture(it->second);
    }
    
    mTextureCache.clear();
}

SDL_Texture* Game::GetTexture(const char* fileName) {
    std::unordered_map<std::string, SDL_Texture*>::const_iterator got = mTextureCache.find(fileName);
    if (got != mTextureCache.end())
        return got->second;
    else {
        SDL_Surface* tempSurface = IMG_Load(fileName);
        SDL_Texture* tempTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);

        SDL_FreeSurface(tempSurface);
        mTextureCache.emplace(fileName, tempTexture);
        return tempTexture;
    }
}

void Game::AddSprite(SpriteComponent* sprite) {
    
    mSprites.emplace_back(sprite);
    std::sort(mSprites.begin(), mSprites.end(),
              [](SpriteComponent* a, SpriteComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });
}

void Game::RemoveSprite(SpriteComponent* sprite) {
    std::vector<SpriteComponent*>::iterator it;
    it = std::find(mSprites.begin(), mSprites.end(), sprite);
    
    if (it != mSprites.end()) {
        mSprites.erase(it);
    }
}

void Game::SetCameraPos(Vector2 position) {
    mCameraPos = position;
}

void Game::SetPen()
{
    // check what boids are in pen
    float offset = (mPen->GetScale() * 64.0f) / 2.0f;
    Vector2 pPos = mPen->GetPosition();
    std::vector<Boid*> caughtBoids;
    float tempScore = 0;
    bool baddies = false;
    for (Boid* b : mBoids)
    {
        Vector2 bPos = b->GetPosition();
        if (bPos.x > pPos.x - offset && bPos.x < pPos.x + offset)
        {
            if (bPos.y > pPos.y - offset && bPos.y < pPos.y + offset)
            {
                tempScore += 1;
                caughtBoids.push_back(b);
            }
        }
    }
    // check baddies
    for (Boid* b : mBaddies)
    {
        if (!baddies)
        {
            Vector2 bPos = b->GetPosition();
            if (bPos.x > pPos.x - offset && bPos.x < pPos.x + offset)
            {
                if (bPos.y > pPos.y - offset && bPos.y < pPos.y + offset)
                {
                    baddies = true;
                }
            }
        }
    }
    // baddies = true if they are in pen
    if (!baddies)
        mScore += tempScore;
    else
    {
        for (Boid* b : caughtBoids)
        {
            // erase from mBoids
            std::vector<Boid*>::iterator it;
            it = std::find(mBoids.begin(), mBoids.end(), b);
            if (it != mBoids.end())
                mBoids.erase(it);
            // pause them
            b->SetState(ActorState::Paused);
            b->SetRotation(0.0f);
            b->SetScale(0.5f);
            b->GetComponent<SpriteComponent>()->SetTexture(GetTexture("Assets/skull.png"));
        }
    }

    // check if game over
    if (mBoids.size() == 0)
    {
        mRunning = false;
        return;
    }

    // reset pen
    float scale = Random::GetFloatRange(2.0f, 6.0f); // max * 64 has to be less than mWindowSize.y
    mPen->SetScale(scale);

    // sprite is 64x64
    offset = (scale * 64.0f) / 2.0f;
    float posX = Random::GetFloatRange(0.0f + offset, (float)mWindowSize.x - offset);
    float posY = Random::GetFloatRange(0.0f + offset, (float)mWindowSize.y - offset);

    mPen->SetPosition(Vector2(posX, posY));
}
