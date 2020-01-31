#version 450

layout(location = 0) out vec2 p_UV;

void main()
{
	p_UV.x = (gl_VertexIndex == 2) ? 2.0f : 0.0f;
	p_UV.y = (gl_VertexIndex == 1) ? 2.0f : 0.0f;

	gl_Position.x = (gl_VertexIndex == 2) ? 3.0f : -1.0f;
	gl_Position.y = (gl_VertexIndex == 1) ? -3.0f : 1.0f;
	gl_Position.zw = vec2(1.0f, 1.0f);
}