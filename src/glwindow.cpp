#include <iostream>
#include <stdio.h>

#include "SDL.h"
#include <GL/glew.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glwindow.h"
#include "geometry.h"

using namespace std;

const char* glGetErrorString(GLenum error)
{
    switch(error)
    {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "UNRECOGNIZED";
    }
}

void glPrintError(const char* label="Unlabelled Error Checkpoint", bool alwaysPrint=false)
{
    GLenum error = glGetError();
    if(alwaysPrint || (error != GL_NO_ERROR))
    {
        printf("%s: OpenGL error flag is %s\n", label, glGetErrorString(error));
    }
}

GLuint loadShader(const char* shaderFilename, GLenum shaderType)
{
    FILE* shaderFile = fopen(shaderFilename, "r");
    if(!shaderFile)
    {
        return 0;
    }

    fseek(shaderFile, 0, SEEK_END);
    long shaderSize = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);

    char* shaderText = new char[shaderSize+1];
    size_t readCount = fread(shaderText, 1, shaderSize, shaderFile);
    shaderText[readCount] = '\0';
    fclose(shaderFile);

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const char**)&shaderText, NULL);
    glCompileShader(shader);

    delete[] shaderText;

    return shader;
}

GLuint loadShaderProgram(const char* vertShaderFilename,
                       const char* fragShaderFilename)
{
    GLuint vertShader = loadShader(vertShaderFilename, GL_VERTEX_SHADER);
    GLuint fragShader = loadShader(fragShaderFilename, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program, 1024, &logLength, message);
        cout << "Shader load error: " << message << endl;
        return 0;
    }

    return program;
}

OpenGLWindow::OpenGLWindow()
{
    parentEntity.position = glm::vec3(0.0f, 0.0f, 0.0f);
    parentEntity.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    parentEntity.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    childEntity.position = glm::vec3(1.0f, 0.0f, 0.0f);
    childEntity.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    childEntity.scale = glm::vec3(0.5f, 0.5f, 0.5f);

    colorIndex = 0;
    translateDirection = 0;
    rotateDirection = 0;
    scaleDirection = 0;
}


void OpenGLWindow::initGL()
{
    // We need to first specify what type of OpenGL context we need before we can create the window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    sdlWin = SDL_CreateWindow("OpenGL Prac 1",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              640, 480, SDL_WINDOW_OPENGL);
    if(!sdlWin)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Unable to create window", 0);
    }
    SDL_GLContext glc = SDL_GL_CreateContext(sdlWin);
    SDL_GL_MakeCurrent(sdlWin, glc);
    SDL_GL_SetSwapInterval(1);

    glewExperimental = true;
    GLenum glewInitResult = glewInit();
    glGetError(); // Consume the error erroneously set by glewInit()
    if(glewInitResult != GLEW_OK)
    {
        const GLubyte* errorString = glewGetErrorString(glewInitResult);
        cout << "Unable to initialize glew: " << errorString;
    }

    int glMajorVersion;
    int glMinorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
    cout << "Loaded OpenGL " << glMajorVersion << "." << glMinorVersion << " with:" << endl;
    cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
    cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
    cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
    cout << "\tGLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0,0,0,1);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    shader = loadShaderProgram("test2.vert", "test2.frag");
    glUseProgram(shader);

    // Set our viewing and projection matrices, since these do not change over time
    glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f), 4.0f/3.0f, 0.1f, 10.0f);
    int projectionMatrixLoc = glGetUniformLocation(shader, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLoc, 1, false, &projectionMat[0][0]);

    glm::vec3 eyeLoc(0.0f, 0.0f, 2.0f);
    glm::vec3 targetLoc(0.0f, 0.0f, 0.0f);
    glm::vec3 upDir(0.0f, 1.0f, 0.0f);
    glm::mat4 viewingMat = glm::lookAt(eyeLoc, targetLoc, upDir);
    int viewingMatrixLoc = glGetUniformLocation(shader, "viewingMatrix");
    glUniformMatrix4fv(viewingMatrixLoc, 1, false, &viewingMat[0][0]);

    // Load the model that we want to use and buffer the vertex attributes
    geometry.loadFromOBJFile("cube.obj");

    int vertexLoc = glGetAttribLocation(shader, "position");
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    //glBufferData(GL_ARRAY_BUFFER, 3*geometry.vertexCount()*sizeof(float),geometry.vertexData(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, 3*geometry.vertexCount()*sizeof(float) + 3*geometry.normals.size()*sizeof(float),NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, 3*geometry.vertexCount()*sizeof(float) , geometry.vertexData() );
    glBufferSubData( GL_ARRAY_BUFFER, 3*geometry.vertexCount()*sizeof(float),3*geometry.normals.size()*sizeof(float),  geometry.normalData() );



    //glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, false, 0, 0);
    //glEnableVertexAttribArray(vertexLoc);

    //int normalLoc = glGetAttribLocation(shader, "vNormal"); //phong shader
    //glUniformMatrix3fv(normalLoc, 1, false, (const GLfloat*)geometry.normalData());
   // glUniformMatrix3fv(normalLoc, 1, false, geometry.normalData());
    //glEnableVertexAttribArray(normalLoc);
    //glVertexAttribPointer(normalLoc,3,GL_FLOAT,GL_FALSE,)



    GLuint vPosition = glGetAttribLocation( shader, "position" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,(GLvoid*)(0) );

    GLuint vNormal = glGetAttribLocation( shader, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,(GLvoid*)( 3*geometry.vertexCount()*sizeof(float)) );



    // Initialize shader lighting parameters
    glm::vec4 light_position( 0.0, 1.0, 2.0, 0.0); //position of light source
    glm::vec4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    glm::vec4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    glm::vec4 light_specular( 1.0, 1.0, 1.0, 1.0 );
    glm::vec4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    glm::vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    glm::vec4 material_specular( 1.0, 0.0, 1.0, 1.0 );
    float  material_shininess = 5.0;

    glm::vec4 ambient_product = light_ambient * material_ambient;
    glm::vec4 diffuse_product = light_diffuse * material_diffuse;
    glm::vec4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(shader, "AmbientProduct"),1, &ambient_product[0] );
    glUniform4fv( glGetUniformLocation(shader, "DiffuseProduct"),1, &diffuse_product[0] );
    glUniform4fv( glGetUniformLocation(shader, "SpecularProduct"),1, &specular_product[0] );
    glUniform4fv( glGetUniformLocation(shader, "LightPosition"),1, &light_position[0] );
    glUniform1f( glGetUniformLocation(shader, "Shininess"),material_shininess );

    glPrintError("Setup complete", true);
}

