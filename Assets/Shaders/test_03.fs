#version 330

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D normal_map;

// uniform float time;

/*
const float near = 1.0; 
const float far  = 100.0;
float LinearizeDepth(float depth) 
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}
*/

void main() {
	
	float ambientStrength = 0.5f;
	vec3 ambient = ambientStrength * vec3(texture(texture_diffuse1, TexCoords));

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, TexCoords));
	
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * vec3(texture(texture_diffuse1, TexCoords));  
	
	
	vec3 result = (ambient + diffuse + specular);
	
	color = vec4(result, 1.0f);
}
