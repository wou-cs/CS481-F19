#version 150

uniform vec3 lightPosition;
uniform vec3 lightRadiance;
uniform sampler2D textureLU;		// texture "look-up table" or "sampler"

//in vec4 color;
in vec3 normal;
in vec3 position;
in vec3 viewer;
in vec2 uv;			// texture coordinates

out vec4 fragColor;

// Point light attenuation (constant, linear, quadratic) terms
vec3 pointAttenuationFactors = vec3(0.0,0.0,0.0125);
float alpha = 20.0; 	// specular shinyness coefficient

void main()
{
	vec3 lvec = lightPosition - position;
	float dist = length(lvec);
	float attenuation = dot(pointAttenuationFactors,vec3(1.0, dist, dist*dist));

	vec3 lhat = normalize(lvec);
	vec3 nhat = normalize(normal);
	vec3 vhat = normalize(viewer);
	vec3 rhat = normalize(2 * dot(nhat,lhat)*nhat - lhat);

	vec4 tex = texture(textureLU, uv);

	vec3 lightCommon = tex.xyz * (lightRadiance / attenuation);

	vec3 diffuse = lightCommon * clamp(dot(nhat,lhat),0.0,1.0);
	vec3 specular = lightCommon * clamp(pow(dot(rhat,vhat),alpha),0.0,1.0);	// Phong

	vec3 lighting =  diffuse + specular;
	fragColor = vec4(lighting.r, lighting.g, lighting.b, tex.a);
}