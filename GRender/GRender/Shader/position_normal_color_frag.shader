#version 430
//#extension GL_ARB_separate_shader_objects : enable

in vec4 intermediateColor;
in vec3 intermediateNormal;
in vec3 intermediateVertex;
in vec3 intermediateLightPosition;
in vec3 intermediateEyePosition;

//uniform vec4 G_lightColor;
//uniform vec4 G_ambientColor;
//uniform vec4 G_diffuseColor;
//uniform vec4 G_specularColor;
//uniform float G_shininess;

//uniform vec4 G_displayColor;

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
		lambert = vec4(ndot); //G_diffuseColor * ndot;
		vec3 posToEye = normalize(intermediateEyePosition);// - myvertex
		vec3 LightReflect = normalize(reflect(lightToPos, normal));
		float factor = dot(posToEye, LightReflect);
		if (factor > 0)
		{
			specucolor = vec4(pow(factor, 64));//G_specularColor  * pow(factor, G_shininess)* G_specularColor.w;
		}
	}

	vec4 col = intermediateColor;
	//col = col * G_displayColor.w + vec4(G_displayColor.xyz, 1.0) * G_displayColor.w;

	fragColor = (lambert + specucolor) * col; //(G_ambientColor + lambert + specucolor) * col * G_lightColor;
}