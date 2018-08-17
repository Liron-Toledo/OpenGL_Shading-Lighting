#version 150

in   vec3 vPosition;
uniform   vec3 vNormal;

// output values that will be interpolated per-fragment
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

//uniform mat4 ModelView;
//uniform vec4 LightPosition;
//uniform mat4 Projection;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelMatrix;

void main()
{
	vec4 LightPosition = vec4(5,0,0,1);


	fN = vNormal;
	fE = vPosition.xyz;
	fL = LightPosition.xyz;


	if( LightPosition.w != 0.0 ) 
	{
		fL = LightPosition.xyz - vPosition.xyz;
	}

	gl_Position = projectionMatrix * viewingMatrix * modelMatrix * vec4(vPosition,1.0f);
}
   
