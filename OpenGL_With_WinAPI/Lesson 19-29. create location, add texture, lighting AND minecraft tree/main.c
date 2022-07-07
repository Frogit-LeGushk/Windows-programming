#include "main.h"

bool coordinates_in_map(float x, float y)
{
    return (x > 0) && (x < mapW) && (y > 0) && (y < mapH);
}
float get_map_height(float x, float y)
{
    if(!coordinates_in_map(x, y)) return 0;
    int xc = x;
    int yc = y;
    float h1 =  (1 - (x - xc))  * map_vertexes[xc][yc].z +
                (x - xc) * map_vertexes[xc + 1][yc].z;
    float h2 =  (1 - (x - xc))  * map_vertexes[xc][yc + 1].z +
                (x - xc) * map_vertexes[xc + 1][yc + 1].z;
    return (1 - (y - yc)) * h1 + (y - yc) * h2;
}
void create_tree(TObjGroup *obj, int type, float x, float y)
{
    // put texture id from OpenGL
    obj->type = type;
    float z = get_map_height(x + 0.5, y + 0.5) - 0.5;
    // count logs of tree
    int logs = 6;
    // count leafs of tree
    int leafs = 5*5*2-2+3*3*2;
    obj->itemsCnt = logs + leafs;
    obj->items = malloc(obj->itemsCnt * sizeof(TObject));

    // create LOGS -> ствол дерева
    for(int i = 0; i < logs; i++)
    {
        // type == 1 == LOGS
        obj->items[i].type = 1;
        obj->items[i].x = x;
        obj->items[i].y = y;
        obj->items[i].z = z + i;
    }

    // create LEAFS -> крона деревьев (first 2 step)
    int pos = logs;
    for(int k = 0; k < 2; k++) // height
        for(int i = x - 2; i <= x + 2; i++) // width
            for(int j = y - 2; j <= y + 2; j++) // depth
                if((i != x) || (j != y)) // огибаем ствол дерева
                {
                    // type == 1 == LEAFS
                    obj->items[pos].type = 2;
                    obj->items[pos].x = i;
                    obj->items[pos].y = j;
                    obj->items[pos].z = z + logs - 2 + k;
                    pos++;
                }
    // create LEAFS -> крона деревьев (second 2 step)
    for(int k = 0; k < 2; k++)
        for(int i = x - 1; i <= x + 1; i++)
            for(int j = y - 1; j <= y + 1; j++)
                if((i != x) || (j != y))
                {
                    obj->items[pos].type = 2;
                    obj->items[pos].x = i;
                    obj->items[pos].y = j;
                    obj->items[pos].z = z + logs + k;
                    pos++;
                }
}
void show_tree(TObjGroup obj)
{
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, array_vertexes_cube);
        glColor3f(0.7, 0.7, 0.7);
        glNormal3f(0,0,1);
        // choose texture of minecraft-tree
        glBindTexture(GL_TEXTURE_2D, obj.type);
        for(int i = 0; i < obj.itemsCnt; i++)
        {
            // type == 1 == ствол
            // type == 1 == листва
            if(obj.items[i].type == 1)
                glTexCoordPointer(2, GL_FLOAT, 0, array_tex_coordinates_cube_log);
            else
                glTexCoordPointer(2, GL_FLOAT, 0, array_tex_coordinates_cube_leaf);

            glPushMatrix();
                glTranslatef(obj.items[i].x, obj.items[i].y, obj.items[i].z);
                glDrawElements(GL_TRIANGLES, arrIndCubeSize, GL_UNSIGNED_INT, array_indexes_cube);
            glPopMatrix();
        }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
void load_texture(const char *filename, int *texture)
{
    int width,height,cnt;
    unsigned char *data = stbi_load(filename, &width, &height, &cnt, 0);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                                    0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}
