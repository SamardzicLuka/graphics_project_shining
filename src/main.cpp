#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char* texPath);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPosition(1.5f, 1.6f, 2.8f);

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

Camera kamerica;

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 backpackPosition = glm::vec3(0.0f);
    float backpackScale = 1.0f;
    PointLight pointLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};
void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}
void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);
void moveLight(Camera_Movement smer);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    programState = new ProgramState;
//    programState->LoadFromFile("resources/program_state.txt");
//    if (programState->ImGuiEnabled) {
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//    }
//    // Init Imgui
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO &io = ImGui::GetIO();
//    (void) io;
//
//
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 330 core");
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }//IMGUI

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST); // DA NE VIDIMO U ISTO VREME I SPOLJA I IZNUTRA

    // build and compile shaders
    // -------------------------
    Shader notCubemapShader("resources/shaders/texture.vs", "resources/shaders/texture.fs");
    Shader lightSourceShader("resources/shaders/source.vs", "resources/shaders/source.fs");

    kamerica.Position = glm::vec3(0.0f, 0.0f,  3.0f); // POZICIJA KAMERE
    kamerica.Front = glm::vec3(0.0f, 0.0f, -1.0f); // VEKTOR GLEDANJA UNAPRED
    kamerica.Up = glm::vec3(0.0f, 1.0f,  0.0f); // VEKTOR NA GORE

// PRIPREMA

    // POD

    // DEFINISANJE VERTEXA KOJE CEMO KORISTITI KAO TEPIH
        float carpetVertices[] = {
           // positions                      // normals                        // texture coords
            0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,1.0f,  1.0f,  // top right
            0.5f, -0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  1.0f,  0.0f,  // bottom right
            -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, -1.0f,0.0f,  0.0f,  // bottom left
            -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0f   // top left
        };

        // KORISTIMO ONAJ EBO KOJI SMO RADILI NA DRUGOM(?) CASU KADA SE PONAVLJAJU VERTICES
        unsigned int carpetIndices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };

        // VAOs, VBOs AND EBOs
        unsigned int carpetVAO, carpetVBO, carpetEBO;
        glGenVertexArrays(1,&carpetVAO);
        glGenBuffers(1, &carpetVBO);
        glGenBuffers(1, &carpetEBO);

        glBindVertexArray(carpetVAO);
        glBindBuffer(GL_ARRAY_BUFFER, carpetVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(carpetVertices), carpetVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carpetEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(carpetIndices), carpetIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


    // KOCKA

    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // IZVOR SVETLOSTI

    unsigned int lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

// UCITAVANJE TEXTURA

    // POD
    unsigned int carpetTexture = loadTexture(FileSystem::getPath("resources/textures/carpet.jpg").c_str());
    unsigned int carpetSpecularMap = loadTexture(FileSystem::getPath("resources/textures/specular_carpet.jpg").c_str());

    // KOCKA

    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/awesomeface.png").c_str());
    unsigned int cubeSpecularMap = loadTexture(FileSystem::getPath("resources/textures/specular_cube.jpg").c_str());

    {
        // load models
        // -----------
        {//    Model ourModel("resources/objects/medieval book/TEST2.fbx");
//    ourModel.SetShaderTextureNamePrefix("material.");

//    PointLight& pointLight = programState->pointLight;
//    pointLight.position = glm::vec3(4.0f, 4.0, 0.0);
//    pointLight.ambient = glm::vec3(1, 1, 1);
//    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
//    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);
//
//    pointLight.constant = 1.0f;
//    pointLight.linear = 0.09f;
//    pointLight.quadratic = 0.032f;
        } //LOADING MODELS- JOS NISAM STIGAO DO OVDE
    } // MODELI

    notCubemapShader.use();
    notCubemapShader.setInt("material.diffuse", 0);
    notCubemapShader.setInt("material.specular", 1);
    notCubemapShader.setFloat("material.shininess", 1.0f);


    // PRAVIMO MATRICU PROJEKCIJE
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    notCubemapShader.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        {///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }//IMGUI
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // INTITALIZE OUR SHADER
        notCubemapShader.use();
        notCubemapShader.setVec3("viewPos", kamerica.Position);

        notCubemapShader.setVec3("dirLight.direction", 1.0f, -0.5f, 0.0f);
        notCubemapShader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
        notCubemapShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        notCubemapShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        notCubemapShader.setVec3("pointLight.position", lightPosition);
        notCubemapShader.setVec3("pointLight.ambient", 0.01f, 0.01f, 0.01f);
        notCubemapShader.setVec3("pointLight.diffuse", 1.5f, 0.8f, 0.8f);
        notCubemapShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        notCubemapShader.setFloat("pointLight.constant", 1.0f);
        notCubemapShader.setFloat("pointLight.linear", 0.09f);
        notCubemapShader.setFloat("pointLight.quadratic", 0.032f);

        notCubemapShader.setVec3("spotLight.position", kamerica.Position);
        notCubemapShader.setVec3("spotLight.direction", kamerica.Front);
        notCubemapShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        notCubemapShader.setVec3("spotLight.diffuse", 0.7f, 0.7f, 0.7f);
        notCubemapShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        notCubemapShader.setFloat("spotLight.constant", 1.0f);
        notCubemapShader.setFloat("spotLight.linear", 0.05);
        notCubemapShader.setFloat("spotLight.quadratic", 0.012);
        notCubemapShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.5f)));
        notCubemapShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(13.0f)));

        glm::mat4 view          = glm::lookAt(kamerica.Position, kamerica.Position+kamerica.Front, kamerica.Up);
        notCubemapShader.setMat4("view", view);


