#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stb_image.h>
#include "tex.h"

static struct {
    int texture_id;
    float cWidthArray[256];
    int pixPerChar;
    int check_byte;
} fonts[64];
static int fonts_count = 0;

static void set_char_size(unsigned char *data, int width_height, int cnt,
                   float *cWidthArray, int check_byte)
{
    int pixPerChar = width_height / 16;
    for(int k = 0; k < 256; k++)
    {
        int x = (k % 16) * pixPerChar;
        int y = (k / 16) * pixPerChar;
        int i;
        int ind;
        unsigned char alpha;
        for(i = x + pixPerChar - 1; i > x; i--)
        {
            for(int j = y + pixPerChar - 1; j > y; j--)
            {
                alpha = data[(j * width_height + i) * cnt + check_byte];
                if(alpha > 0) break;
            }
            if(alpha > 0) break;
        }

        i += pixPerChar / 10.0;
        if(i > x + pixPerChar - 1) i = x + pixPerChar - 1;
        if(k == 32) i = x + pixPerChar / 2;

        cWidthArray[k] = (i - x) / (float)pixPerChar;
    }
}
static void callback_load_texture(unsigned char *data, int width, int height, int cnt)
{
    if(!data) return;
    fonts[fonts_count-1].pixPerChar = width / 16;
    set_char_size(data, width, cnt,
                  fonts[fonts_count-1].cWidthArray,
                  fonts[fonts_count-1].check_byte);
}
void load_texture(const char *filename, int *texture,
                  int wrap, int filter,
                  TLoad_texture_callback callback)
{
    int width,height,cnt;
    unsigned char *data = stbi_load(filename, &width, &height, &cnt, 0);

    if(callback)
        callback(data, width, height, cnt);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                                    0, cnt == 4 ? GL_RGBA : GL_RGB,
                                    GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}
void load_texture_font(const char *filename, int *font_id, int filter, int check_byte)
{
    fonts_count++;
    fonts[fonts_count-1].check_byte = check_byte;
    int k;
    load_texture(filename, &k, GL_REPEAT, filter, callback_load_texture);
    fonts[fonts_count-1].texture_id = k;
    *font_id = fonts_count-1;
}
void text_out(int font_id, char *text)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fonts[font_id].texture_id);
    glPushMatrix();
        int scale = fonts[font_id].pixPerChar;
        glScalef(scale, scale, scale);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            GLfloat array_vertexes[] = {0,1,  1,1,  1,0,  0,0};
            GLfloat array_tex_coord[] = {0,1,  1,1,  1,0,  0,0};
            glVertexPointer(2, GL_FLOAT, 0, array_vertexes);
            glTexCoordPointer(2, GL_FLOAT, 0, array_tex_coord);

            while(*text)
            {
                unsigned char c = *text;
                int x = c & 0b1111;
                int y = c >> 4;

                float char_size = 1 / 16.0;
                float char_width = fonts[font_id].cWidthArray[c];

                struct {float left,right,top,botton} rect;
                rect.left = x * char_size;
                rect.right = rect.left + char_size * char_width;
                rect.top = y * char_size;
                rect.botton = rect.top + char_size;

                array_tex_coord[0] = array_tex_coord[6] = rect.left;
                array_tex_coord[2] = array_tex_coord[4] = rect.right;
                array_tex_coord[1] = array_tex_coord[3] = rect.botton;
                array_tex_coord[5] = array_tex_coord[7] = rect.top;
                array_vertexes[2] = array_vertexes[4] = char_width;

                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glTranslatef(char_width,0,0);
                text++;
            }
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}