void calc_normals(TCell a, TCell b, TCell c, TCell* n)
{
    double wrki;
    TCell v1, v2;

    v1.x = a.x - b.x;
    v1.y = a.y - b.y;
    v1.z = a.z - b.z;

    v2.x = b.x - c.x;
    v2.y = b.y - c.y;
    v2.z = b.z - c.z;

    #define sqr(a) (a)*(a)
    wrki = sqrt(sqr(v1.y*v2.z - v1.z * v2.y) + sqr(v1.z * v2.x - v1.x * v2.z) + sqr(v1.x * v2.y - v1.y * v2.x));
    n->x = (v1.y * v2.z - v1.z * v2.y) / wrki;
    n->y = (v1.z * v2.x - v1.x * v2.z) / wrki;
    n->z = (v1.x * v2.y - v1.y * v2.x) / wrki;
}
void create_hill_in_map(float xc, float yc, float radius, float height)
{
    for(int i = xc - radius; i <= xc + radius; i++)
        for(int j = yc - radius; j <= yc + radius; j++)
            if(coordinates_in_map(i, j))
            {
                float length = sqrt(pow(xc - i, 2) + pow(yc - j, 2));
                float alpha = length / radius * M_PI_2;
                if(length < radius)
                    map_vertexes[i][j].z += cos(alpha) * height;
            }
}
void init_all_maps(HWND hwnd)
{
    /**
     * filling client bag by void
     */
    for(int i = 0; i < client_area_bag_size; i++)
        client_area_bag[i].type = 0;
    /**
     * Set start property of displaying
     */
    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    window_resize(client_rect.right, client_rect.bottom);
    /**
     * load all needed textures
     */
    load_texture("textures/pole.png", &tex_pole);
    load_texture("textures/trava.png", &tex_trava);
    load_texture("textures/flower.png", &tex_flower);
    load_texture("textures/flower2.png", &tex_flower2);
    load_texture("textures/grib.png", &tex_grib);
    load_texture("textures/tree.png", &tex_tree);
    load_texture("textures/tree2.png", &tex_tree2);
    load_texture("textures/wood.png", &tex_wood);
    load_texture("textures/icon_eye.png", &tex_icon_eye);
    load_texture("textures/icon_speed.png", &tex_icon_speed);
    load_texture("textures/mortar.png", &tex_icon_mortar);
    load_texture("textures/potion_eye.png", &tex_icon_potion_eye);
    load_texture("textures/potion_life.png", &tex_icon_potion_life);
    load_texture("textures/potion_speed.png", &tex_icon_potion_speed);

    client_area_bag[0].type = tex_icon_mortar;
    add_recipe((int[3][3]) {
        tex_flower2,        0,          tex_flower2,
        0,              tex_grib,       0,
        tex_flower2,        0,          tex_flower2
    }, tex_icon_potion_eye);
    add_recipe((int[3][3]) {
        tex_flower,     tex_flower,     tex_flower,
        0,                  0,          0,
        tex_flower,     tex_flower,     tex_flower
    }, tex_icon_potion_speed);
    add_recipe((int[3][3]) {
        0,              tex_grib,       0,
        tex_grib,       tex_grib,       tex_grib,
        0,              tex_grib,       0
    }, tex_icon_potion_life);

    /**
     * Enable needed capabilities OpegGL
     */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // enable colors (а не только интенсивность цвета)
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    // for png textures
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.99);
    glEnable(GL_DEPTH_TEST);
    // for display how much time be work buffs
    glEnable(GL_BLEND);
    glAlphaFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /**
     * generate vertexes of surface and texture binding to triangles
     */
    int i, j, pos;
    for(i = 0; i < mapW; i++)
    {
        for(j = 0; j < mapH; j++)
        {
            map_vertexes[i][j].x = i;
            map_vertexes[i][j].y = j;
            map_vertexes[i][j].z = (rand() % 10) * 0.02;
            map_uv[i][j].u = i;
            map_uv[i][j].v = j;
        }
    }
    /**
     * calculate sequence of vertexes for GL_TRIANGLES mode
     */
    for(i = 0; i < mapW - 1; i++)
    {
        pos = i * mapH;
        for(j = 0; j < mapH - 1; j++)
        {
            map_indexes[i][j][0] = pos;
            map_indexes[i][j][1] = pos + 1;
            map_indexes[i][j][2] = pos + 1 + mapH;
            map_indexes[i][j][3] = pos + 1 + mapH;
            map_indexes[i][j][4] = pos + mapH;
            map_indexes[i][j][5] = pos;
            pos++;
        }
    }
    /**
     * create 10 hill on surface (change z coordinate in some area)
     */
    for(i = 0; i < 10; i++)
        create_hill_in_map(rand() % mapW, rand() % mapH, rand() % 50, rand() % 10);
    /**
     * calculate normals for each triangle of surface
     */
    for(i = 0; i < mapW - 1; i++)
    {
        for(j = 0; j < mapH - 1; j++)
        {
            calc_normals(
                map_vertexes[i][j],
                map_vertexes[i + 1][j],
                map_vertexes[i][j + 1],
                &map_normals[i][j]);
        }
    }
    /**
     * generate texture-objects on surface
     */
    int cntTrava = 2000;
    int cntGrib = 60;
    int cntTree = 40;
    arrObjectsCnt = cntTrava + cntGrib + cntTree;
    array_objects = malloc(arrObjectsCnt * sizeof(TObject));
    for(i = 0; i < arrObjectsCnt; i++)
    {
        if(cntTrava > 0)
        {
            array_objects[i].scale = 0.7 + (rand() % 5) * 0.1;
            array_objects[i].type = rand() % 10 != 0 ? tex_trava : (rand() % 2 == 0 ? tex_flower : tex_flower2);
            cntTrava--;
        }
        else if(cntGrib > 0)
        {
            array_objects[i].scale = 0.2 + (rand() % 10) * 0.01;
            array_objects[i].type = tex_grib;
            cntGrib--;
        }
        else
        {
            array_objects[i].scale = 4 + (rand() % 14);
            array_objects[i].type = rand() % 2 == 0 ? tex_tree : tex_tree2;
            cntTree--;
        }

        array_objects[i].x = rand() % mapW;
        array_objects[i].y = rand() % mapH;
        array_objects[i].z = get_map_height(array_objects[i].x, array_objects[i].y);
    }
    /**
     * generate minecraft-tree
     */
    array_tree_cnt = 50;
    array_tree = malloc(array_tree_cnt * sizeof(TObjGroup));
    for(int i = 0; i < array_tree_cnt; i++)
        create_tree(array_tree + i, tex_wood, rand() % mapW, rand() % mapH);
}
void window_resize(int width, int height)
{
    float size = 0.1;
    float xScale = (float)width / height;
    glViewport(0, 0, width, height);

    client_area_size.x = width;
    client_area_size.y = height;
    client_area_scale_factory = xScale;

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-xScale*size,xScale*size,  -size,size,  2*size, 1000);
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    resize_craft_menu(50);
}
void player_move()
{
    if(mouse_is_bind)
        rotation_camera_mouse(400, 400, 0.2);

    move_camera(
        GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1 : 0),
        GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1 : 0),
        0.1 + ((player_buffs.speed.time > 0) ? 0.2 : 0)
    );
    camera.z = get_map_height(camera.x, camera.y) + 1.7;
}
void show_map()
{
    /**
     * Calculate corners for correct lightning over time
     */
    #define abs_float(a) ((a) >= 0 ? (a) : (-a))
    #define sqr(a) (a)*(a)
    #define zakat 40.0
    static float alpha = -80;
    alpha += 0.05;
    if(alpha > 180) alpha -= 360;
    float kcc = 1 - abs_float(alpha) / 180.0;
    float k = 90 - abs_float(alpha);
    k = zakat - abs_float(k);
    k = k < 0 ? 0 : k / zakat;
    /**
     * Clear background and buffer of depth-test
     */

    if(select_mode)
        glClearColor(0, 0, 0, 0);
    else
        glClearColor(0.6f * kcc, 0.8f * kcc, 1.0f * kcc, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(select_mode)
    {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }

    move_animation(&animation);
    glPushMatrix();
        if(!select_mode)
        {
            /**
             * show displaying for sun in shape of square
             */
            glPushMatrix();
                // draw sun in player-coordinates without "glTranslatef"
                // glTranslatef set custom = 20
                glRotatef(-camera.xAlpha, 1, 0, 0);
                glRotatef(-camera.zAlpha, 0, 0, 1);
                glRotatef(alpha, 0,1,0);
                glTranslatef(0,0,20);

                // turn off for correct displaying
                glDisable(GL_TEXTURE_2D);
                glDisable(GL_DEPTH_TEST);
                    // change color of square
                    // more red, when sun at sunset (закате)
                    glColor3f(1,1-k*0.8,1-k);
                    glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3, GL_FLOAT, 0, array_vertexes_sun);
                        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    glDisableClientState(GL_VERTEX_ARRAY);
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
        }
        /**
         * Rotate world in start of point projection
         */
        apply_camera();
        /**
         * setting source of lightning
         */
        glPushMatrix();
            // setting (position) for lightning
            glRotatef(alpha, 0,1,0);
            GLfloat light_position[] = {0,0,1,0};
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            // setting (diffuse) for lightning
            // *set more red spectrum for lightning*
            float mas_diffuse[] = {1 + k*2, 1,1,0};
            glLightfv(GL_LIGHT0, GL_DIFFUSE, mas_diffuse);

            // change global lightning
            float clr = kcc * 0.15 + 0.05;
            float mass_background_lightning[] = {clr,clr,clr,0};
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, mass_background_lightning);
        glPopMatrix();

        if(!select_mode)
        {
            /**
             * draw terrain with texture, set normals
             */
            glBindTexture(GL_TEXTURE_2D, tex_pole);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, map_vertexes);
                glTexCoordPointer(2, GL_FLOAT, 0, map_uv);
                glColor3f(0.7, 0.7, 0.7);
                glNormalPointer(GL_FLOAT, 0, map_normals);
                glDrawElements(GL_TRIANGLES, mapIndSize, GL_UNSIGNED_INT, map_indexes);
            glDisableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
        }
        /**
         * Objects on terrain, except minecraft-tree
         */
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, array_vertexes);
            glTexCoordPointer(2, GL_FLOAT, 0, array_tex_coordinates);
            glColor3f(0.7, 0.7, 0.7);
            glNormal3f(0,0,1);
            // variables for select mode
            array_select_objects_cnt = 0;
            int color_red_id = 1;
            // arrObjectsCnt - all created objects (except minecraft-tree)
            for(int i = 0; i < arrObjectsCnt; i++)
            {
                if(select_mode)
                {
                    // texture-tree don't choose
                    if(array_objects[i].type == tex_tree)
                        continue;
                    if(array_objects[i].type == tex_tree2)
                        continue;
                    // avoid very far objects
                    static float choose_radius = 3;
                    float dx = array_objects[i].x - camera.x;
                    float dy = array_objects[i].y - camera.y;
                    if(sqr(dx) + sqr(dy) > sqr(choose_radius))
                        continue;
                    // draw elements shade of red
                    glColor3ub(color_red_id, 0, 0);
                    array_select_objects[array_select_objects_cnt].color_index = color_red_id;
                    array_select_objects[array_select_objects_cnt].plant_mas_index = i;
                    array_select_objects_cnt++;
                    // overflow
                    if(array_select_objects_cnt > objListCnt)
                        break;
                    color_red_id++;
                }
                else
                {
                    if((player_buffs.night_eyes.time > 0)
                       && (array_objects[i].type == tex_grib))
                    {
                        glDisable(GL_LIGHTING);
                    }
                    else
                        glEnable(GL_LIGHTING);
                }

                // choose current texture
                glBindTexture(GL_TEXTURE_2D, array_objects[i].type);
                glPushMatrix();
                    // set position
                    glTranslatef(
                        array_objects[i].x,
                        array_objects[i].y,
                        array_objects[i].z
                    );
                    glScalef(
                        array_objects[i].scale,
                        array_objects[i].scale,
                        array_objects[i].scale
                    );
                    // drawing
                    glDrawElements(GL_TRIANGLES, arrIndSize, GL_UNSIGNED_INT, array_indexes);
                glPopMatrix();
            }
        glDisableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        if(!select_mode)
        {
            /**
             * draw minecraft-tree
             */
            // array_tree_cnt - all created tree
            for(int i = 0; i < array_tree_cnt; i++)
                show_tree(array_tree[i]);
        }
    glPopMatrix();
}
void player_take(HWND hwnd)
{
    // draw frame in background (don't use swap buffers)
    select_mode = true;
    show_map();
    select_mode = false;
    // read pixel in center client rectangle
    RECT rect;
    GetClientRect(hwnd, &rect);
    GLubyte pixel[3];
    glReadPixels(rect.right / 2.0, rect.bottom / 2.0,
                 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    // processing...
    if(pixel[0] > 0)
        for(int i = 0; i < array_select_objects_cnt; i++)
            if(array_select_objects[i].color_index == pixel[0])
                set_animation(
                    &animation,
                    &array_objects[array_select_objects[i].plant_mas_index]
                );
}
void set_animation(TAnim *animation, TObject *object)
{
    // play only one animation by time
    if(animation->obj != NULL) return;
    // set props for animation
    animation->obj = object;
    animation->cnt = 10;
    animation->dx = (camera.x - object->x) / animation->cnt;
    animation->dy = (camera.y - object->y) / animation->cnt;
    animation->dz = ((camera.z - object->scale - 0.2) - object->z) / animation->cnt;
}
void move_animation(TAnim *animation)
{
    if(animation->obj)
    {
        // change position
        animation->obj->x += animation->dx;
        animation->obj->y += animation->dy;
        animation->obj->z += animation->dz;
        animation->cnt--;
        // if animation finished => set object in new place of map
        if(animation->cnt == 0)
        {
            int i;
            for(i = 0; i < client_area_bag_size; i++)
            {
                // free slot
                if(client_area_bag[i].type <= 0)
                {
                    // save texture id in slot
                    client_area_bag[i].type = animation->obj->type;
                    break;
                }
            }
            // change coordinates, if we have free slot in bag
            if(i < client_area_bag_size)
            {
                animation->obj->x = rand() % mapW;
                animation->obj->y = rand() % mapH;
            }
            animation->obj->z = get_map_height(animation->obj->x, animation->obj->y);
            animation->obj = NULL;
        }
    }
}
void toggle_fullscreen(HWND hwnd)
{
    static bool is_full = false;
    static RECT screen;
    if(screen.right == 0 && screen.bottom == 0)
        GetWindowRect(GetDesktopWindow(), &screen);

    if(GetKeyState('F') < 0 && GetKeyState(VK_LCONTROL))
    {
        if(!is_full)
        {
            // set new styles of window
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            // SWP_SHOWWINDOW == set new attribute and show
            SetWindowPos(hwnd, HWND_TOP, 0, 0, screen.right, screen.bottom, SWP_SHOWWINDOW);
            is_full = true;
        }
        else
        {
            // set default styles
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            SetWindowPos(hwnd, HWND_TOP, 200, 50, 1080, 720, SWP_SHOWWINDOW);
            is_full = false;
        }
    }
}
void show_menu()
{
    glPushMatrix();
        /**
         * change projection on normal for display menu
         */
        glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0,client_area_size.x, client_area_size.y,0, -1,1);
        glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

        /**
         * turn off GL_DEPTH_TEST and GL_LIGHTING for correct displaying
         */
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        /**
         * draw cursor
         */
        glColor3f(1,1,1);
        glLineWidth(3);
        glBegin(GL_LINES);
            glVertex2f(client_area_size.x / 2 - 15,client_area_size.y / 2);
            glVertex2f(client_area_size.x / 2 + 15,client_area_size.y / 2);
            glVertex2f(client_area_size.x / 2 ,client_area_size.y / 2 - 15);
            glVertex2f(client_area_size.x / 2,client_area_size.y / 2 + 15);
        glEnd();

        /**
         * draw slots belong player (P.S. coordinates == pixels)
         */
        show_client_bag(10, 10, 50);

        /**
         * draw state of health
         */
        show_health(10,70,30);

        /**
         * show craft menu
         */
        show_craft_menu();

        /**
         * draw active buffs
         */
        show_active_buffs(10, 100, 50, player_buffs.speed, tex_icon_speed);
        show_active_buffs(60, 100, 50, player_buffs.night_eyes, tex_icon_eye);

        /**
         * draw item in hand (if his exist)
         */
        show_player_hand_item();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    /**
     * set old value of projection
     */
    window_resize(client_area_size.x, client_area_size.y);
}
void show_client_bag(int x, int y, float scale)
{
    for(int i = 0; i < client_area_bag_size; i++)
        show_client_cell_bag(x + i * scale, y, scale, scale, client_area_bag[i].type);
}
void show_client_cell_bag(int x, int y, float scaleX, float scaleY, int type)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, client_area_bag_coordinates);
    glTexCoordPointer(2, GL_FLOAT, 0, client_area_bag_coordinates);
    glPushMatrix();
        // set position of slot
        glTranslatef(x, y, 0);
        glScalef(scaleX, scaleY, 0);
        // draw slot
        glColor3ub(110,95,73);
        glDisable(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // if slot contain thing, then show him
        if(type > 0)
        {
            glColor3f(1,1,1);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, type);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        // draw border around slot
        glColor3ub(160,146,116);
        glLineWidth(2);
        glDisable(GL_TEXTURE_2D);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
void show_health(int x, int y, float scale)
{
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT,0, player_health_array);
    for(int i = 0; i < player_health_max; i++)
    {
        glPushMatrix();
            // move indicator in client area
            glTranslated(x + i * scale, y, 0);
            glScalef(scale, scale, 1);
            // color hearth
            if(i < player_health)
                glColor3f(1,0,0);
            else
                glColor3f(0,0,0);
            // draw state of health
            glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
        glPopMatrix();
    }

    glEnable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
}
void process_click_slots(int x, int y, int scale, int mx, int my, int button)
{
    if((y > my) || (y < my - scale)) return;
    for(int i = 0; i < client_area_bag_size; i++)
    {
        if((mx > x + i * scale) && (mx < x + (i + 1) * scale))
        {
            // swap item from bag in hand
            if(button == WM_LBUTTONDOWN)
            {
                int type = player_item_type_hand;
                player_item_type_hand = client_area_bag[i].type;
                client_area_bag[i].type = type;
            }
            // toggle craft menu
            else if(client_area_bag[i].type == tex_icon_mortar)
            {
                player_craft_menu.is_active = !player_craft_menu.is_active;
            }
            else if(client_area_bag[i].type == tex_grib)
            {
                player_health += 2;
                if(player_health > player_health_max)
                    player_health = player_health_max;
                client_area_bag[i].type = -1;
            }
            // process click on grib
            else if(client_area_bag[i].type == tex_icon_potion_life)
            {
                player_health += 15;
                if(player_health > player_health_max)
                    player_health = player_health_max;
                client_area_bag[i].type = -1;
            }
            // process eat 10 red flowers
            else if(client_area_bag[i].type == tex_icon_potion_speed)
            {
                player_buffs.speed.time = 3600;
                player_buffs.speed.maxTime = 3600;
                delete_bag_count(tex_icon_potion_speed, 1);
            }
            // process eat 10 yellow flowers
            else if(client_area_bag[i].type == tex_icon_potion_eye)
            {
                player_buffs.night_eyes.time = 3600;
                player_buffs.night_eyes.maxTime = 3600;
                delete_bag_count(tex_icon_potion_eye, 1);
            }
        }
    }
}
int get_bag_count(int type)
{
    int counter = 0;
    for(int i = 0; i < client_area_bag_size; i++)
        if(client_area_bag[i].type == type)
            counter++;
    return counter;
}
void delete_bag_count(int type, int cnt)
{
    for(int i = 0; i < client_area_bag_size; i++)
        if(client_area_bag[i].type == type && cnt > 0)
        {
            client_area_bag[i].type = -1;
            cnt--;
            if(cnt <= 0) return;
        }
}
void tick_map(HWND hwnd)
{
    /**
     * tick for increase health
     */
    static int tick_hunger_current = 0;
    static int tick_hunger_max = 6000;

    tick_hunger_current++;
    if(tick_hunger_current >= tick_hunger_max)
    {
        player_health--;
        tick_hunger_current = 0;

        if(player_health < 1)
            PostQuitMessage(0);
    }

    /**
     * tick for player buffs
     */
    tick_buff_timer(&player_buffs.speed);
    tick_buff_timer(&player_buffs.night_eyes);

    /**
     * change mouse position
     */
    GetCursorPos(&mouse_cursor_position);
    ScreenToClient(hwnd, &mouse_cursor_position);
}
void tick_buff_timer(TBuff *buff)
{
    if(buff->time > 0)
    {
        buff->time--;
        if(buff->time <= 0)
            buff->maxTime = 0;
    }
}
void show_active_buffs(int x, int y, int scale, TBuff buff, int tex_id)
{
    if(buff.time > 0)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, client_area_bag_coordinates);
        glTexCoordPointer(2, GL_FLOAT, 0, client_area_bag_coordinates);
        glPushMatrix();
            // draw icon
            glTranslatef(x,y,0);
            glScalef(scale, scale, 1);
            glColor3f(1,1,1);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            // draw indicator of work time
            glScalef(1,1 - (buff.time / (float)buff.maxTime), 1);
            glColor4f(1,1,1,0.5);
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_TEXTURE_2D);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_TEXTURE_2D);
        glPopMatrix();
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}
void show_player_hand_item()
{
    // check existence of item in hand
    if(player_item_type_hand > 0 && !mouse_is_bind)
    {
        show_client_cell_bag(
            mouse_cursor_position.x, mouse_cursor_position.y,
            50,50,player_item_type_hand
        );
    }
}
void game_create()
{
    memset(&player_craft_menu, 0, sizeof(player_craft_menu));
    player_craft_menu.is_active = false;
}
void resize_craft_menu(int scale)
{
    player_craft_menu.width = scale * 6;
    player_craft_menu.height = scale * 4;
    player_craft_menu.x = 0.5*(client_area_size.x - player_craft_menu.width);
    player_craft_menu.y = 0.5*(client_area_size.y - player_craft_menu.height);
    int scale05 = 0.5 * scale;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
    {
        player_craft_menu.items[i][j].x = player_craft_menu.x + scale05 + i*scale;
        player_craft_menu.items[i][j].y = player_craft_menu.y + scale05 + j*scale;
        player_craft_menu.items[i][j].width = scale;
        player_craft_menu.items[i][j].height = scale;
    }

    player_craft_menu.item_out.x = player_craft_menu.x + scale05 + 4*scale;
    player_craft_menu.item_out.y = player_craft_menu.y + scale05 + 1*scale;
    player_craft_menu.item_out.width = scale;
    player_craft_menu.item_out.height = scale;
}
void show_craft_menu()
{
    if(!player_craft_menu.is_active || mouse_is_bind) return;

    show_client_cell_bag(
        player_craft_menu.x,
        player_craft_menu.y,
        player_craft_menu.width,
        player_craft_menu.height,
        0
    );

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
    {
        show_client_cell_bag(
            player_craft_menu.items[i][j].x,
            player_craft_menu.items[i][j].y,
            player_craft_menu.items[i][j].width,
            player_craft_menu.items[i][j].height,
            player_craft_menu.items[i][j].type
        );
    }

    show_client_cell_bag(
        player_craft_menu.item_out.x,
        player_craft_menu.item_out.y,
        player_craft_menu.item_out.width,
        player_craft_menu.item_out.height,
        player_craft_menu.item_out.type
    );
}
bool coordinates_in_craft_slot(TSlot slot, int x, int y)
{
    return  (slot.x < x) && (x < slot.x + slot.width) &&
            (slot.y < y) && (y < slot.y + slot.height);
}
void process_click_craft_menu(int mx, int my, int button)
{
    if(!player_craft_menu.is_active || button != WM_LBUTTONDOWN) return;

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
    {
        if(coordinates_in_craft_slot(player_craft_menu.items[i][j], mx, my))
        {
            int type = player_craft_menu.items[i][j].type;
            player_craft_menu.items[i][j].type = player_item_type_hand;
            player_item_type_hand = type;

            if(player_craft_menu.items[i][j].type <= 0)
            {
                player_craft_menu.items[i][j].type = 0;
            }
        }
    }
    if(
        coordinates_in_craft_slot(player_craft_menu.item_out, mx, my) &&
        player_item_type_hand <= 0 &&
        player_craft_menu.item_out.type > 0
    )
    {
        player_item_type_hand = player_craft_menu.item_out.type;
        player_craft_menu.item_out.type = 0;

        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                player_craft_menu.items[i][j].type = 0;
    }

    check_correct_recipe();
}
void add_recipe(int items[3][3], int item_out)
{
    array_recipes_count++;
    array_recipes = realloc(array_recipes, array_recipes_count * sizeof(TRecipe));
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            array_recipes[array_recipes_count - 1].items[i][j] = items[i][j];
    array_recipes[array_recipes_count - 1].item_out = item_out;
}
void check_correct_recipe()
{
    for(int k = 0; k < array_recipes_count; k++)
    {
        bool is_correct = true;
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                if(array_recipes[k].items[i][j] != player_craft_menu.items[i][j].type)
                    is_correct = false;
        if(is_correct)
        {
            player_craft_menu.item_out.type = array_recipes[k].item_out;
            break;
        }
        else
            player_craft_menu.item_out.type = 0;
    }
}



