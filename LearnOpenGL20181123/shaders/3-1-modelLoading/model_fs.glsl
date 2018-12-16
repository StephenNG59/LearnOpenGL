#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float Time;
uniform sampler2D texture_diffuse1;

void main()
{    
//    FragColor = texture(texture_diffuse1, TexCoords);
	FragColor = vec4(sin(Time) * TexCoords.x, cos(Time) * TexCoords.y, 0.5f * cos(Time) * TexCoords.x + 0.5f * sin(Time) * TexCoords.y, 1.0f);
}