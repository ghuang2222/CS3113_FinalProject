#include "LevelA.h"
#include "Utility.h"

#define LOG(argument) std::cout << argument << '\n'
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 6

// CONSTANTS
constexpr int BULLET_COUNT = LEVEL_WIDTH * LEVEL_HEIGHT;

unsigned int LEVELA_DATA[] =
{
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

};

//VARIABLES
unsigned int g_next_bullet = 0;
GLuint g_font_texture_id;



LevelA::~LevelA()
{
    delete[] m_game_state.enemies;
    delete[] m_game_state.bullets;
    delete    m_game_state.player;
    delete    m_game_state.map;
    delete[]    m_game_state.icons;
    delete m_game_state.house;
    for (Entity* ptr : m_game_state.non_enemies) {delete ptr;}
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeChunk(m_game_state.lose_sfx);
    Mix_FreeChunk(m_game_state.win_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    
    m_sun = 200.0f;
    m_game_state.next_scene_id = -1;

   
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint map_texture_id = Utility::load_texture("assets/tileset2.png");
    //Map
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 7, 7  );

    //House setup
    GLuint house_texture_id = Utility::load_texture(HOUSE_FILEPATH);
    m_game_state.house = new Entity();
    m_game_state.house->set_texture_id(house_texture_id);
    m_game_state.house->set_position(glm::vec3(-3.8f, -2.0f, 0.0f));
    m_game_state.house->set_scale(glm::vec3(8.25f, 8.25f, 0.0f));
    //Player setup
    int player_walking_animation[4][4] =
    {

        { 8, 11, 13, 15 }, //left 
        { 0, 3, 5, 7 },  // right
        { 24, 27, 29, 31 }, //up
        { 16, 19, 21, 23 } //down
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    g_font_texture_id = Utility::load_texture(FONT_FILEPATH);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        3.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        4,                         // animation row amount
        0.6f,                      // width
        1.0f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_game_state.player->set_health(PLAYER_HEALTH);
    m_game_state.non_enemies.push_back(m_game_state.player);
    
    //Bullet setup
    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    m_game_state.bullets = new Entity[BULLET_COUNT];
    for (int i = 0; i < BULLET_COUNT; ++i)
    {
        m_game_state.bullets[i] = Entity(bullet_texture_id, 3.0f, 0.5f, 0.5f, BULLET);
        //m_game_state.bullets[i].deactivate();
        m_game_state.bullets[i].set_position(glm::vec3(0.0f, 1000.0f, 0.0f));
        m_game_state.bullets[i].set_bullet_distance(PEASHOOTER_RANGE);
        m_game_state.bullets[i].set_damage(BULLET_DAMAGE);
        m_game_state.non_enemies.push_back(&m_game_state.bullets[i]);
    }
    
 
    
    //Plant setup
    GLuint peashooter_texture_id = Utility::load_texture(PEASHOOTER_FILEPATH);
    GLuint sunflower_texture_id = Utility::load_texture(SUNFLOWER_FILEPATH);
    GLuint wallnut_texture_id = Utility::load_texture(WALLNUT_FILEPATH);
    
    
   
    // Enemy setup

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 0.25f, 1.0f, 1.0f, ENEMY, WALKER, WALKING);
        m_game_state.enemies[i].set_position(glm::vec3(14.0f,  -1.0f * i, 0.0f));
        m_game_state.enemies[i].set_damage(0.5f);
        m_game_state.enemies[i].activate();
    }

    // UI setup
    m_game_state.icons = new Entity[UNIQUE_PLANTS];
    m_game_state.icons[0] = Entity(sunflower_texture_id, 0.0f, 1.0f, 1.0f, ICON); 
    m_game_state.icons[0].set_position(glm::vec3(-1.5f, -3.0f, 0.0f));
    m_game_state.icons[1] = Entity(peashooter_texture_id, 0.0f, 1.0f, 1.0f, ICON);
    m_game_state.icons[1].set_position(glm::vec3(0.0f, -3.0f, 0.0f));
    m_game_state.icons[2] = Entity(wallnut_texture_id, 0.0f, 1.0f, 1.0f, ICON);
    m_game_state.icons[2].set_position(glm::vec3(1.5f, -3.0f, 0.0f));

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/I Got a Stick.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/click.wav");
    m_game_state.lose_sfx = Mix_LoadWAV("assets/Zombies Eat Brains.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/Cheer.wav");
}


