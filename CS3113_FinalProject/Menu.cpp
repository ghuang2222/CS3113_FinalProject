#include "Menu.h"
#include "Utility.h"

GLuint g_font_texture_id_menu;
Menu::~Menu()
{
    delete[] m_game_state.enemies;
    delete[] m_game_state.bullets;
    delete    m_game_state.player;
    delete    m_game_state.map;
    delete[]    m_game_state.icons;
    for (Entity* ptr : m_game_state.non_enemies) { delete ptr; }
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Menu::initialise()
{ 
    m_game_state.next_scene_id = -1;
    GLuint player_texture_id = Utility::load_texture(BULLET_FILEPATH);
    g_font_texture_id_menu = Utility::load_texture(FONT_FILEPATH);
    m_game_state.player = new Entity(player_texture_id, 0.0f, 0.0f, 0.0f, PLAYER);
}

void Menu::update(float delta_time){}

void Menu::render(ShaderProgram* program)
{   
    Utility::draw_text(program, g_font_texture_id_menu, "Press ENTER",
        0.25f, 0.05f, glm::vec3(-3.0f, 0.0f, 0.0f));
}