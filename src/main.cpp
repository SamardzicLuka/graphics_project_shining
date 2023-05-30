#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include<cstdlib>
#include<cmath>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char* texPath);
unsigned int loadCubemap(vector<std::string> sides);
void moveLight(Camera_Movement smer);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool blinn = false;
bool blinnActive = false;
bool lampa = false;
bool lampaActive = false;

// camera
Camera kamerica;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPosition(1.5f, 1.6f, 2.8f);

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
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // DA LI ZELIMO DA NAM SE KURSOR VIDI NA EKRANU

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST); // DA NE VIDIMO U ISTO VREME I SPOLJA I IZNUTRA
    glEnable(GL_BLEND); //OMOGUCAVAMO BLENDING
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//PODESAVAMO ONU FORMULU

    kamerica.Position = glm::vec3(0.0f, 2.0f,  3.0f); // POZICIJA KAMERE
    kamerica.Front = glm::vec3(0.0f, 0.0f, -1.0f); // VEKTOR GLEDANJA UNAPRED
    kamerica.Up = glm::vec3(0.0f, 1.0f,  0.0f); // VEKTOR NA GORE

    // build and compile shaders
    // -------------------------
    Shader carpetShader("resources/shaders/texture.vs", "resources/shaders/texture.fs");
    Shader lightSourceShader("resources/shaders/source.vs", "resources/shaders/source.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");

// PRIPREMA

    // DEFINISANJE VERTEXA KOJE CEMO KORISTITI KAO TEPIH
    float carpetVertices[] = {
           // positions                      // normals                        // texture coords
            0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,1.0f,  1.0f,  // top right
            0.5f, -0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  1.0f,  0.0f,  // bottom right
            -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, -1.0f,0.0f,  0.0f,  // bottom left
            -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0f   // top left
        };

    // KORISTIMO ONAJ EBO KOJI SMO RADILI NA DRUGOM CASU KADA SE PONAVLJAJU VERTICES
    unsigned int carpetIndices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };

    // KOCKE
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

    // SKYBOX
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    // CARPET SETUP
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


    // CUBE SETUP
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

    // SOURCE SETUP

    unsigned int lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // SKYBOX SETUP
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

// UCITAVANJE TEXTURA

    // KOCKA
    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/stone.jpg").c_str());
    unsigned int cubeSpecularMap = loadTexture(FileSystem::getPath("resources/textures/specular_cube.jpg").c_str());

    // POD
    unsigned int carpetTexture = loadTexture(FileSystem::getPath("resources/textures/carpet.jpg").c_str());
    unsigned int carpetSpecularMap = loadTexture(FileSystem::getPath("resources/textures/specular_carpet.jpg").c_str());

    // SKKYBOX
    vector<std::string> faces{
            FileSystem::getPath("resources/textures/skybox/right.jpg"),
            FileSystem::getPath("resources/textures/skybox/left.jpg"),
            FileSystem::getPath("resources/textures/skybox/top.jpg"),
            FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
            FileSystem::getPath("resources/textures/skybox/front.jpg"),
            FileSystem::getPath("resources/textures/skybox/back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // MODEL
    unsigned int modelSpec = loadTexture(FileSystem::getPath("resources/objects/axe/basecolor.png").c_str());
    carpetShader.use();
    carpetShader.setInt("material.texture_diffuse1", 0);
    carpetShader.setInt("material.texture_specular1", 1);
    carpetShader.setFloat("material.shininess", 1.0f);


// PRAVIMO MATRICU PROJEKCIJE
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    carpetShader.setMat4("projection", projection);

    // UCITAVANJE MODELA

    Model axe(FileSystem::getPath("resources/objects/axe/axeLP.fbx").c_str());
    axe.SetShaderTextureNamePrefix("material.");

    while (!glfwWindowShouldClose(window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // INTITALIZE OUR SHADER
        carpetShader.use();
        carpetShader.setVec3("viewPos", kamerica.Position);
        carpetShader.setFloat("material.shininess", 16.0f);
        carpetShader.setBool("blinn", blinn);
        carpetShader.setInt("lampa", lampa);

        carpetShader.setVec3("dirLight.direction", 1.0f, -0.5f, 0.0f);
        carpetShader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
        carpetShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        carpetShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        carpetShader.setVec3("pointLight.position", lightPosition);
        carpetShader.setVec3("pointLight.ambient", 0.01f, 0.01f, 0.01f);
        carpetShader.setVec3("pointLight.diffuse", 1.5f, 0.8f, 0.8f);
        carpetShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        carpetShader.setFloat("pointLight.constant", 1.0f);
        carpetShader.setFloat("pointLight.linear", 0.09f);
        carpetShader.setFloat("pointLight.quadratic", 0.032f);

        carpetShader.setVec3("spotLight.position", kamerica.Position);
        carpetShader.setVec3("spotLight.direction", kamerica.Front);
        carpetShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        carpetShader.setVec3("spotLight.diffuse", 0.7f, 0.7f, 0.7f);
        carpetShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        carpetShader.setFloat("spotLight.constant", 1.0f);
        carpetShader.setFloat("spotLight.linear", 0.05);
        carpetShader.setFloat("spotLight.quadratic", 0.012);
        carpetShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.5f)));
        carpetShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(13.0f)));

        glm::mat4 view          = glm::lookAt(kamerica.Position, kamerica.Position+kamerica.Front, kamerica.Up);
        carpetShader.setMat4("view", view);