void LevelA::update(float delta_time)
{
    if (m_game_state.win_game) m_game_state.next_scene_id = 2; //level1  -> level2
    m_game_state.enemies_killed = 0; //reset for each count
    //passive sun
    m_sun += 0.5f * delta_time;
    //cooldowns
    m_peashooter_timer -= delta_time;
    m_peashooter_timer *= m_peashooter_timer >= 0;
    m_sunflower_timer -= delta_time;
    m_sunflower_timer *= m_sunflower_timer >= 0;
    m_wallnut_timer -= delta_time;
    m_wallnut_timer *= m_wallnut_timer >= 0;

    m_game_state.house->update(delta_time, m_game_state.player, nullptr, 0, nullptr);
    //player
    m_game_state.player->update(delta_time, m_game_state.player,
                                m_game_state.enemies, 
                                ENEMY_COUNT,
                                nullptr);
   
    
    //non enemies
    for (int i = 0; i < m_game_state.non_enemies.size(); ++i) {
        m_game_state.non_enemies[i]->update(delta_time, m_game_state.player,
                       m_game_state.enemies, ENEMY_COUNT, nullptr);
       
        //acquire bullet if shooter doesn't have one
        if (m_game_state.non_enemies[i]->get_ai_type() == SHOOTER
            && m_game_state.non_enemies[i]->get_bullet() == nullptr)
        {
            m_game_state.non_enemies[i]->set_bullet(m_game_state.bullets,
                g_next_bullet,
                BULLET_COUNT);
            ++g_next_bullet;
        }  
        if (m_game_state.non_enemies[i]->get_entity_type() == PLANT
            && m_game_state.non_enemies[i]->get_health() <= 0)
        {
            float plant_x = m_game_state.non_enemies[i]->get_position().x,
                  plant_y = m_game_state.non_enemies[i]->get_position().y;
            float plant_hash = hash_function(plant_x, plant_y);
            m_game_state.plant_map[plant_hash] = false;
        }
        if (m_game_state.non_enemies[i]->get_ai_type() == SUNFLOWER) { m_sun += SUN_PER_SEC * delta_time; }
    }
    //UI
    for (int i = 0; i < UNIQUE_PLANTS; ++i)
    {
        m_game_state.icons[i].update(delta_time, m_game_state.player, nullptr, 0, nullptr);
    }
    
    //ENEMIES
    for (int i = 0; i < ENEMY_COUNT; ++i) 
    { 
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.non_enemies, 
                                       m_game_state.non_enemies.size(), nullptr);
        // Lose condition
        if (m_game_state.enemies[i].get_position().x <= -1 || m_game_state.player->get_health() <= 0
            &&  m_game_state.lose_game == false)
        {
            m_game_state.lose_game = true;
            Mix_PlayChannel(-1, m_game_state.lose_sfx, 0);


        }
        if (m_game_state.enemies[i].get_health() <= 0) { ++m_game_state.enemies_killed; }
    }
    //win condition
    if (m_game_state.enemies_killed == ENEMY_COUNT && m_game_state.win_game == false ) 
    { 
        m_game_state.win_game = true; 
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
    }
}

void LevelA::render(ShaderProgram* program)
{
    m_game_state.map->render(program);
    
    Utility::draw_text(program, g_font_texture_id,
        std::to_string((int)m_sun), 0.25f, 0.05f,
        glm::vec3(m_game_state.player->get_position().x,
            m_game_state.player->get_position().y + 0.5f, 0.0f));
    
    for (int i = 0; i < m_game_state.non_enemies.size(); ++i) 
    { m_game_state.non_enemies[i]->render(program); }
    
    for (int i = 0; i < ENEMY_COUNT; ++i) { m_game_state.enemies[i].render(program); }
    m_game_state.house->render(program);
    m_game_state.player->render(program);
    if (m_sunflower_timer == 0) m_game_state.icons[0].render(program);
    if (m_peashooter_timer == 0) m_game_state.icons[1].render(program);
    if (m_wallnut_timer == 0) m_game_state.icons[2].render(program);

    if (m_game_state.lose_game)
    {
        Utility::draw_text(program, g_font_texture_id,
            "YOU LOSE!", 0.5f, 0.05f,
            glm::vec3(m_game_state.player->get_position().x - 2.0f,
                m_game_state.player->get_position().y + 0.5f, 0.0f));
    }
    else if (m_game_state.win_game)
    {
        Utility::draw_text(program, g_font_texture_id,
            "YOU WIN!", 0.5f, 0.05f,
            glm::vec3(m_game_state.player->get_position().x - 2.0f,
                m_game_state.player->get_position().y + 0.5f, 0.0f));
        
    }
   
    Utility::draw_text(program, g_font_texture_id,
        "LEVEL 1", 0.35f, 0.05f,
        glm::vec3(m_game_state.player->get_position().x - 2.0f,
            m_game_state.player->get_position().y + 2.5f, 0.0f));


}