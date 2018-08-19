#version 330 core


// per-fragment interpolated values from the vertex shader
in vec3 fN;
in vec3 fL;
in vec3 fE;

out vec4 fColor;

uniform vec4 AmbientProduct, DiffuseProduct1, SpecularProduct1;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform float Shininess;

uniform vec4 LightPosition2;
uniform vec4 DiffuseProduct2, SpecularProduct2;
in vec3 fL2;

void main()
{
	// Normalize the input lighting vectors
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);
	vec3 H = normalize(L+E);

	float Kd = max(dot(L, N), 0.0);
	float Ks = pow(max(dot(N, H), 0.0), Shininess);

	vec4 ambient = AmbientProduct;
	//vec4 ambient = vec4(0.1f  ,0.1f,0.1f,1);

	vec4 diffuse = Kd*DiffuseProduct1;
	vec4 specular = Ks*SpecularProduct1;

	// discard the specular highlight if the light’s behind the vertex
	if( dot(L, N) < 0.0 ) 
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fColor = ambient + diffuse + specular;
	fColor.a = 1.0;

	//Light 2:

	vec3 L2 = normalize(fL2);
	vec3 H2 = normalize(L2+E);

	float Kd2 = max(dot(L2, N), 0.0);
	float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);

	vec4 diffuse2 = Kd2*DiffuseProduct2;
	vec4 specular2 = Ks2*SpecularProduct2;

	// discard the specular highlight if the light’s behind the vertex
	if( dot(L2, N) < 0.0 ) 
	{
		specular2 = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fColor = fColor + ambient + diffuse2 + specular2;
}