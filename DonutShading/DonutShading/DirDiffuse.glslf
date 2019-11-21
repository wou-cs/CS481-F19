#version 150

// If a uniform is declared here, but not used in the rest of this shader
// the shader compiler may optimize it out, which means that if you try to 
// get this uniform and write to it, it won't exist and will fail.  So only
// declare ones you'll actually use, comment out the rest
//uniform vec4 constColor;

uniform vec3 lightDir;
uniform vec3 lightRadiance;

in vec4 color;
in vec3 normal;

out vec4 fragColor;

void main()
{
	// Compute simple light equation for directional light
	vec3 lhat = normalize(-lightDir);
	vec3 nhat = normalize(normal);

	vec3 diffuse = color.xyz * lightRadiance * clamp(dot(nhat,lhat),0.0,1.0);
	fragColor = vec4(diffuse.r, diffuse.g, diffuse.b, 1.0);

	//fragColor = vec4(1.0,0.0,0.0,1.0);
	//fragColor = constColor + color;
	//fragColor = color;
}