#version 330 core

in vec3 position;
in vec3 vNormal;

// output values that will be interpolated per-fragment
out vec3 fN;
out vec3 fE;
out vec3 fL;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelMatrix;
uniform vec4 LightPosition;

uniform vec4 LightPosition2;
out vec3 fL2;

void main()
{
	fN = vNormal;
	fE = position.xyz;

	//(light position follows shape transformations)
	//fL = LightPosition.xyz; 

	//NB makes light position static
	fL = (inverse(modelMatrix) * LightPosition).xyz;

	//Light 2:

	fL2 = (inverse(modelMatrix) * LightPosition2).xyz;

	vec4 transformedPosition = projectionMatrix*viewingMatrix*modelMatrix*vec4(position,1.0f);
    gl_Position = transformedPosition;
}