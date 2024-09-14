#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

out vec4 finalColor;

uniform vec4 fogColor;
uniform float fogDensity;
uniform vec3 viewPosition;

void main() 
{
	float visibility = gl_FragCoord.w/fogDensity;
	visibility = clamp(visibility, 0.0, 1.0);
	finalColor = mix(fogColor, fragColor, visibility);
}