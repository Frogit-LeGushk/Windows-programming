#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <menu.h>
#include <stb_easy_font.h>

#define MAX_BTN_LENGTH 32
typedef struct {
    char name[MAX_BTN_LENGTH];
    float vertexs[8];
    float buffers[50 * MAX_BTN_LENGTH];
    int number_quads;
    float text_pos_x, text_pos_y, text_scale;
    char is_hover, is_down;
    int check_group, radio_group;
} TBtn;
static TBtn buttons[128];
static int buttons_size = 0;
static float mouse_x, mouse_y;

int add_button(char *name, int x, int y, int width, int height, float scale)
{
    buttons_size++;
    snprintf(buttons[buttons_size-1].name, MAX_BTN_LENGTH, "%s", name);
    float *vert = buttons[buttons_size-1].vertexs;
    vert[0] = vert[6] = x;
    vert[2] = vert[4] = x + width;
    vert[1] = vert[3] = y;
    vert[5] = vert[7] = y + height;

    TBtn *b = buttons + buttons_size - 1;
    b->number_quads = stb_easy_font_print(
                0,0,name,0,b->buffers,sizeof(b->buffers));
    b->text_pos_x = x + (width - stb_easy_font_width(name) * scale) / 2.0;
    b->text_pos_y = y + (height - stb_easy_font_height(name) * scale) / 2.0;
    b->text_pos_y += scale * 2;
    b->text_scale = scale;

    b->is_hover = 0;
    b->is_down = 0;
    b->check_group = 0;
    b->radio_group = 0;

    return buttons_size-1;
}
static void show_button(int button_id)
{
    TBtn btn = buttons[button_id];
    glVertexPointer(2, GL_FLOAT, 0, btn.vertexs);
    glEnableClientState(GL_VERTEX_ARRAY);
        if(btn.is_down) glColor3f(0.2, 1.0, 0.2);
        else if(btn.is_hover) glColor3f(0.8, 0.8, 1.0);
        else glColor3f(0.6, 0.6, 0.8);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glColor3f(1,1,1);
        glLineWidth(1);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
        glColor3f(0,0,0);
        glTranslatef(btn.text_pos_x, btn.text_pos_y, 0);
        glScalef(btn.text_scale, btn.text_scale, 1);
        glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, 16, btn.buffers);
            glDrawArrays(GL_QUADS, 0, btn.number_quads*4);
        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}
void show_menu()
{
    for(int i = 0; i < buttons_size; i++)
        show_button(i);
}
static char is_coordinates_in_btn(int btn_id, float x, float y)
{
    float *vert = buttons[btn_id].vertexs;
    return (x > vert[0]) && (y > vert[1]) && (x < vert[4]) && (y < vert[5]);
}
int move_mouse(float x, float y)
{
    mouse_x = x;
    mouse_y = y;
    int move_btn = -1;
    for(int i = 0; i < buttons_size; i++)
    {
        if(is_coordinates_in_btn(i, mouse_x, mouse_y))
        {
            buttons[i].is_hover = 1;
            move_btn = i;
        }
        else
        {
            buttons[i].is_hover = 0;
            if((buttons[i].check_group == 0) && (buttons[i].radio_group == 0))
                buttons[i].is_down = 0;
        }
    }
    return move_btn;
}
static void __button_down(int btn_id)
{
    if(buttons[btn_id].check_group)
        buttons[btn_id].is_down = !buttons[btn_id].is_down;
    else
    {
        if(buttons[btn_id].radio_group)
        {
            for(int i = 0; i < buttons_size; i++)
                if(buttons[btn_id].radio_group == buttons[i].radio_group)
                    buttons[i].is_down = 0;
        }

        buttons[btn_id].is_down = 1;
    }
}
int down_mouse()
{
    int down_btn = -1;
    for(int i = 0; i < buttons_size; i++)
    {
        if(is_coordinates_in_btn(i, mouse_x, mouse_y))
        {
            __button_down(i);
            down_btn = i;
        }
    }
    return down_btn;
}
void up_mouse()
{
    for(int i = 0; i < buttons_size; i++)
        if((buttons[i].check_group == 0) && (buttons[i].radio_group == 0))
            buttons[i].is_down = 0;
}
char *get_button_name(int btn_id)
{
    return buttons[btn_id].name;
}
void clear_menu()
{
    buttons_size = 0;
}
void set_param_1i(int btn_id, int param_name, int value)
{
    if(param_name == MENU_BTN_CHECK_GROUP)
        buttons[btn_id].check_group = value;
    else if(param_name == MENU_BTN_RADIO_GROUP)
        buttons[btn_id].radio_group = value;
}
int get_radio_value_by_group(int radio_group)
{
    int radio_value = -1;
    for(int i = 0; i < buttons_size; i++)
        if(buttons[i].radio_group == radio_group)
    {
        radio_value++;
        if(buttons[i].is_down)
            return radio_value;
    }

    return -1;
}
int get_check_value_by_group(int check_group)
{
    for(int i = 0; i < buttons_size; i++)
        if(buttons[i].check_group == check_group)
            return buttons[i].is_down;

    return -1;
}







