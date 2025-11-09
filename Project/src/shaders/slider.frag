#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D tex; 

uniform float sliderValue = 0.5;

uniform vec3 FillColor = vec3(0.0, 1.0, 0.0);
uniform vec3 BackgroundColor = vec3(1.0, 0.0, 0.0);
uniform vec3 BorderColor = vec3(0.0, 0.0, 1.0);

void main()
{
    if(vTexCoord.x < 0.05 || vTexCoord.x > 0.95 || vTexCoord.y < 0.05 || vTexCoord.y > 0.95)
        FragColor = vec4(BorderColor, 0.1);
    else if (vTexCoord.x <= sliderValue * 0.90 + 0.05)
        FragColor = vec4(FillColor, 0.5);
    else
        FragColor = vec4(BackgroundColor, 0.5);

    vec4 TexColor = texture(tex, vTexCoord);
    FragColor *= TexColor;
}