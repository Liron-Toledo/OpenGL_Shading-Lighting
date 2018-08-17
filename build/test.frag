#version 330 core

uniform vec3 objectColor;

out vec4 outColor;

in  vec3 fN;
in  vec3 fL;
in  vec3 fE;

void main()
{
	  vec3 N = normalize(fN);
  	vec3 E = normalize(fE);
  	vec3 L = normalize(fL);
  	vec3 H = normalize(L+E);

    //float Kd = min(max(dot(L, N)/length(L)/length(N), 0.0), 0.99f);
  	//float Kd = max(dot(L, N)/length(L)/length(N), 0.0);
    float Kd = max(dot(L, N), 0.0);

  	float Ks = pow(max(dot(N, H), 0.0), 0.5);



    //vec4 ambient = vec4(0.1f ,0.1f+ Kd,0.1f+ Kd,1);
    //ambient.r  = ambient.r + Kd;

    vec4 ambient = vec4(0.1f  ,0.1f,0.1f,1);

    //float dott = abs(dot(L, N) * 0.5f);
    //vec4 ambient = vec4(dott + 0.5f ,0.5f + dott,0.5f ,1);
    //ambient.r = ambient.r + 1000000;



  	vec4 diffuse = Kd*vec4(1,1,1,1);
  	vec4 specular = Ks*vec4(0.7,0.7,0.7,1);

	 if( dot(L, N) < 0.0 ) 
  	{
   		specular = vec4(0.0, 0.0, 0.0, 1.0);
  	}

    ambient = clamp(ambient, 0.04f,0.99f);
    diffuse = clamp(diffuse, 0.01f,0.99f);
    specular = clamp(specular, 0.01f,0.99f);


	  outColor = ambient + diffuse + specular;
	//outColor =  diffuse + specular;
    //outColor = ambient;


    outColor = clamp(outColor, 0.01f,0.99f);
  	outColor = outColor*vec4(objectColor,1.0);
  	outColor = clamp(outColor, 0.01f,0.99f);
    outColor.a = 1.0;

    //outColor = vec4(objectColor,1);
}
