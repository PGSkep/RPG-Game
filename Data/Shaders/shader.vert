#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position;
};

layout(push_constant) uniform PushConsts
{
	layout(offset = 0)vec2 position;
	layout(offset = 8)float aspect;
	layout(offset = 12)float zoom;
	layout(offset = 16)int hexPerLine;
} pushConsts;

layout(location = 0) in vec2 vertPos;
layout(location = 1) in vec3 vertCol;

layout(location = 0) out vec3 fragColor;

void main() {
	float hexHeight = 0.86603;
	float hexWidth = 0.75;

	int lineNum = (gl_InstanceIndex / pushConsts.hexPerLine);
	int columnNum = (gl_InstanceIndex % pushConsts.hexPerLine);

	int isEvenLine = ((columnNum + gl_InstanceIndex) & 1);
	int isEvenColumn = (columnNum & 1);

	float xOffset = columnNum * hexWidth;

	float yOffset = lineNum * hexHeight + isEvenColumn * hexHeight/2;

	gl_Position = vec4(
		((vertPos.x + xOffset)						+ pushConsts.position.x) * pushConsts.zoom,
		((vertPos.y + yOffset) * pushConsts.aspect	+ pushConsts.position.y) * pushConsts.zoom,
		0.0, 1.0);

	fragColor = vertCol;
}