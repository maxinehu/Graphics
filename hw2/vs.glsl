#version 130

precision mediump float;

in vec4 in_position;
in vec4 in_color;
in vec4 in_normal;

out vec4 vColor;

uniform mat4 MVP;
uniform mat4 ModelN;
uniform int lighton;
uniform vec4 light;
uniform vec4 diffuse;
uniform vec4 specular;


void main(){
	
    gl_Position = MVP * in_position;  
	vec4 color = in_color;
	if(lighton == 0){
		vec4 mn = ModelN * in_normal;
		vec4 n = vec4(normalize(mn.xyz),1.0f);
		vec4 diffuse_dist = vec4(normalize(light.xyz-in_position.xyz),1.0f);
		float diffuse_intensity = max(dot(diffuse_dist, n),0.0f);
		vec4 s = vec4(normalize(vec3(light.xyz - in_position.xyz)),1.0f) + vec4(normalize(vec3(vec3(10.0f,10.0f,10.0f) - in_position.xyz)),1.0f);
		float specular_intensity = max(dot((vec4((normalize(vec3(s.xyz))),1.0f)),n), 0.0f);
		vec4 color = in_color * (diffuse_intensity+specular_intensity);
		vColor = color;
		}
	else{
		vColor = color;
		}
			
}
