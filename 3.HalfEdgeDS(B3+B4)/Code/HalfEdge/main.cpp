#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Other includes
#include "transform.hpp"
#include "lighting.hpp"
#include "camera.h"
#include "shader.h"
#include "HalfEdge.hpp"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
void update_render_mode();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera and Lighting
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));

Light light(glm::vec3(10.0f, 10.0f, 10.0f),//position
            glm::vec3(0.1f, 0.1f, 0.1f),//ambient
            glm::vec3(1.0f, 1.0f, 1.0f),//diffuse
            glm::vec3(1.0f, 1.0f, 1.0f));//specular

Material defaultMaterial(glm::vec3(1.0f, 0.0f, 0.0f),//ambient
                  glm::vec3(0.8f, 0.0f, 0.0f),//diffuse
                  glm::vec3(0.5f, 0.0f, 0.0f),//specular
                  32.0f);//shininess

// Model
char modelpath[] = "models/bunny.obj";

Transform bunnyTrans(glm::vec3(0.0f, 0.0f, 0.0f),//position
                     glm::vec3(0.0f, 30.0f, 0.0f),//rotation
                     glm::vec3(1.0f, 1.0f, 1.0f) * 0.008f );//scale

Transform& modelTrans = bunnyTrans;

bool keys[1024];
bool firstMouse = true;
bool isRotating = false;
double scaleFactor = 1.0f;
GLfloat lastX = 400, lastY = 300;

bool show_vertices = false, show_edges = false, show_faces = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Half Edge Model Loading", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    MeshHDS myModel(modelpath);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();
        update_render_mode();
        
        // Clear the colorbuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // update MVP related params
        glm::mat4 view = camera.GetViewMatrix();
        modelTrans.scaleBy(scaleFactor);
        
        // Render: www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
        // offset the wireframe to avoid z-fighting
        
        if(show_vertices){
            glEnable(GL_POLYGON_OFFSET_POINT);
            glPolygonOffset(-2,-2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            myModel.Render(modelTrans.getModelMat(), view, projection, glm::vec3(0.6f));
            glDisable(GL_POLYGON_OFFSET_POINT);
        }
        
        if(show_edges){
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1,-1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            myModel.Render(modelTrans.getModelMat(), view, projection, glm::vec3(0.2f));
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
        
        if(show_faces){
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            myModel.Render(modelTrans.getModelMat(), view, projection, camera.Position, light, defaultMaterial);
        }
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
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

void do_movement()
{
    // Camera controls
    GLfloat cameraSpeed = 5.0f * deltaTime;
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, cameraSpeed);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, cameraSpeed);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, cameraSpeed);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, cameraSpeed);
}

void update_render_mode(){
    if (keys[GLFW_KEY_F]) { show_faces = ! show_faces; keys[GLFW_KEY_F] = false; }
    if (keys[GLFW_KEY_E]) {show_edges = ! show_edges; keys[GLFW_KEY_E] = false; }
    if (keys[GLFW_KEY_V]) {show_vertices = ! show_vertices;  keys[GLFW_KEY_V] = false; }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    if(isRotating){
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to up
        modelTrans.Rotate(glm::vec3(-yoffset * 0.5f,xoffset * 0.5f,0));
    }
    lastX = xpos;
    lastY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        isRotating = true;
    else
        isRotating = false;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scaleFactor += yoffset * 0.01f;
    scaleFactor = fmax(0.2f, fmin(5.0f,scaleFactor));
}
