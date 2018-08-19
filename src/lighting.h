#ifndef LIGHTING_H
#define LIGHTING_H

#include "entity.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct lightSource
{   
    Entity entity;
    
    int shaderLightLocation;

    glm::vec4 light_specular;
    glm::vec4 light_diffuse;

    glm::vec4 material_specular;
    glm::vec4 material_diffuse;   

    glm::vec4 getLightPosition()
    {
    	return glm::vec4(entity.position, 0.0);
    }

    glm::vec4 getSpecularValue()
    {
    	return light_specular * material_specular;
    }

    glm::vec4 getDiffuseValue()
    {
    	return light_diffuse * material_diffuse;
    }
};

#endif