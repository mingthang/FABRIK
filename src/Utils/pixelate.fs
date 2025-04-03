#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTex;
uniform float vx_offset; // Split position
uniform float rt_w;  // Screen width
uniform float rt_h;  // Screen height
uniform float pixel_w; // Pixel block width
uniform float pixel_h; // Pixel block height

void main() 
{ 
    vec2 uv = TexCoords;
    vec3 tc = vec3(1.0, 0.0, 0.0); // Default red (if broken)

    if (uv.x < (vx_offset - 0.005)) 
    {
        float dx = pixel_w / rt_w;
        float dy = pixel_h / rt_h;
        vec2 coord = vec2(dx * floor(uv.x / dx),
                          dy * floor(uv.y / dy));
        tc = texture(sceneTex, coord).rgb; // Pixelate effect
    }
    else if (uv.x >= (vx_offset + 0.005)) 
    {
        tc = texture(sceneTex, uv).rgb; // Normal effect
    }

    FragColor = vec4(tc, 1.0);
}
