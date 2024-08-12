#include "LevelA.h"
#include "Utility.h"

#define LOG(argument) std::cout << argument << '\n'
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 6

// CONSTANTS
constexpr int BULLET_COUNT = LEVEL_WIDTH * LEVEL_HEIGHT;

unsigned int LEVELA_DATA[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

};

//VARIABLES
unsigned int g_next_bullet = 0;

LevelA::~LevelA()
{
    delete[] m_game_state.enemies;
    delete[] m_game_state.bullets;
    delete    m_game_state.player;
    delete    m_game_state.map;

    for (Entity* ptr : m_game_state.non_enemies) {delete ptr;}
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        3.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_game_state.player->set_health(100.0f);
    m_game_state.non_enemies.push_back(m_game_state.player);
    
    //Bullet setup
    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    m_game_state.bullets = new Entity[BULLET_COUNT];
    for (int i = 0; i < BULLET_COUNT; ++i)
    {
        m_game_state.bullets[i] = Entity(bullet_texture_id, 3.0f, 0.5f, 0.5f, BULLET);
        //m_game_state.bullets[i].deactivate();
        m_game_state.bullets[i].set_position(glm::vec3(0.0f, 1000.0f, 0.0f));
        m_game_state.bullets[i].set_bullet_distance(5.0f);
        m_game_state.bullets[i].set_damage(10.0f);
        m_game_state.non_enemies.push_back(&m_game_state.bullets[i]);
    }
    
 
    
    //Plant setup
    GLuint plant_texture_id = enemy_texture_id;
    /*Entity* peashooter = new Entity(plant_texture_id, 0.0f, 1.0f, 1.0f, PLANT, SHOOTER, IDLE);
    peashooter->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    peashooter->set_movement(glm::vec3(0.0f));
    peashooter->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    peashooter->set_bullet(m_game_state.bullets, g_next_bullet, BULLET_COUNT);
    ++g_next_bullet;
    m_game_state.plant_map[0] = peashooter;
    m_game_state.non_enemies.push_back(peashooter);*/
   
    // Enemy setup

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, WALKING);
        m_game_state.enemies[i].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
        m_game_state.enemies[i].set_damage(0.0f);
    }



    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/Who Likes to Party.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(0.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/click.wav");
}

void LevelA::update(float delta_time)
{
    m_peashooter_timer -= delta_time;
    m_peashooter_timer *= m_peashooter_timer >= 0;



    m_game_state.player->update(delta_time, m_game_state.player,
                                m_game_state.enemies, 
                                ENEMY_COUNT,
                                nullptr);
    
    for (int i = 0; i < m_game_state.non_enemies.size(); ++i) {
        m_game_state.non_enemies[i]->update(delta_time, m_game_state.player,
                       m_game_state.enemies, ENEMY_COUNT, nullptr);
       
        if (m_game_state.non_enemies[i]->get_ai_type() == SHOOTER
            && m_game_state.non_enemies[i]->get_bullet() == nullptr)
        {
            m_game_state.non_enemies[i]->set_bullet(m_game_state.bullets,
                g_next_bullet,
                BULLET_COUNT);
            ++g_next_bullet;
        }        
    }
 
    for (int i = 0; i < ENEMY_COUNT; ++i) 
    { 
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.non_enemies, 
                                       m_game_state.non_enemies.size(), nullptr);
    }
    
}

void LevelA::render(ShaderProgram* program)
{
    m_game_state.map->render(program);
    
    for (int i = 0; i < m_game_state.non_enemies.size(); ++i) 
    { m_game_state.non_enemies[i]->render(program); }

    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; ++i) { m_game_state.enemies[i].render(program); }
}