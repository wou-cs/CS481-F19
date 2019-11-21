
uniform mat4 IvModelViewProjectionMatrix;
uniform mat4 IvNormalMatrix;

layout(location = POSITION) in vec3 position;
layout(location = COLOR)	in vec4 inColor;
layout(location = NORMAL)	in vec3 inNormal;
//layout(location = TEXCOORD0)in vec2 texCoord;

out vec4 color;
out vec3 normal;

void main()
{
	gl_Position = IvModelViewProjectionMatrix * vec4(position, 1.0);
	color = inColor;
	//color = vec4(inNormal,1.0);
	normal = vec3(IvNormalMatrix * vec4(inNormal,0));
}

/* We need to transform the normal vector with the Model->World matrix
   so the normal "goes with" the triangle.  The common library makes
   this available in IvNormalMatrix.  It has been calculated so that
   it works even for shearing and non-uniform scaling transforms that 
   can deform angles (see section 4.3.7)*/