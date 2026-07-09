#include "Level.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace BerryFlux;

// HSVtoRGB
static glm::vec4 HSVtoRGB(const glm::vec3 &hsv) {
  int H = (int)(hsv.x * 360.0f);
  double S = hsv.y;
  double V = hsv.z;

  double C = S * V;
  double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
  double m = V - C;
  double Rs, Gs, Bs;

  if (H >= 0 && H < 60) {
    Rs = C;
    Gs = X;
    Bs = 0;
  } else if (H >= 60 && H < 120) {
    Rs = X;
    Gs = C;
    Bs = 0;
  } else if (H >= 120 && H < 180) {
    Rs = 0;
    Gs = C;
    Bs = X;
  } else if (H >= 180 && H < 240) {
    Rs = 0;
    Gs = X;
    Bs = C;
  } else if (H >= 240 && H < 300) {
    Rs = X;
    Gs = 0;
    Bs = C;
  } else {
    Rs = C;
    Gs = 0;
    Bs = X;
  }

  return {(Rs + m), (Gs + m), (Bs + m), 1.0f};
}

static bool PointInTri(const glm::vec2 &p, glm::vec2 &p0, const glm::vec2 &p1,
                       const glm::vec2 &p2) {
  float s =
      p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y;
  float t =
      p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y;

  if ((s < 0) != (t < 0))
    return false;

  float A =
      -p1.y * p2.x + p0.y * (p2.x - p1.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y;

  return A < 0 ? (s <= 0 && s + t >= A) : (s >= 0 && s + t <= A);
}

void Level::Init() {
  m_PipeTexture = Texture2D::Create("assets/textures/Pipe.png");
  m_BackgroundTexture = BerryFlux::Texture2D::Create("assets/textures/bg.png");
  m_Player.LoadAssets();

  // Create 5 pillars at position 0,10,20,30,40
  m_Pillars.resize(5);
  for (int i = 0; i < 5; i++)
    CreatePillar(i, i * 10.0f);
}

void Level::OnUpdate(BerryFlux::Timestep ts) {
  m_Player.OnUpdate(ts);

  if (CollisionTest()) {
    GameOver();
    return;
  }

  // Recycle Pillars
  if (m_Player.GetPosition().x > m_PillarTarget) {
    CreatePillar(m_PillarIndex, m_PillarTarget + 20.0f);
    m_PillarIndex = ++m_PillarIndex % m_Pillars.size();
    m_PillarTarget += 10.0f;
  }
}

void Level::OnRender() {
  const auto &playerPos = m_Player.GetPosition();

  // Background
  // Renderer2D::DrawQuad({ playerPos.x, 0.0f, -0.8f }, { 50.0f, 50.0f }, {
  // 0.3f, 0.3f, 0.3f, 1.0f });
  Renderer2D::DrawQuad({playerPos.x, 0.0f, -0.8f}, {35.0f, 21.0f},
                       m_BackgroundTexture);

  // Floor and ceiling
  // Renderer2D::DrawQuad({ playerPos.x,  34.0f }, { 50.0f, 50.0f }, color);
  // Renderer2D::DrawQuad({ playerPos.x, -34.0f }, { 50.0f, 50.0f }, color);

  for (auto &pillar : m_Pillars) {
    Renderer2D::DrawQuad(pillar.TopPosition, pillar.TopScale,
                         glm::radians(180.0f), m_PipeTexture);
    Renderer2D::DrawQuad(pillar.BottomPosition, pillar.BottomScale, 0.0f,
                         m_PipeTexture);
  }

  m_Player.OnRender();
}

// Generates random obstacle positions.
void Level::CreatePillar(int index, float offset) {
  Pillar &pillar = m_Pillars[index];
  // Places pillar ahead.
  pillar.TopPosition.x = offset;
  pillar.BottomPosition.x = offset;
  // Used for render ordering.
  pillar.TopPosition.z = index * 0.1f - 0.5f;
  pillar.BottomPosition.z = index * 0.1f - 0.5f + 0.05f;

  // Random Center
  float center = Random::Float() * 35.0f - 17.5f;
  float gap = 2.0f + Random::Float() * 5.0f; // Random Gap

  pillar.TopPosition.y =
      10.0f - ((10.0f - center) * 0.2f) + gap * 0.5f; // Position Top Pillar
  pillar.BottomPosition.y = -10.0f - ((-10.0f - center) * 0.2f) -
                            gap * 0.5f; // Position Bottom Pillar
}

bool Level::CollisionTest() {
  const auto &playerPos = m_Player.GetPosition();

  float cameraY = glm::clamp(playerPos.y, -2.0f, 2.0f);

  if (playerPos.y > cameraY + 8.0f)
    return true;

  if (playerPos.y < cameraY - 8.0f)
    return true;

  // Only kill on ground hit
  if (playerPos.y < -8.0f)
    return true;

  // Bird hitbox (slightly smaller than sprite)
  glm::vec2 birdSize = {0.7f, 0.9f};

  for (auto &p : m_Pillars) {
    glm::vec2 topPipeSize = p.TopScale;
    glm::vec2 bottomPipeSize = p.BottomScale;

    bool overlapTop = std::abs(playerPos.x - p.TopPosition.x) <
                          (birdSize.x * 0.5f + topPipeSize.x * 0.5f) &&
                      std::abs(playerPos.y - p.TopPosition.y) <
                          (birdSize.y * 0.5f + topPipeSize.y * 0.5f);

    bool overlapBottom = std::abs(playerPos.x - p.BottomPosition.x) <
                             (birdSize.x * 0.5f + bottomPipeSize.x * 0.5f) &&
                         std::abs(playerPos.y - p.BottomPosition.y) <
                             (birdSize.y * 0.5f + bottomPipeSize.y * 0.5f);

    if (overlapTop || overlapBottom)
      return true;
  }

  return false;
}

void Level::GameOver() { m_GameOver = true; }

void Level::Reset() {
  m_GameOver = false;

  m_Player.Reset();

  m_PillarTarget = 30.0f;
  m_PillarIndex = 0;
  for (int i = 0; i < 5; i++)
    CreatePillar(i, i * 10.0f);
}