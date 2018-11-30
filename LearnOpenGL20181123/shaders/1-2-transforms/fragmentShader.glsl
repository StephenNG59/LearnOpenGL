#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord1;
in vec2 TexCoord2;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float faceVisibility;

void main()
{
	FragColor = mix(texture(texture1, TexCoord1), texture(texture2, TexCoord2), faceVisibility) * ourColor;
	// FragColor = mix(texture(texture1, TexCoord1), texture(texture2, TexCoord2), 1);	// only show the second texture
	// FragColor = mix(texture(texture1, TexCoord1), texture(texture2, vec2(TexCoord2.y, TexCoord2.x)), faceVisibility) * ourColor;	// the face will turn 90 degrees clock-wise
}