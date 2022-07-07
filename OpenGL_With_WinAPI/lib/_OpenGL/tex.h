#ifndef TEX_H_INCLUDED
#define TEX_H_INCLUDED

typedef void (*TLoad_texture_callback) (unsigned char *data,
                                        int width, int height, int cnt);
void load_texture(const char *filename, int *texture,
                  int wrap, int filter,
                  TLoad_texture_callback callback);
void load_texture_font(const char *filename, int *font_id,
                    int filter, int check_byte);
void text_out(int font_id, char *text);

#endif // TEX_H_INCLUDED
