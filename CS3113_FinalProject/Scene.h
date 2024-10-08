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


constexpr float PLAYER_HEALTH = 35.0f;

constexpr float PEASHOOTER_COST = 100.0f,
                SUNFLOWER_COST = 50.0f,
                WALLNUT_COST = 50.0f;

constexpr float PEASHOOTER_RECHARGE = 4.25f,
                SUNFLOWER_RECHARGE = 1.25f,
                WALLNUT_RECHARGE = 3.0f;

constexpr float PEASHOOTER_HEALTH = 100.0f,
                SUNFLOWER_HEALTH = 50.0f,
                WALLNUT_HEALTH = 600.0f;

constexpr float PEASHOOTER_RANGE = 14.0f;

constexpr float BULLET_DAMAGE = 12.0f;
constexpr float SUN_PER_SEC = 50.0f / 6.0f; //rate at which sunflowers produce sun

constexpr char SPRITESHEET_FILEPATH[] = "assets/player.png",
PEASHOOTER_FILEPATH[] = "assets/peashooter.png",
SUNFLOWER_FILEPATH[] = "assets/sunflower.png",
WALLNUT_FILEPATH[] = "assets/wallnut.png",
ENEMY_FILEPATH[] = "assets/zombie.png",
BULLET_FILEPATH[] = "assets/circle.png",
HOUSE_FILEPATH[] = "assets/house.png",
FONT_FILEPATH[] = "assets/font1.png";

constexpr int UNIQUE_PLANTS = 3;
/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ����� GAME OBJECTS ����� //
    Map* map;
    Entity* house;
    Entity* player;
    Entity* bullets;
    std::vector<Entity*> non_enemies;
    std::unordered_map<float, bool> plant_map;
    Entity* enemies;
    Entity* icons;
    bool win_game = false;
    bool lose_game = false;
    int enemies_killed = 0;

    // ����� AUDIO ����� //
    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;
    Mix_Chunk* lose_sfx;
    Mix_Chunk* win_sfx;

    // ����� POINTERS TO OTHER SCENES ����� //
    int next_scene_id;
};

class Scene {
protected:
    GameState m_game_state;

public:
    // ����� ATTRIBUTES ����� //
    float m_number_of_enemies = 1;

    float m_peashooter_timer = 0.0f;
    float m_sunflower_timer = 0.0f;
    float m_wallnut_timer = 0.0f;
    float m_sun = 100.0f;

    // ����� METHODS ����� //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    float hash_function(float x_coord, float y_coord);
    bool plant_it(GLuint texture_id, AIType ai_type, 
                  float x_coord, float y_coord,
                  float min_x, float max_x, float min_y, float max_y);
    // ����� GETTERS ����� //
    GameState const get_state() const { return m_game_state; }
    
    int const get_number_of_enemies() const { return m_number_of_enemies; }
    //SETTERS
    void set_scene_id(int next_scene_id) { m_game_state.next_scene_id = next_scene_id; }
    
};