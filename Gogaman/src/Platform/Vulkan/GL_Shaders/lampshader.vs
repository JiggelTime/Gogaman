layout (location = 0) in vec3 aPos;

uniform mat4 M;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * M * vec4(aPos, 1.0f);
	//gl_Position = vec4(aPos, 1.0f);
}