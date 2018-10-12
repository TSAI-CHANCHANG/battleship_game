#version 330 core

precision highp float;

out vec4 gl_FragColor;

in vec2 v_Coord;
in vec4 v_particleColor;

uniform sampler2D u_textureMap;

void main() {
	vec4 texColor=texture2D(u_textureMap, v_Coord);
	if(texColor.r>=0.98&&texColor.g>=0.98&&texColor.b>=0.98)
	{
		gl_FragColor=vec4(1,1,1,0);
	}
	else{
		gl_FragColor = (texColor*v_particleColor);
	}

    

}