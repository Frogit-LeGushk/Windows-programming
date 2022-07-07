#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#define MENU_BTN_CHECK_GROUP 0
#define MENU_BTN_RADIO_GROUP 1

int add_button(char *name, int x, int y, int width, int height, float scale);
void show_menu();
int move_mouse(float x, float y);
int down_mouse();
void up_mouse();
char *get_button_name(int btn_id);
void clear_menu();
void set_param_1i(int btn_id, int param_name, int value);
int get_radio_value_by_group(int radio_group);
int get_check_value_by_group(int check_group);

#endif // MENU_H_INCLUDED
