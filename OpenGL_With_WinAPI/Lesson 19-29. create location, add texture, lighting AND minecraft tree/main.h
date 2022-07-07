/****************
 * include libs *
 ****************/
#include <windows.h>
#include <gl/gl.h>
#include <stdbool.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "_libs/stb_image.h"
#include "./_OpenGL/camera.h"
#include "./_OpenGL/handlers.h"
#include "./_window/init.h"


#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
/*************************************
 * declaration structs and functions *
 *************************************/
typedef struct {
    float x, y, z;
} TCell;
typedef struct {
    float r, g, b;
} TColor;
typedef struct {
    float u,v;
} TUV;
typedef struct {
    float x,y,z;
    int type;
    float scale;
} TObject;
typedef struct {
    TObject *items;
    int itemsCnt;
    int type;
} TObjGroup;
typedef struct {
    TObject *obj;
    float dx,dy,dz;
    int cnt;
} TAnim;
typedef struct {
    int type;
    int x,y;
    int width,height;
} TSlot;
typedef struct {
    int time;
    int maxTime;
} TBuff;
typedef struct {
    int items[3][3];
    int item_out;
} TRecipe;


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
bool coordinates_in_map(float x, float y);
float get_map_height(float x, float y);
void create_tree(TObjGroup *obj, int type, float x, float y);
void show_tree(TObjGroup obj);
void load_texture(const char *filename, int *texture);
void calc_normals(TCell a, TCell b, TCell c, TCell* n);
void create_hill_in_map(float xc, float yc, float radius, float height);
void init_all_maps(HWND hwnd);
void window_resize(int width, int height);
void player_move();
void show_map();
void player_take(HWND hwnd);
void set_animation(TAnim *animation, TObject *object);
void move_animation(TAnim *animation);
void toggle_fullscreen(HWND hwnd);
void show_menu();
void show_client_bag(int x, int y, float scale);
void show_health(int x, int y, float scale);
void process_click_slots(int x, int y, int scale, int mx, int my, int button);
void tick_map(HWND hwnd);
int get_bag_count(int type);
void delete_bag_count(int type, int cnt);
void tick_buff_timer(TBuff *buff);
void show_active_buffs(int x, int y, int scale, TBuff buff, int tex_id);
void show_client_cell_bag(int x, int y, float scaleX, float scaleY, int type);
void show_player_hand_item();
void game_create();
void resize_craft_menu(int scale);
void show_craft_menu();
bool coordinates_in_craft_slot(TSlot slot, int x, int y);
void process_click_craft_menu(int mx, int my, int button);
void add_recipe(int items[3][3], int item_out);
void check_correct_recipe();


/********************
 * global variables *
 ********************/


/**
 * surface (main map)
 */
#define mapW 100
#define mapH 100
TCell map_vertexes[mapW][mapH];
TCell map_normals[mapW][mapH];
TColor map_colors[mapW][mapH];
TUV map_uv[mapW][mapH];
GLuint map_indexes[mapW - 1][mapH - 1][6];
#define mapIndSize (sizeof(map_indexes) / sizeof(GLuint))


/**
 * all textures except minecraft-tree
 */
GLfloat array_vertexes[] = {
    -0.5,0,0,  0.5,0,0,  0.5,0,1,  -0.5,0,1,
    0,-0.5,0,  0,0.5,0,  0,0.5,1,  0,-0.5,1
};
GLfloat array_tex_coordinates[] = {
    0,1,  1,1,  1,0,  0,0,
    0,1,  1,1,  1,0,  0,0
};
GLuint array_indexes[] = {
    0,1,2,  2,3,0,  4,5,6,  6,7,4
};
#define arrIndSize sizeof(array_indexes) / sizeof(GLuint)
TObject *array_objects = NULL;
int arrObjectsCnt = 0;


/**
 * minecraft tree
 */
GLfloat array_vertexes_cube[] = {
    0,0,0,  1,0,0,  1,1,0,  0,1,0,
    0,0,1,  1,0,1,  1,1,1,  0,1,1,

    0,0,0,  1,0,0,  1,1,0,  0,1,0,
    0,0,1,  1,0,1,  1,1,1,  0,1,1
};
GLfloat array_tex_coordinates_cube_log[] = {
    0.5,0.5,  1,0.5,  1,0,  0.5,0,
    0.5,0.5,  1,0.5,  1,0,  0.5,0,

    0,0.5,  0.5,0.5,  0,0.5,  0.5,0.5,
    0,0,  0.5,0,  0,0,  0.5,0
};
GLfloat array_tex_coordinates_cube_leaf[] = {
    0,1,  0.5,1,  0.5,0.5,  0,0.5,
    0,1,  0.5,1,  0.5,0.5,  0,0.5,
    0,0.5,  0.5,0.5,  0,0.5,  0.5,0.5,
    0,1,  0.5,1,  0,1,  0.5,1
};
GLuint array_indexes_cube[] = {
    0,1,2,  2,3,0,  4,5,6,  6,7,4,
    8,9,13,  13,12,8,  9,10,14,  14,13,9,
    10,11,15,  15,14,10,  11,8,12,  12,15,11
};
#define arrIndCubeSize sizeof(array_indexes_cube) / sizeof(GLuint)
TObjGroup *array_tree = NULL;
int array_tree_cnt = 0;


/**
 * array vertexes for imitations sun
 */
float array_vertexes_sun[] = {
    -1,-1,0,  1,-1,0,  1,1,0,  -1,1,0
};


/**
 * all descriptors of textures
 */
int tex_pole,tex_trava,tex_flower,tex_flower2,tex_grib,tex_tree,tex_tree2,tex_wood;
int tex_icon_eye, tex_icon_speed;
int tex_icon_mortar,tex_icon_potion_eye,tex_icon_potion_life,tex_icon_potion_speed;

/**
 * implementation select object's mode
 */
#define objListCnt 255
bool select_mode = false;
typedef struct {
    int plant_mas_index;
    int color_index;
} TSelectObj;
TSelectObj array_select_objects[objListCnt];
int array_select_objects_cnt = 0;


/**
 * simple animation for taking loot
 * animation global variable store state of animation
 */
TAnim animation = {0,0,0,0};


/**
 * user interface
 */
#define client_area_bag_size 16
POINT client_area_size;
float client_area_scale_factory;
TSlot client_area_bag[client_area_bag_size];
GLfloat client_area_bag_coordinates[] = {
    0,0,  1,0,  1,1,  0,1
};
int player_health = 15;
int player_health_max = 20;
float player_health_array[] = {
    0.5,0.25,  0.25,0,  0,0.25,  0.5,1,  1,0.25,  0.75,0
};
bool mouse_is_bind = true;

/**
 * add buffs for player
 */
struct {
    TBuff speed;
    TBuff night_eyes;
} player_buffs = {0,0,0,0};


/**
 * add drag-and-drop
 */
int player_item_type_hand = 0;
POINT mouse_cursor_position;


/**
 * add craft
 */
struct {
    int x,y;
    int width,height;
    TSlot items[3][3];
    TSlot item_out;
    bool is_active;
} player_craft_menu;
TRecipe *array_recipes = NULL;
int array_recipes_count = 0;


#endif // MAIN_H_INCLUDED
