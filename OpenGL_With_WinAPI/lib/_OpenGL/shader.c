#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>
#include <errno.h>
#include "shader.h"

char *load_shader_file(const char *filename)
{
    static char program_text[(1 << 13)];
    FILE *fp = fopen(filename, "rb");
    if(!fp)
    {
        printf("Error load shader file");
    }
    fread(program_text, sizeof(char), (1 << 12) - 2, fp);
    return program_text;
}
static void check_compile_status(GLuint shader)
{
    GLuint status_ok;
    static char status_log[(1 << 13)] = "";
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status_ok);
    if(!status_ok)
    {
        glGetShaderInfoLog(shader, (1 << 10) - 2, NULL, status_log);
        printf("COMPILE STATUS LOG: %s\n", status_log);
    }
}
static void check_link_status(GLuint shader)
{
    GLuint status_ok;
    static char status_log[(1 << 13)];
    glGetProgramiv(shader, GL_LINK_STATUS, &status_ok);
    if(!status_ok)
    {
        glGetProgramInfoLog(shader, (1 << 13) - 2, NULL, status_log);
        printf("LINK STATUS LOG: %s\n", status_log);
    }
}
GLuint load_shader(const char *filename, GLuint type)
{
    char *text = load_shader_file(filename);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, NULL);

    glCompileShader(shader);
    check_compile_status(shader);
    return shader;
}
GLuint create_shader_program(const char *vertex_file_name, const char *fragment_file_name)
{
    GLuint f_shader = load_shader(fragment_file_name, GL_FRAGMENT_SHADER);
    GLuint v_shader = load_shader(vertex_file_name, GL_VERTEX_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, f_shader);
    glAttachShader(program, v_shader);

    glLinkProgram(program);

    check_link_status(f_shader);
    check_link_status(v_shader);

    return program;
}



