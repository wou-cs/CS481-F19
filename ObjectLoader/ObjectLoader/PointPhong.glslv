
uniform mat4 IvModelViewProjectionMatrix;
uniform mat4 IvNormalMatrix;
uniform mat4 modelMatrix;
uniform vec3 viewPosition;

layout(location = POSITION) in vec3 inPosition;
//layout(location = COLOR)	in vec4 inColor;
layout(location = NORMAL)	in vec3 inNormal;
layout(location = TEXCOORD0)in vec2 texCoord;

//out vec4 color;
out vec3 normal;
out vec3 position;
out vec3 viewer;
out vec2 uv;

void main()
{
	gl_Position = IvModelViewProjectionMatrix * vec4(inPosition, 1.0);
    position = vec3(modelMatrix * vec4(inPosition,1.0));
	//color    = inColor;
	normal   = vec3(IvNormalMatrix * vec4(inNormal,0.0));
    viewer   = viewPosition - position;
	uv		 = texCoord;
}
