#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

char *load_shader_file(const char *filename);
GLuint load_shader(const char *filename, GLuint type);
GLuint create_shader_program(const char *vertex_file_name, const char *fragment_file_name);

#endif // SHADER_H_INCLUDED
