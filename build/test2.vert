#version 330 core


in   vec3 position;
in   vec3 vNormal;

// output values that will be interpolated per-fragment
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelMatrix;
uniform vec4 LightPosition;


void main()
{

fN = vNormal;
fE = position.xyz;
fL = LightPosition.xyz;

if( LightPosition.w != 0.0 ) {
	fL = LightPosition.xyz - position.xyz;
}

	vec4 transformedPosition = projectionMatrix*viewingMatrix*modelMatrix*vec4(position,1.0f);
    gl_Position = transformedPosition;

}

//potentially for second light source just add second version of all variables, LightPosition2 (with its own location in glWindow), etc