#version 330 core

in vec3 position;
uniform   vec3 vNormal;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelMatrix;

out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

void main()
{
	vec4 LightPosition = vec4(1.5,0.3,0.3,1);

	fN = vNormal;
	fE = position.xyz;
	fL = LightPosition.xyz;

	if( LightPosition.w != 0.0 ) 
	{
		fL = LightPosition.xyz - position.xyz;
	}

    vec4 transformedPosition = projectionMatrix*viewingMatrix*modelMatrix*vec4(position,1.0f);
    gl_Position = transformedPosition;
}
