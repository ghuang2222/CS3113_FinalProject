#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"


constexpr float PEASHOOTER_COST = 100.0f,
                SUNFLOWER_COST = 50.0f,
                WALLNUT_COST = 50.0f;

constexpr float PEASHOOTER_RECHARGE = 5.0f,
                SUNFLOWER_RECHARGE = 2.0f,
                WALLNUT_RECHARGE = 3.0f;


constexpr char SPRITESHEET_FILEPATH[] = "assets/george_0.png",
PEASHOOTER_FILEPATH[] = "assets/peashooter.png",
ENEMY_FILEPATH[] = "assets/square.png",
BULLET_FILEPATH[] = "assets/circle.png";


/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map* map;
    Entity* player;
    Entity* bullets;
    std::vector<Entity*> non_enemies;
    std::unordered_map<float, bool> plant_map;
    Entity* enemies;


   

    // ————— AUDIO ————— //
    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;

    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
};

class Scene {
protected:
    GameState m_game_state;

public:
    // ————— ATTRIBUTES ————— //
    float m_number_of_enemies = 1;

    float m_peashooter_timer = 0.0f;
    float m_sunflower_timer = 0.0f;
    float m_wallnut_timer = 0.0f;
    float m_sun = 200.0f;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    float hash_function(float x_coord, float y_coord);
    void plant_it(GLuint texture_id, AIType ai_type, float x_coord, float y_coord);
    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state; }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
};