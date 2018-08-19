#ifndef LIGHTING_H
#define LIGHTING_H

#include "entity.h"
#include <glm/vec3.hpp>

struct lightSource
{
    Entity values;
    int lightLoc;
    
    glm::vec4 specular;
    glm::vec4 diffuse;
    glm::vec4 materialSpecular;
    glm::vec4 materialDiffuse;   
    

    glm::vec4 getLightPosition()
    {
    	return glm::vec4(values.position, 0.0);
    }

    glm::vec4 getDiffuse()
    {
        return diffuse * materialDiffuse;
    }

    glm::vec4 getSpecular()
    {
    	return specular * materialSpecular;
    }

    
};

#endif