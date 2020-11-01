#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "SDL2/SDL.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "Math.h"
#include "Boid.h"
#include "Pen.h"
#include "Timer.h"

class Game {
    
public:
    Game();
    bool Initialize(void);
    void Shutdown(void);
    void Runloop(void);
    void ProcessUpdate(void);
    void UpdateGame(void);
    void GenerateOutput(void);

    SDL_Point GetWindowSize(){ return mWindowSize; };
    
    void AddActor(Actor* actor);
    void RemoveActor(Actor* actor);
    
    SDL_Texture* GetTexture(const char* fileName);
    
    void AddSprite(SpriteComponent* sprite);
    void RemoveSprite(SpriteComponent* sprite);
    
    Vector2 GetCameraPos() { return mCameraPos; }
    void SetCameraPos(Vector2 position);

    Vector2 GetMousePos() { return mMousePos; }

    std::vector<Boid*> GetBoids() { return mBoids; }

    void SetPen();
    
private:
    void LoadData(void);
    void UnloadData(void);
    
protected:
    bool mRunning;
    int mPreviousTime;
    int mPlayChannel;
    SDL_Point mWindowSize;
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    std::vector<Actor*> mActors;
    std::unordered_map<std::string, SDL_Texture*> mTextureCache;
    std::vector<SpriteComponent*> mSprites;
    Vector2 mCameraPos;
    Vector2 mMousePos;
    Game* game;
    std::vector<Boid*> mBoids;
    std::vector<Boid*> mBaddies;
    bool mNeeru = false;
    Pen* mPen;
    Timer* mTimer;
    int mLastResetTime;
    float mRoundTime = 10000.0f;
    int mScore = 0;
};