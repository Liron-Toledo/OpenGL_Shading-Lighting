#version 330 core

in vec3 vPosition;
in vec3 vNormal;  

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
fE = vPosition.xyz;

//(light position follows shape transformations)
//fL = LightPosition.xyz; 

//NB makes light position static
fL = LightPosition.xyz * vec3(inverse(modelMatrix));

if( LightPosition.w != 0.0 ) 
{
	//fL = LightPosition.xyz - vPosition.xyz;
	fL = LightPosition.xyz * vec3(inverse(modelMatrix)) - vPosition.xyz;
}

vec4 transformedPosition = projectionMatrix*viewingMatrix*modelMatrix*vec4(vPosition,1.0f);
gl_Position = transformedPosition;




}

//potentially for second light source just add second version of all variables, LightPosition2 (with its own location in glWindow), etc