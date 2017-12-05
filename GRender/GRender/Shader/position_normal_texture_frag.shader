#version 430
//#extension GL_ARB_separate_shader_objects : enable

in vec2 texCoord0;
in vec3 mynormal;
in vec3 myvertex;
in vec3 mylightpos;
in vec3 myeyepos;

uniform vec4 G_lightColor;
uniform vec4 G_ambientColor;
uniform vec4 G_diffuseColor;
uniform vec4 G_specularColor;
uniform float G_shininess;
uniform sampler2D G_texture0;

uniform int hasTexture;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 normal = normalize(mynormal);
	vec3 lightToPos = normalize(myvertex - mylightpos);

	vec4 specucolor = vec4(0.0,0.0,0.0,0.0);
	vec4 lambert = vec4(0.0,0.0,0.0,0.0);
	float ndot = dot(-lightToPos, normal);
	if(ndot>0)
	{
		lambert = G_diffuseColor * ndot;
		vec3 posToEye = normalize(myeyepos);// - myvertex
		vec3 LightReflect = normalize(reflect(lightToPos, normal));
		float factor = dot(posToEye,LightReflect);
		if(factor>0)
		{
			specucolor = G_specularColor  * pow(factor, G_shininess)* G_specularColor.w;
		}
	}

	vec4 textureColor = vec4(1.0);
	if (hasTexture == 1)
	{
		textureColor = texture(G_texture0, texCoord0.xy).zyxw;
	}

	fragColor = (G_ambientColor + lambert + specucolor) * textureColor * G_lightColor;
}
