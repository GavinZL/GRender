#version 430
//#extension GL_ARB_separate_shader_objects : enable

out vec4 intermediateColor;
out vec3 intermediateNormal;
out vec3 intermediateVertex;
out vec3 intermediateLightPosition;
out vec3 intermediateEyePosition;

uniform vec4 G_lightColor;
uniform vec4 G_ambientColor;
uniform vec4 G_diffuseColor;
uniform vec4 G_specularColor;
uniform float G_shininess;

uniform vec4 G_displayColor;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 normal = normalize(intermediateNormal);
	vec3 lightToPos = normalize(intermediateVertex - intermediateLightPosition);

	vec4 specucolor;
	vec4 lambert;
	float ndot = dot(-lightToPos, normal);
	if (ndot > 0)
	{
		lambert = G_diffuseColor * ndot;
		vec3 posToEye = normalize(intermediateEyePosition);// - myvertex
		vec3 LightReflect = normalize(reflect(lightToPos, normal));
		float factor = dot(posToEye, LightReflect);
		if (factor > 0)
		{
			specucolor = G_specularColor  * pow(factor, G_shininess)* G_specularColor.w;
		}
	}

	vec4 col = intermediateColor;
	col = col * G_displayColor.w + vec4(G_displayColor.xyz, 1.0) * G_displayColor.w;

	fragColor = (G_ambientColor + lambert + specucolor) * col * G_lightColor;
}