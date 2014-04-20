#version 130 
precision mediump float;

in  vec4 vColor;
in  vec2 vTexCoord;
uniform sampler2D face_tex;
uniform int texture_flag;

out vec4 frag_color;

void main() 
{
	if ( distance(vTexCoord , vec2(8.f/19.f+0.015,9.f/19.f-0.005)) <0.015 )
		discard;
	if ( distance(vTexCoord , vec2(8.f/19.f+0.015,7.5f/19.f)) <0.015 )
		discard;
	if ( vTexCoord.x > 13.f/19.f)
		discard;
	if( texture_flag == 1 )
		frag_color = texture(face_tex, vTexCoord);
	else
		frag_color = vColor;
} 
