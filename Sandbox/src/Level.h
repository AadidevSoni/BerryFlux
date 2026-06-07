#pragma once 

#include "Player.h"

struct Pillar
{
    glm::vec3 TopPosition = { 0.0f, 10.0f, 0.0f };
    glm::vec2 TopScale = { 3.0f, 12.0f };

    glm::vec3 BottomPosition = { 10.0f, 10.0f, 0.0f };
    glm::vec2 BottomScale = { 3.0f, 12.0f };
};

class Level {
  public:
    void Init();

    void OnUpdate(BerryFlux::Timestep ts);
    void OnRender();

    void OnImGuiRender();
    
    bool IsGameOver() const { return m_GameOver; }
    void Reset();

    Player& GetPlayer() { return m_Player; }
  private:
    void CreatePillar(int index, float offset);
    bool CollisionTest();

    void GameOver();
  private:
    Player m_Player;
    bool m_GameOver = false;

    //Next x-position where a pillar recycle should happen
    float m_PillarTarget = 30.0f;

    //Which pillar will be reused next. Old onces are recycled
    int m_PillarIndex = 0;

    //Animate pillar colors through the rainbow.

    //Stores all Pillars
    std::vector<Pillar> m_Pillars;

    //Texture used for obstacle triangles
    BerryFlux::Ref<BerryFlux::Texture2D> m_PipeTexture;
    BerryFlux::Ref<BerryFlux::Texture2D> m_BackgroundTexture;
};