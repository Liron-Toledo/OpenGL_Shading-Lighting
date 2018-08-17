#version 150
// per-fragment interpolated values from the vertex shader
in  vec3 fN;
in  vec3 fL;
in  vec3 fE;

out vec4 outColor;

//uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
//uniform mat4 ModelView;
//uniform vec4 LightPosition;
//uniform float Shininess;

uniform vec3 objectColor;


void main()
{
  // Normalize the input lighting vectors
  vec3 N = normalize(fN);
  vec3 E = normalize(fE);
  vec3 L = normalize(fL);
  vec3 H = normalize(L+E);

  //vec4 ambient = AmbientProduct;
    vec4 ambient = vec4(0.5,0.5,0.5,1);


  float Kd = max(dot(L, N), 0.0);

  //vec4 diffuse = Kd*DiffuseProduct;
  vec4 diffuse = Kd*vec4(0.2,0.2,0.3,1);


  //float Ks = pow(max(dot(N, H), 0.0), Shininess);
  float Ks = pow(max(dot(N, H), 0.0), 0.5);


  //vec4 specular = Ks*SpecularProduct;
  vec4 specular = Ks*vec4(0.5,0.5,0.3,1);


  // discard the specular highlight if the light’s behind the vertex
  if( dot(L, N) < 0.0 ) 
  {
    specular = vec4(0.0, 0.0, 0.0, 1.0);
  }

  //fColor = ambient + diffuse + specular;
  //fColor.a = 1.0;

  //outColor = vec4(0.2f,0.2f,0.2f,1);
    outColor = vec4(objectColor,1);

} 