// WORK ON OUR FLOOR
// MODEL AND VIEW WE WILL USE TO RENDER FLOOR

        carpetShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        carpetShader.setFloat("material.shininess", 256.0f);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f));
        carpetShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, carpetTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, carpetSpecularMap);

        // IT IS A DRAWING, OF A KEY (CARPET)
        glBindVertexArray(carpetVAO);
        glEnable(GL_CULL_FACE);     // floor won't be visible if looked from bellow
        glCullFace(GL_BACK);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisable(GL_CULL_FACE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

// CUBE AS A REFERENCE POINT

        model = glm::mat4(1.0f);
        glm::vec3 translVektor (3.0f, 1.0f, 1.0f);
        model = glm::translate(model, translVektor);
        model = glm::scale(model, glm::vec3(0.5f));
        carpetShader.use();
        carpetShader.setMat4("model", model);
        carpetShader.setFloat("material.shininess", 3.0f);

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

// CRTAMO MODEL
        carpetShader.use();
        glm::mat4 modelSikire = glm::mat4(1.0f);
        modelSikire = glm::scale(modelSikire, glm::vec3(0.2f));
        modelSikire = glm::rotate(modelSikire, (float)glfwGetTime(), glm::vec3(1.0f));
        modelSikire = glm::translate(modelSikire, glm::vec3(0.0f, 2.0f, 1.0f));
        carpetShader.setMat4("model", modelSikire);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, modelSpec);
        axe.Draw(carpetShader);

//SKYBOX
        // skybox shader setup
        // -----------
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(kamerica.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // render skybox cube

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &carpetVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &lightSourceVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &carpetVBO);
    glDeleteBuffers(1, &carpetEBO);
    glDeleteBuffers(1, &skyboxVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly

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

    // BLINN-PHONG
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !blinnActive)
    {
        blinn = !blinn;
        blinnActive = true;
        if (blinn)
            cout << "Blinn-Phong" << endl;
        else
            cout << "Phong" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        blinnActive = false;
    }
    // LAMPA
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lampaActive)
    {
        lampa = !lampa;
        lampaActive = true;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
    {
        lampaActive = false;
    }
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called

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

}

unsigned int loadTexture(const char* texPath){
    unsigned int texObject;
    glGenTextures(1, &texObject);
    glBindTexture(GL_TEXTURE_2D,texObject);


    int width, height, noChannels;
    unsigned char *data = stbi_load(texPath, &width, &height, &noChannels, 0);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        GLenum format = GL_RED;
        if (noChannels == 1)
            format = GL_RED;
        else if (noChannels == 3)
            format = GL_RGB;
        else if (noChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texObject);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        stbi_image_free(data);
    }
    else{
        std::cout<<"Neuspesno ucitavanje tekstura\n"<<texPath;
        stbi_image_free(data);
    }
    return texObject;
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

unsigned int loadCubemap(vector<std::string> sides){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < sides.size(); i++)
    {
        unsigned char *data = stbi_load(sides[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << sides[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    kamerica.ProcessMouseScroll(yoffset);
}
