#version 130

precision mediump float;

in vec4 in_position;
in vec4 in_color;
in vec4 in_normal;
in vec2 in_texcoord;

vec4 light_position = vec4(-10,10,-10,1);
vec4 light_diffuse = vec4(1,1,1,1);
vec4 light_specular = vec4(1,1,1,1);
vec4 material_diffuse = in_color;
vec4 material_specular = in_color;

out vec4 vColor;
out vec2 vTexCoord;

uniform int green;
uniform int light;

uniform mat3 normalmatrix;
uniform mat4 v_inv;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;


void main()
{
    vec3 normalDirection= normalize(normalmatrix *  normalize(in_normal.xyz));
	vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - M * in_position));
	vec3 lightDirection = normalize(vec3(light_position - M * in_position));

	vec3 vertex_position= vec3(M*in_position);

	vec3 diffuseReflection = vec3(light_diffuse)*vec3(material_diffuse)
					*max(0.0, dot(normalDirection, normalize(vec3(light_position)-vertex_position)));

	vec3 specularReflection = vec3(light_specular) * vec3(material_specular)
					* pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), 20);

    gl_Position =P*V*M* in_position;

	if(light==1)
		//vColor = vec4(1.0,0,0,1.0);
		//vColor = vec4(in_normal.xyz,1);
		vColor = vec4(diffuseReflection+specularReflection,1.0);
	else
		//vColor = vec4(0.0,1.0,0,1.0); 
		vColor = in_color;
	//if(green==1)
	//	vColor = vec4(0,1,0,1);

	vTexCoord = in_texcoord;
}