int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    init_and_show_window(hInstance, hPrevInstance, lpCmdLine, nCmdShow, &hwnd);
    EnableOpenGL(hwnd, &hDC, &hRC);
    init_all_maps(hwnd);

    while (!bQuit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if(GetForegroundWindow() == hwnd)
                player_move();
            show_map();
            show_menu();
            tick_map(hwnd);

            SwapBuffers(hDC);
            Sleep (1);
        }
    }

    DisableOpenGL(hwnd, hDC, hRC);
    DestroyWindow(hwnd);
    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_CREATE:
            game_create();
        break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if(mouse_is_bind)
                player_take(hwnd);
            else
            {
                process_click_slots(10,10,50,LOWORD(lParam), HIWORD(lParam), uMsg);
                process_click_craft_menu(LOWORD(lParam), HIWORD(lParam), uMsg);
            }
        break;

        case WM_DESTROY:
            return 0;

        case WM_SIZE:
            window_resize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_SETCURSOR:
            ShowCursor(!mouse_is_bind);
        break;

        case WM_KEYDOWN:
        {
            toggle_fullscreen(hwnd);
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
                case 'E':
                    SetCursorPos(400, 400);
                    mouse_is_bind = !mouse_is_bind;
                    if(mouse_is_bind)
                        while(ShowCursor(false) >= 0);
                    else
                        while(ShowCursor(true) <= 0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

