#version 110

uniform vec2 map_size;
varying vec2 map_tex_coord;

void main()
{
    vec2 vertex = gl_Vertex.xy * map_size;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex, 0, 1);
    map_tex_coord = gl_Vertex.xy;
}
