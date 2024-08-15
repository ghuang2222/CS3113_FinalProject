/**
* Author: Garvin Huang
* Assignment: Plants vs Zombies Clone
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 6
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"

#include "Effects.h"

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH = 900,
WINDOW_HEIGHT = 700;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
               F_GREYSCALE_SHADER_PATH[] = "shaders/fragment_greyscale.glsl";


constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ––––– GLOBAL VARIABLES ––––– //
Scene* g_current_scene;
Menu* g_menu;
LevelA* g_levelA;
LevelB* g_levelB;
LevelC* g_levelC;
//
//Effects* g_effects;
Scene* g_levels[4];

SDL_Window* g_display_window;


ShaderProgram g_shader_program;

glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;
GLuint g_peashooter_texture_id,
       g_sunflower_texture_id,
       g_wallnut_texture_id;

AppStatus g_app_status = RUNNING;

void switch_to_scene(Scene* scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
}

void initialise()
{
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Plantz vs Sombiez",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_menu = new Menu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();

    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    // Start at menu
    switch_to_scene(g_levels[0]);

   /* g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(SHRINK, 2.0f);*/

    g_peashooter_texture_id= Utility::load_texture(PEASHOOTER_FILEPATH);
    g_sunflower_texture_id = Utility::load_texture(SUNFLOWER_FILEPATH);
    g_wallnut_texture_id = Utility::load_texture(WALLNUT_FILEPATH);
}

void process_input()
{
    
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    glm::vec3 player_pos = g_current_scene->get_state().player->get_position();
    float player_pos_hash = g_current_scene->hash_function(round(player_pos.x), round(player_pos.y));
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;
            case SDLK_RETURN:
                if (g_menu != nullptr && g_current_scene == g_menu)
                {
                    g_current_scene->set_scene_id(1);
                }
                break;
            case SDLK_1:
                if (g_current_scene->m_sunflower_timer == 0.0f
                    && g_current_scene->m_sun >= SUNFLOWER_COST)
                {
                    if (g_current_scene->plant_it(g_sunflower_texture_id, SUNFLOWER, player_pos.x, player_pos.y,
                        0, LEVEL_WIDTH - 1, -1 * LEVEL_HEIGHT + 1, 0))
                    {
                        g_current_scene->m_sunflower_timer = SUNFLOWER_RECHARGE;
                        g_current_scene->m_sun -= SUNFLOWER_COST;
                    }

                }

                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                break;

            case SDLK_2:
                // Plant peashooter
                
                if (g_current_scene->m_peashooter_timer == 0.0f 
                    && g_current_scene->m_sun >= PEASHOOTER_COST) 
                {   
                    if (g_current_scene->plant_it(g_peashooter_texture_id, SHOOTER, player_pos.x, player_pos.y,
                        0, LEVEL_WIDTH - 1, -1 * LEVEL_HEIGHT + 1, 0))
                    {
                        g_current_scene->m_peashooter_timer = PEASHOOTER_RECHARGE;
                        g_current_scene->m_sun -= PEASHOOTER_COST;
                    }
                    
                }
                
                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                
                break;
            
            case SDLK_3:
                if (g_current_scene->m_wallnut_timer == 0.0f
                    && g_current_scene->m_sun >= WALLNUT_COST)
                {
                    if (g_current_scene->plant_it(g_wallnut_texture_id, WALLNUT, player_pos.x, player_pos.y,
                        0, LEVEL_WIDTH - 1, -1 * LEVEL_HEIGHT + 1, 0))
                    {
                        g_current_scene->m_wallnut_timer = WALLNUT_RECHARGE;
                        g_current_scene->m_sun -= WALLNUT_COST;
                    }
                    Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);

                }
            default:    
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
    else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
    
    if (key_state[SDL_SCANCODE_UP])  g_current_scene->get_state().player->move_up();
    else if (key_state[SDL_SCANCODE_DOWN]) g_current_scene->get_state().player->move_down();
    if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();

}

void update()
{
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        //g_effects->update(FIXED_TIMESTEP);
        
       

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);

   /* if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 
            -g_current_scene->get_state().player->get_position().y,
                                                                 0));
    }
    else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }*/
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x,
        -g_current_scene->get_state().player->get_position().y,
        0));

}

void render()
{
    if (g_current_scene->get_state().lose_game)
    {
        g_shader_program.load(V_SHADER_PATH, F_GREYSCALE_SHADER_PATH);
        g_shader_program.set_projection_matrix(g_projection_matrix);
        g_shader_program.set_view_matrix(g_view_matrix);


    }
    glUseProgram(g_shader_program.get_program_id());
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    
    g_current_scene->render(&g_shader_program);
    //g_effects->render();

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
 
    SDL_Quit();

   
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();

        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);

        render();
    }

    shutdown();
    return 0;
}