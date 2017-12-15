#version 430

// 顶点到点光源方向
in vec3 inter_vertexToPointLightDirection[8];

// 顶点到聚光源方向
in vec3 inter_vertexToSpotLightDirection[8];

// out vertex
in vec3 inter_vertex;

// out normal
in vec3 inter_Normal;

// out color
in vec4 inter_Color;

// uniforms

//direction ligth
uniform vec3 G_directoinLightSourceColor[8];
uniform vec3 G_directionLightSourceDirection[8];

//point light
uniform vec3 G_pointLightSourceColor[8];
uniform float G_pointLightSourceRangeInverse[8];

//spot light
uniform vec3 G_spotLightSourceColor[8];
uniform vec3 G_spotLightSourceDirection[8];
uniform float G_spotLightSourceInnerAngleCos[8];
uniform float G_spotLightSourceOuterAngleCos[8];
uniform float G_spotLightSourceRangeInverse[8];

//ambient
uniform vec3 G_ambientLightSourceColor;

// is light
uniform int G_hasLights;

out vec4 fragColor;

// calc color
vec3 computeLighting(vec3 normalVector,vec3 vertexPos, vec3 lightDirection, vec3 lightColor, float attenuation)
{
	// diffuse
	vec4 diffuseColor;
	vec4 specucolor;

	float diffuse = dot(normalVector, lightDirection);
	
	vec3 color = lightColor * max(diffuse, 0.0) * attenuation;
	
	/*
	if (diffuse > 0)
	{
		diffuseColor = vec4(diffuse);

		// specular
		vec3 eyePos = normalize(-vertexPos);

		vec3 lightReflect = normalize(reflect(-lightDirection, normalVector));
		float factor = dot(eyePos, lightReflect);
		if (factor > 0)
		{
			specucolor = vec4(pow(factor, 16));
		}
	}

	vec3 color = (diffuseColor + specucolor).xyz * lightColor * attenuation;
	
	*/
	return color;
}

void main(void)
{
	vec3 normal = normalize(inter_Normal);

	vec4 combinedColor = vec4(G_ambientLightSourceColor, 1.0);

	if (G_hasLights > 0)
	{
		// direction light
		
		for (int i = 0; i < 8; ++i)
		{
			if((G_directionLightSourceDirection[i] != vec3(0.0,0.0,0.0)))
			{
				vec3 lightDirection = normalize(G_directionLightSourceDirection[i] * 2.0);
				combinedColor.xyz += computeLighting(normal, inter_vertex ,lightDirection, G_directoinLightSourceColor[i], 1.0);
			}
		}
		
		
		// point light
		for (int i = 0; i < 1; ++i)
		{
			if((G_pointLightSourceRangeInverse[i] != 0.0)){
				vec3 ldir = inter_vertexToPointLightDirection[i] * G_pointLightSourceRangeInverse[i];
				float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
				combinedColor.xyz += computeLighting(normal, inter_vertex, normalize(inter_vertexToPointLightDirection[i]), G_pointLightSourceColor[i], attenuation);	
			}
		}

		// spot light
		
		for (int i = 0; i < 8; ++i)
		{
			if((G_spotLightSourceRangeInverse[i] != 0.0))
			{
				vec3 ldir = inter_vertexToSpotLightDirection[i] * G_spotLightSourceRangeInverse[i];
				float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);

				vec3 vertexToSpotLightDirection = normalize(inter_vertexToSpotLightDirection[i]);
				vec3 spotlightDirection = normalize(G_spotLightSourceDirection[i] * 2.0);

				float spotCurrAngleCos = dot(spotlightDirection, -vertexToSpotLightDirection);

				attenuation *= smoothstep(G_spotLightSourceOuterAngleCos[i], G_spotLightSourceInnerAngleCos[i], spotCurrAngleCos);
				combinedColor.xyz += computeLighting(normal, inter_vertex ,vertexToSpotLightDirection, G_spotLightSourceColor[i], attenuation);			
			}
		}	
		
	}
	
	fragColor =  inter_Color * combinedColor;	
}
