#include "Scene.h"

float Scene::hash_function(float x_coord, float y_coord)
{
    float prime_x = 37;
    float prime_y = 17;
    return prime_x * x_coord + prime_y * y_coord;
}


void Scene::plant_it(GLuint texture_id, AIType ai_type, float x_coord,float y_coord)
{
    float plant_x = round(x_coord), plant_y = round(y_coord);
    float coord_hash = hash_function(plant_x, plant_y);
    //if there is already a plant here
    if (m_game_state.plant_map.count(coord_hash) == 1 
        || m_game_state.plant_map[coord_hash] == true) { return; }
    
    m_game_state.plant_map[coord_hash] = true;
    Entity* new_plant = new Entity(texture_id, 0.0f, 1.0f, 1.0f, PLANT, ai_type, IDLE);
    new_plant->set_position(glm::vec3(plant_x, plant_y, 0.0f));
    m_game_state.non_enemies.push_back(new_plant);

}
