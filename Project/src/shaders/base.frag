#version 330 core

in vec3 vColor;
in vec2 vTexCoord;

uniform sampler2D tex; 
uniform bool useTexture;
uniform float opacity;

out vec4 FragColor;

void main()
{
    vec4 texColor = vec4(1.0);
    if (useTexture)
        texColor = texture(tex, vTexCoord);

    FragColor = texColor * vec4(vColor, opacity);
}