#version 110

uniform sampler2D tex_map;
uniform sampler2D texSand;
uniform sampler2D texSpice;
uniform sampler2D texStone;
uniform sampler2D texRock;

varying vec2 map_tex_coord;

void main()
{
    vec4 sand = texture2D(texSand, map_tex_coord);
    vec4 spice = texture2D(texSpice, map_tex_coord);
    vec4 stone = texture2D(texStone, map_tex_coord);
    vec4 rock = texture2D(texRock, map_tex_coord);

    vec4 mapV = texture2D(tex_map, map_tex_coord);
    float sandV = clamp(1.0-(mapV.r + mapV.g + mapV.b),0.0,1.0);

    gl_FragColor = (sand * sandV
                    + spice * mapV.r
                    + stone * mapV.g
                    + rock * mapV.b);
}
