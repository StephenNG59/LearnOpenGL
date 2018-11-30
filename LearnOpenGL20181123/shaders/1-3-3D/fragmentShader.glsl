#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float faceVisibility;

void main()
{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), faceVisibility);
	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1);	// only show the second texture
	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(TexCoord.y, TexCoord.x)), faceVisibility) * ourColor;	// the face will turn 90 degrees clock-wise
}