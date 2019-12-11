// Std. Includes
#include <string>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader.h"
#include "camera.h"
#include "texture.hpp"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Other Libs
#include <SOIL/SOIL.h>

//Customized Classes
#include "skybox.hpp"
#include "water.hpp"
#include "terrain.hpp"

// Properties
GLuint WIDTH = 800, HEIGHT = 600;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Terrain Engine", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    
    // Create Frame Buffers and their Textures
    GLuint reflectBuffer;
    glGenFramebuffers(1, &reflectBuffer);
    GLuint reflecTex = frameBufferTexture(reflectBuffer, WIDTH, HEIGHT);
    
    GLuint refractBuffer;
    glGenFramebuffers(1, &refractBuffer);
    GLuint refracTex = frameBufferTexture(refractBuffer, WIDTH, HEIGHT);
    
    // Create Scene Objects
    SkyBox sky;
    Water water(1000.0f, 0.0f);
    Terrain terrain(0.4f, 0.04f, -3.0f);
    water.loadTextures("data/Water/diffuse.bmp", "data/Water/wave0.png", "data/Water/wave1.png", "data/Water/flowmap.png", "data/Water/noise.png");
    terrain.loadHeightmap("data/Terrain/heightmap.bmp");
    terrain.loadTextures("data/Terrain/diffuse.bmp", "data/Terrain/detail.bmp");
    
    
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // Clear the colorbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up matrices
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::vec3 viewPos = camera.Position;
        
        // Render reflection to Texture
        glBindFramebuffer(GL_FRAMEBUFFER, reflectBuffer);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        terrain.Render(view, projection, viewPos, RenderType::reflection);
        
        // Render refraction to Texture
        glBindFramebuffer(GL_FRAMEBUFFER, refractBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        terrain.Render(view, projection, viewPos, RenderType::refraction);
        
        // Draw Objects To Default Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        sky.Render(view, projection);
        water.Render(view, projection, viewPos, sky.getTexSkybox(), reflecTex, refracTex, currentFrame);
        terrain.Render(view, projection, viewPos, RenderType::visible);
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    GLfloat displacement = 2.0f * deltaTime;
    // Camera controls
    if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, displacement);
    if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, displacement);
    if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, displacement);
    if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, displacement);
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