void OpenGLWindow::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float entityColors[15] = { 1.0f, 1.0f, 1.0f,
                               1.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 1.0f,
                               0.2f, 0.2f, 0.2f };

    // NOTE: glm::translate/rotate/scale apply the transformation by right-multiplying by the
    //       corresponding transformation matrix (T). IE glm::translate(M, v) = M * T, not T*M
    //       This means that the transformation you apply last, will effectively occur first
    glm::mat4 modelMat(1.0f);
    modelMat = glm::translate(modelMat, parentEntity.position);
    modelMat = glm::rotate(modelMat, parentEntity.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::rotate(modelMat, parentEntity.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, parentEntity.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, parentEntity.scale);
    int modelMatrixLoc = glGetUniformLocation(shader, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLoc, 1, false, &modelMat[0][0]);

    int colorLoc = glGetUniformLocation(shader, "objectColor");
    glUniform3fv(colorLoc, 1, &entityColors[3*colorIndex]);

    glDrawArrays(GL_TRIANGLES, 0, geometry.vertexCount());

    // NOTE: This assumes that we're using the same mesh for the child and parent object, if
    //       You used a different mesh for the child, you would need to give it its own VAO
    //       and the bind that and upload all relevant data (IE the other matrices)

    glm::mat4 childModelMat(1.0f);
    childModelMat = glm::translate(childModelMat, childEntity.position);
    childModelMat = glm::rotate(childModelMat, childEntity.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    childModelMat = glm::rotate(childModelMat, childEntity.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    childModelMat = glm::rotate(childModelMat, childEntity.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    childModelMat = glm::scale(childModelMat, childEntity.scale);

    childModelMat = modelMat * childModelMat;
    glUniformMatrix4fv(modelMatrixLoc, 1, false, &childModelMat[0][0]);

    int childColorIndex = (colorIndex+1)%5;
    glUniform3fv(colorLoc, 1, &entityColors[3*childColorIndex]);
    glDrawArrays(GL_TRIANGLES, 0, geometry.vertexCount());

    // Swap the front and back buffers on the window, effectively putting what we just "drew"
    // onto the screen (whereas previously it only existed in memory)
    SDL_GL_SwapWindow(sdlWin);

    //glm::vec4 light_position = light_position*modelMat; //position of light source

}

// The program will exit if this function returns false
bool OpenGLWindow::handleEvent(SDL_Event e)
{
    // A list of keycode constants is available here: https://wiki.libsdl.org/SDL_Keycode
    // Note that SDL provides both Scancodes (which correspond to physical positions on the keyboard)
    // and Keycodes (which correspond to symbols on the keyboard, and might differ across layouts)
    if(e.type == SDL_KEYDOWN)
    {
        if(e.key.keysym.sym == SDLK_ESCAPE)
        {
            return false;
        }
        else if(e.key.keysym.sym == SDLK_1)
        {
            colorIndex = 0;
        }
        else if(e.key.keysym.sym == SDLK_2)
        {
            colorIndex = 1;
        }
        else if(e.key.keysym.sym == SDLK_3)
        {
            colorIndex = 2;
        }
        else if(e.key.keysym.sym == SDLK_4)
        {
            colorIndex = 3;
        }
        else if(e.key.keysym.sym == SDLK_5)
        {
            colorIndex = 4;
        }

        else if(e.key.keysym.sym == SDLK_q)
        {
            parentEntity.position[translateDirection] -= 0.5f;
        }
        else if(e.key.keysym.sym == SDLK_w)
        {
            translateDirection = (translateDirection+1)%3;
        }
        else if(e.key.keysym.sym == SDLK_e)
        {
            parentEntity.position[translateDirection] += 0.5f;
        }

        else if(e.key.keysym.sym == SDLK_a)
        {
            parentEntity.rotation[rotateDirection] -= glm::radians(15.0f);
        }
        else if(e.key.keysym.sym == SDLK_s)
        {
            rotateDirection = (rotateDirection+1)%3;
        }
        else if(e.key.keysym.sym == SDLK_d)
        {
            parentEntity.rotation[rotateDirection] += glm::radians(15.0f);
        }

        else if(e.key.keysym.sym == SDLK_z)
        {
            parentEntity.scale[scaleDirection] -= 0.2f;
        }
        else if(e.key.keysym.sym == SDLK_x)
        {
            scaleDirection = (scaleDirection+1)%3;
        }
        else if(e.key.keysym.sym == SDLK_c)
        {
            parentEntity.scale[scaleDirection] += 0.2f;
        }
    }
    return true;
}

void OpenGLWindow::cleanup()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    SDL_DestroyWindow(sdlWin);
}