// WORK ON OUR FLOOR
// MODEL AND VIEW WE WILL USE TO RENDER FLOOR
        glm::mat4  model(1.0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.51f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f));
        notCubemapShader.setMat4("model", model);
        notCubemapShader.setInt("texture1", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, carpetTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, carpetSpecularMap);

        // IT IS A DRAWING, OF A KEY (CARPET)
        glBindVertexArray(carpetVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


// CUBE AS A REFERENCE POINT

        model = glm::mat4(1.0f);
        glm::vec3 translVektor (3.0f, 1.0f, 1.0f);
        model = glm::translate(model, translVektor);
        model = glm::scale(model, glm::vec3(0.5f));
        notCubemapShader.setMat4("model", model);
        notCubemapShader.setFloat("material.shininess", 3.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cubeSpecularMap);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

// LIGHT SOURCE

        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightSourceShader.setMat4("model", model);

        glBindVertexArray(lightSourceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        {// don't forget to enable shader before setting uniforms
//        notCubemapShader.use();
//        pointLight.position = glm::vec3(4.0 * cos(currentFrame), 4.0f, 4.0 * sin(currentFrame));
//        notCubemapShader.setVec3("pointLight.position", pointLight.position);
//        notCubemapShader.setVec3("pointLight.ambient", pointLight.ambient);
//        notCubemapShader.setVec3("pointLight.diffuse", pointLight.diffuse);
//        notCubemapShader.setVec3("pointLight.specular", pointLight.specular);
//        notCubemapShader.setFloat("pointLight.constant", pointLight.constant);
//        notCubemapShader.setFloat("pointLight.linear", pointLight.linear);
//        notCubemapShader.setFloat("pointLight.quadratic", pointLight.quadratic);
//        notCubemapShader.setVec3("viewPosition", programState->camera.Position);
//        notCubemapShader.setFloat("material.shininess", 32.0f);
//        // view/projection transformations
//        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
//                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
//        glm::mat4 view = programState->camera.GetViewMatrix();
//        notCubemapShader.setMat4("projection", projection);
//        notCubemapShader.setMat4("view", view);
//
//        // render the loaded model
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model,
//                               programState->backpackPosition); // translate it down so it's at the center of the scene
//        model = glm::scale(model, glm::vec3(programState->backpackScale));    // it's a bit too big for our scene, so scale it down
//        notCubemapShader.setMat4("model", model);
//        ourModel.Draw(notCubemapShader);
        } // MODELI SEJDERI
        {//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//        if (programState->ImGuiEnabled)
//            DrawImGui(programState);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }//IMGUI

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &carpetVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &carpetVBO);
    glDeleteBuffers(1, &carpetEBO);
    {//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    programState->SaveToFile("resources/program_state.txt");
//    delete programState;
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }   //IMGUI
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        kamerica.ProcessKeyboard(FORWARD, deltaTime/2.0); // POMERA SE KAMERA U KOM GLEDAMO
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        kamerica.ProcessKeyboard(BACKWARD, deltaTime/2.0);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        kamerica.ProcessKeyboard(LEFT, deltaTime/2.0);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        kamerica.ProcessKeyboard(RIGHT, deltaTime/2.0);
    {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      //  programState->camera.ProcessKeyboard(FORWARD, deltaTime * 6);
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      //  programState->camera.ProcessKeyboard(BACKWARD, deltaTime * 6);
    ///if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
       /// programState->camera.ProcessKeyboard(LEFT, deltaTime * 6);
    ///if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
       /// programState->camera.ProcessKeyboard(RIGHT, deltaTime * 6);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // IMGUI

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        moveLight(FORWARD);
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        moveLight(BACKWARD);
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        moveLight(LEFT);
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        moveLight(RIGHT);

    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        moveLight(UP);
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        moveLight(DOWN);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos; // AKO JE OO PRVO POMERANJE MISEM, POSTAVLJAMO POZICIJU NA KOJOJ SMO PRETHODNO BILI NA OVU TRENUTNU
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX; // ZA KOLIKO SE POMERAMO
    float yoffset = lastY - ypos; // ZA KOLIKO SE POMERAMO; reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // KOLIKO BRZO SE POMERA KAMERA PRI POMERANJU MISA
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    kamerica.Yaw += xoffset; // IZRACUNAVAMO ONE UGLOVE
    kamerica.Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    // DA NAM NE BI KAMERA RADILA BACKFLIP. ONO KAD SE POMERAMO NPR. NA GORE I KAD STIGNEMO DO 90 STEPENI.
    if (kamerica.Pitch > 89.0f)
        kamerica.Pitch = 89.0f;
    if (kamerica.Pitch < -89.0f)
        kamerica.Pitch = -89.0f;

    // RACUNAMO VEKTOR SMERA GLEDANJA
    glm::vec3 front;
    front.x = cos(glm::radians(kamerica.Yaw)) * cos(glm::radians(kamerica.Pitch));
    front.y = sin(glm::radians(kamerica.Pitch));
    front.z = sin(glm::radians(kamerica.Yaw)) * cos(glm::radians(kamerica.Pitch));
    kamerica.Front = glm::normalize(front); // OVDE POSTAVLJAMO KONACNO SMER GLEDANJA KAMERE

//    if (programState->CameraMouseMovementUpdateEnabled)
//        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(const char* texPath){
    unsigned int texObject;
    glGenTextures(1, &texObject);
    glBindTexture(GL_TEXTURE_2D,texObject);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    int width, height, noChannels;
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/carpet.jpg").c_str(), &width, &height, &noChannels, 0);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // !!!!!!!!!!!!!!!!!!!!!!!! TASK_NO_1
        // OVDE CE NESTO MORATI DA SE DODA. POGLEDAJ KNJIGU POSLE I POGLEDAJ U KODOVIMA KAD SE PRVI PUT
        // POJAVLJUJE ONAJ ENUM. JA MISLIM DA JE TO ZBOG MODELA
    }
    else{
        std::cout<<"Neuspesno ucitavanje tekstura\n";
    }
    stbi_image_free(data);
    return texObject;

}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("Backpack position", (float*)&programState->backpackPosition);
        ImGui::DragFloat("Backpack scale", &programState->backpackScale, 0.05, 0.1, 4.0);

        ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void moveLight(Camera_Movement smer){
    float velocity = 2.0f*deltaTime;
    glm::vec3 yLock(1.0f, 0.0f, 1.0f);
    glm::vec3 yMove(0.0f, 1.0f, 0.0f);

    if (smer == FORWARD)
        lightPosition += kamerica.Front * velocity * yLock;
    if (smer == BACKWARD)
        lightPosition -= kamerica.Front * velocity * yLock;
    if (smer == LEFT)
        lightPosition -= kamerica.Right * velocity * yLock;
    if (smer == RIGHT)
        lightPosition += kamerica.Right * velocity * yLock;

    if (smer == UP)
        lightPosition += velocity * yMove;
    if (smer == DOWN)
        lightPosition -= velocity * yMove;

    if (lightPosition.y < 0.0f)
        lightPosition.y = 0.0f;
    else if (lightPosition.y > 3.0f)
        lightPosition.y = 3.0f;
}
