#include <iostream>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h>

// Other includes
#include "shader.h"
#include "camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Function prototypes
GLuint loadTexture(GLchar* path, GLboolean alpha = false);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void change_scale();
void Do_Movement();


// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 5.0f, 3.0f));
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 2.0f, -1.5f);

bool keys[1024];
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float level = 5.0f;
int drawMode = 1;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bezier Surface", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.tcs.glsl", "main.tes.glsl", "main.frag.glsl");
    Shader ourShader2("main.vert.glsl", "main.frag2.glsl");
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    // 16 control points
    GLfloat vertices[] = {
        -1.5, 1., -3.,
        -0.5, -1., -3.,
        0.5, -1., -3.,
        1.5, 1., -3.,
        
        -1.5, -1., -2.,
        -0.5, 2., -2.,
        0.5, -2., -2.,
        1.5, 0., -2.,
        
        -1.5, 0., -1.,
        -0.5, 1., -1.,
        0.5, -5., -1.,
        1.5, 0., -1.,
        
        -1.5, -3., 0.,
        -0.5, 0., 0.,
        0.5, -2., 0.,
        1.5, 2., 0.
    };
    
//    glm::vec4 p00 = glm::vec4(-1.5, 0., -3., 1);
//    glm::vec4 p10 = glm::vec4(-0.5, 0., -3.,1);
//    glm::vec4 p20 = glm::vec4(0.5, 0., -3.,1);
//    glm::vec4 p30 = glm::vec4(1.5, 0., -3.,1);
//    glm::vec4 p01 = glm::vec4(-1.5, 0., -2.,1);
//    glm::vec4 p11 = glm::vec4(-0.5, -0., -2.,1);
//    glm::vec4 p21 = glm::vec4(0.5, 0., -2.,1);
//    glm::vec4 p31 = glm::vec4(1.5, 0., -2.,1);
//    glm::vec4 p02 = glm::vec4(-1.5, 0., -1.,1);
//    glm::vec4 p12 = glm::vec4(-0.5, 0., -1.,1);
//    glm::vec4 p22 = glm::vec4(0.5, 0., -1.,1);
//    glm::vec4 p32 = glm::vec4(1.5, -0., -1.,1);
//    glm::vec4 p03 = glm::vec4(-1.5, 0., 0.,1);
//    glm::vec4 p13 = glm::vec4(-0.5, 0., 0.,1);
//    glm::vec4 p23 = glm::vec4(0.5, -0., 0.,1);
//    glm::vec4 p33 = glm::vec4(1.5, -0., 0,.1);
//
//    for(int i = 0; i < 11; i++)
//        for(int j = 0; j < 11; j++){
//            float u = i/10;
//            float v = j/10;
//            float bu0 = (1.-u) * (1.-u) * (1.-u);
//            float bu1 = 3. * u * (1.-u) * (1.-u);
//            float bu2 = 3. * u * u * (1.-u);
//            float bu3 = u * u * u;
//            float bv0 = (1.-v) * (1.-v) * (1.-v);
//            float bv1 = 3. * v * (1.-v) * (1.-v);
//            float bv2 = 3. * v * v * (1.-v);
//            float bv3 = v * v * v;
//
//            float dbu0 = - 3. * (1.-u) * (1.-u);
//            float dbu1 = 3. * (1.-3*u) * (1.-u);
//            float dbu2 = 3 * u * (2.-3*u);
//            float dbu3 = 3. * u * u;
//            float dbv0 = - 3. * (1.-v) * (1.-v);
//            float dbv1 = 3. * (1.-3*v) * (1.-v);
//            float dbv2 = 3 * v * (2.-3*v);
//            float dbv3 = 3. * v * v;
//
//            glm::vec4 pos = bu0 * ( bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03 ) + bu1 * ( bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13 ) + bu2 * ( bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23 ) + bu3 * ( bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33 );
//            glm::vec4 dP_u = dbu0 * ( bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03 ) + dbu1 * ( bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13 ) + dbu2 * ( bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23 ) + dbu3 * ( bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33 );
//            glm::vec4 dP_v = bu0 * ( dbv0*p00 + dbv1*p01 + dbv2*p02 + dbv3*p03 ) + bu1 * ( dbv0*p10 + dbv1*p11 + dbv2*p12 + dbv3*p13 ) + bu2 * ( dbv0*p20 + dbv1*p21 + dbv2*p22 + dbv3*p23 ) + bu3 * ( dbv0*p30 + dbv1*p31 + dbv2*p32 + dbv3*p33 );
//            glm::vec3 Normal = glm::cross( glm::vec3(dP_v), glm::vec3(dP_u));
//
////            std::cout<<glm::to_string(pos)<<std::endl;
//            std::cout<<glm::to_string(Normal)<<std::endl;
//        }
    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0); // Unbind VAO
    
    // Load and create a texture
    GLuint texture = loadTexture("blackwhite.png");
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        if(keys[GLFW_KEY_O]) lightPos.y -= 0.01f;
        if(keys[GLFW_KEY_P]) lightPos.y += 0.01f;
        
        std::cout<<lightPos.y<<std::endl;
        
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        change_scale();
        Do_Movement();
        
        // Render
        // Clear the colorbuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glm::mat4 model(1);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
        // Activate shader
        ourShader.Use();
        GLint lightPosLoc    = glGetUniformLocation(ourShader.Program, "light.position");
        GLint viewPosLoc     = glGetUniformLocation(ourShader.Program, "viewPos");
        glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
        
        glm::vec3 lightColor = glm::vec3(1.0f);
        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.8f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.0f); // low influence
        glm::vec3 specularColor = glm::vec3(0.5f);
        GLint lightAmbientLoc = glGetUniformLocation(ourShader.Program, "light.ambient");
        GLint lightDiffuseLoc = glGetUniformLocation(ourShader.Program, "light.diffuse");
        GLint lightSpecularLoc = glGetUniformLocation(ourShader.Program, "light.specular");
        glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
        glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);
        glUniform3f(lightSpecularLoc, specularColor.x, specularColor.y, specularColor.z);
        
        // material properties
        GLint matAmbientLoc = glGetUniformLocation(ourShader.Program, "material.ambient");
        GLint matDiffuseLoc = glGetUniformLocation(ourShader.Program, "material.diffuse");
        GLint matSpecularLoc = glGetUniformLocation(ourShader.Program, "material.specular");
        GLint matShineLoc = glGetUniformLocation(ourShader.Program, "material.shininess");
        
        glUniform3f(matAmbientLoc, 1.0f, 1.0f, 1.0f);
        glUniform3f(matDiffuseLoc, 1.0f, 1.0f, 1.0f);
        glUniform3f(matSpecularLoc, 0.2f, 0.2f, 0.2f);
        glUniform1f(matShineLoc, 32.0f);
        
        glUniform1f(glGetUniformLocation(ourShader.Program, "uOuter02"), level);
        glUniform1f(glGetUniformLocation(ourShader.Program, "uOuter13"), level);
        glUniform1f(glGetUniformLocation(ourShader.Program, "uInner0"), level);
        glUniform1f(glGetUniformLocation(ourShader.Program, "uInner1"), level);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // Draw bezier surface
        switch (drawMode) {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case 1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
        }
                
        glBindVertexArray(VAO);
        glPatchParameteri(GL_PATCH_VERTICES, 16);// Define the number of vertices for a patch
        glDrawArrays(GL_PATCHES, 0, 16);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Draw control points
        ourShader2.Use();
        glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glPointSize(10.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 16);
        glBindVertexArray(0);
        
        //Visualize the lightsource
        GLfloat lightVerts[] = {lightPos.x, lightPos.y, lightPos.z};
        GLuint lightVBO, lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);
        glBindVertexArray(lightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lightVerts), lightVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        glPointSize(30.0f);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);
        
        glUseProgram(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

GLuint loadTexture(GLchar* path, GLboolean alpha)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void change_scale()
{
    if (keys[GLFW_KEY_Z] && level > 1){
        if (level <= 20.0f)
            level -= deltaTime * 5.0f;
        else
            level -= deltaTime * 10.0f;
        level = level <= 1.0f ? 1.0f : level;
        std::cout << "\rLevel: " << level << "    ";
    }
    if (keys[GLFW_KEY_X] && level < 40){
        if (level < 20.0f)
            level += deltaTime * 5.0f;
        else
            level += deltaTime * 10.0f;
        level = level >= 40.0f ? 40.0f : level;
        std::cout << "\rLevel: " << level << "    ";
    }
    if (keys[GLFW_KEY_C]){
        drawMode = 1 - drawMode;
        std::cout << "\rDrawMode: " << drawMode << "    ";
        keys[GLFW_KEY_C] = false;
    }
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_R])
        camera.ProcessKeyboard(UP, deltaTime);
    if (keys[GLFW_KEY_F])
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

