#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "basic_camera.h"
#include "shader.h"
#include "sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void updateAnimations();
void applyCameraWalkConstraints();
float sampleFloorHeight(const glm::vec3& pos);
void printControls();
void printCameraState();

bool firstMouse = true;
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;

unsigned int loadTexture(const char* path);
void setupCubeBuffers();
glm::mat4 makeModel(const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale);
void setMaterial(Shader& shader, glm::vec3 color, glm::vec3 specular = glm::vec3(0.35f), float shininess = 32.0f, glm::vec3 emissive = glm::vec3(0.0f));
void drawCubeShape(Shader& shader, const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale,
    glm::vec3 color, glm::vec3 specular = glm::vec3(0.35f), float shininess = 32.0f,
    glm::vec3 emissive = glm::vec3(0.0f));
void drawCubeShapeTextured(Shader& shader, const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale,
    glm::vec3 color, unsigned int texID, glm::vec3 specular = glm::vec3(0.35f), float shininess = 32.0f,
    glm::vec3 emissive = glm::vec3(0.0f));
void drawSphereShape(Shader& shader, const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale,
    glm::vec3 color, glm::vec3 specular = glm::vec3(0.35f), float shininess = 32.0f,
    glm::vec3 emissive = glm::vec3(0.0f));

void drawFrontOpeningFrame(Shader& shader, const glm::mat4& parent, glm::vec3 center, float width, float height,
    glm::vec3 frameColor, int mullions = 4, glm::vec3 glow = glm::vec3(0.0f));
void drawSideGlassWall(Shader& shader, const glm::mat4& parent, glm::vec3 center, float depth, float height,
    glm::vec3 glassColor = glm::vec3(0.66f, 0.78f, 0.90f));
void drawWoodAccentPanel(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale);
void drawSignPanel(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale, bool twoLines = true);
void drawPlanterStrip(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale,
    glm::vec3 hedgeColor = glm::vec3(0.24f, 0.56f, 0.24f));
void drawPottedPlant(Shader& shader, const glm::mat4& parent, glm::vec3 center, float scale = 1.0f);
void initBezierVaseMesh();
void destroyBezierVaseMesh();
glm::vec2 cubicBezier2D(const std::array<glm::vec2, 4>& controlPoints, float t);
glm::vec2 cubicBezierTangent2D(const std::array<glm::vec2, 4>& controlPoints, float t);
void drawBezierVase(Shader& shader, const glm::mat4& parent, glm::vec3 baseCenter, glm::vec3 scale);
void drawBezierVasePlant(Shader& shader, const glm::mat4& parent, glm::vec3 baseCenter, glm::vec3 scale, float yawDeg = 0.0f);
void drawPoolsideBezierVases(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale);
void drawSmallTree(Shader& shader, const glm::mat4& parent, glm::vec3 center, float scale = 1.0f);
void drawFractalBranch(Shader& shader, const glm::mat4& parent, float length, float thickness, int depth);
void drawFractalTree(Shader& shader, const glm::mat4& parent, glm::vec3 center, float scale = 1.0f);
void drawLoungeChair(Shader& shader, const glm::mat4& parent);
void drawUmbrella(Shader& shader, const glm::mat4& parent);
void drawPool(Shader& shader, const glm::mat4& parent);
void drawChair(Shader& shader, const glm::mat4& parent);
void drawMonitor(Shader& shader, const glm::mat4& parent, bool glow = true);
void drawKeyboard(Shader& shader, const glm::mat4& parent);
void drawDesk(Shader& shader, const glm::mat4& parent, bool withMonitor = true);
void drawWashroom(Shader& shader, const glm::mat4& parent);
void drawUpperSupportZone(Shader& shader);
void drawRain(Shader& shader);
float stableNoise2D(int x, int z);
void drawReceptionDesk(Shader& shader, const glm::mat4& parent);
void drawServerRack(Shader& shader, const glm::mat4& parent);
void drawBookShelf(Shader& shader, const glm::mat4& parent, float width = 4.0f, float height = 3.2f, int columns = 5);
void drawDoorPanel(Shader& shader, const glm::mat4& hinge, float angleDeg);
void drawWindowPanel(Shader& shader, const glm::mat4& hinge, float angleDeg);
void drawWoodDoorPanel(Shader& shader, const glm::mat4& hinge, float angleDeg);
void drawGlassBarrier(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale);
void drawFrontGlassWallWithDoor(Shader& shader, const glm::mat4& parent, glm::vec3 center, float width, float height,
    float doorWidth, float doorHeight, float angleDeg);
float animateAngle(float currentAngle, bool opening, float targetAngle, float speed);
void toggleNearestDoor();

void drawOutdoorScene(Shader& shader);
void drawBuildingEnvelope(Shader& shader);
void drawGroundFloorRooms(Shader& shader);
void drawUpperFloorRooms(Shader& shader);
void drawRoofPavilion(Shader& shader);
void drawCentralAtrium(Shader& shader);
void drawServerRoom(Shader& shader);
void drawLibraryRoom(Shader& shader);
void drawReceptionRoom(Shader& shader);
void drawUpperLeftClassroom(Shader& shader);
void drawUpperRightClassroom(Shader& shader);

void configureLighting(Shader& shader, const glm::vec3& viewPos, const glm::vec3& spotPos, const glm::vec3& spotDir);
void renderITPark(Shader& shader);
void renderLightCubes(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
void renderView(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& projection,
    const glm::vec3& eye, const glm::vec3& spotPos, const glm::vec3& spotDir);

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

constexpr int NR_POINT_LIGHTS = 7;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const glm::vec3 defaultCameraPos(-17.5f, 1.75f, 30.5f);
const glm::vec3 defaultLookTarget(4.5f, 4.0f, 10.5f);
BasicCamera camera(defaultCameraPos.x, defaultCameraPos.y, defaultCameraPos.z,
    defaultLookTarget.x, defaultLookTarget.y, defaultLookTarget.z,
    glm::vec3(0.0f, 1.0f, 0.0f));

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
unsigned int cubeEBO = 0;
unsigned int lightCubeVAO = 0;
Sphere* unitSphere = nullptr;

bool doorOpening = false;
bool serverDoorOpening = false;
bool libraryDoorOpening = false;
bool receptionDoorOpening = false;
bool upperLeftDoorOpening = false;
bool upperRightDoorOpening = false;
bool washroomDoorOpening = false;
bool innovationDoorOpening = false;
bool windowOpening = false;
bool masterLight = true;
bool directionalOn = true;
bool pointLightsOn = true;
bool spotLightOn = true;
bool emissiveOn = true;
bool ambientOn = true;
bool diffuseOn = true;
bool specularOn = true;
bool useGouraudShading = false;
bool rainEnabled = false;
bool pointLightEnabled[NR_POINT_LIGHTS] = { true, true, true, true, true, true, true };
glm::vec3 lastSafeCameraPos = defaultCameraPos;

float doorAngle = 0.0f;
float serverDoorAngle = 0.0f;
float libraryDoorAngle = 0.0f;
float receptionDoorAngle = 0.0f;
float upperLeftDoorAngle = 0.0f;
float upperRightDoorAngle = 0.0f;
float washroomDoorAngle = 0.0f;
float innovationDoorAngle = 0.0f;
float windowAngle = 0.0f;
float sceneClock = 0.0f;
int textureMode = 0;  // 0=no texture, 1=simple, 2=blended
unsigned int doorTexture = 0;
unsigned int grassTexture = 0;
unsigned int wallTexture = 0;

unsigned int bezierVaseVAO = 0;
unsigned int bezierVaseVBO = 0;
unsigned int bezierVaseEBO = 0;
GLsizei bezierVaseIndexCount = 0;

const glm::vec3 pointLightPositions[NR_POINT_LIGHTS] = {
    glm::vec3(10.6f, 3.2f, 4.8f),    // reception
    glm::vec3(-10.4f, 7.0f, 4.9f),   // upper left classroom
    glm::vec3(-10.8f, 3.1f, 4.8f),   // server room
    glm::vec3(-1.1f, 3.0f, 4.7f),    // library / atrium
    glm::vec3(10.6f, 7.0f, 4.8f),    // upper right classroom
    glm::vec3(5.1f, -0.06f, 18.0f),  // pool left
    glm::vec3(11.9f, -0.06f, 18.0f)  // pool right
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(static_cast<int>(SCR_WIDTH), static_cast<int>(SCR_HEIGHT), "3D IT Park Visualization", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader lampShader("vertexShader.vs", "fragmentShader.fs");

    setupCubeBuffers();
    unitSphere = new Sphere(1.0f, 24, 24);
    initBezierVaseMesh();

    // Load textures
    doorTexture = loadTexture("door.jpg");
    grassTexture = loadTexture("grass.jpg");
    wallTexture = loadTexture("wall1.jpg");
    // Bind texture unit 0 for lighting shader
    lightingShader.use();
    lightingShader.setInt("texture1", 0);

    camera.MovementSpeed = 2.4f;
    camera.RotationSpeed = 50.0f;

    printControls();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.033f) deltaTime = 0.033f;
        if (deltaTime < 0.0f) deltaTime = 0.0f;

        processInput(window);
        applyCameraWalkConstraints();
        updateAnimations();

        if (masterLight) {
            if (rainEnabled) glClearColor(0.52f, 0.61f, 0.72f, 1.0f);
            else glClearColor(0.64f, 0.79f, 0.94f, 1.0f);
        }
        else {
            glClearColor(0.05f, 0.06f, 0.10f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.createViewMatrix();
        glm::mat4 projection = camera.createProjectionMatrix(static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.25f, 180.0f);
        glViewport(0, 0, static_cast<int>(SCR_WIDTH), static_cast<int>(SCR_HEIGHT));
        renderView(lightingShader, lampShader, view, projection, camera.Position, camera.Position, camera.Front);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyBezierVaseMesh();
    delete unitSphere;
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glfwTerminate();
    return 0;
}

void printControls() {
    cout << "\n================ MODERN IT PARK CONTROLS ================\n";
    cout << "Move             : W/A/S/D or Arrow Keys\n";
    cout << "Vertical Move    : Q = Down, E = Up\n";
    cout << "Mouse Look       : Move mouse\n";
    cout << "Key Look         : J/L = Yaw Left/Right, I/K = Pitch Up/Down\n";
    cout << "Doors            : O or SPACE = toggle nearest door\n";
    cout << "Roof Window      : P\n";
    cout << "Weather          : T = toggle rain\n";
    cout << "Master Light     : 0\n";
    cout << "Light Types      : 1 = Directional, 2 = All Point Lights, 3 = Spot, 4 = Emissive\n";
    cout << "Light Components : 5 = Ambient, 6 = Diffuse, 7 = Specular\n";
    cout << "Shading          : G = toggle Phong / Gouraud\n";
    cout << "Texture Mode     : V = cycle (material / texture / blended)\n";
    cout << "Camera Info      : M = print camera state\n";
    cout << "Reset Camera     : HOME\n";
    cout << "Reprint Controls : F1\n";
    cout << "Exit             : ESC\n";
    cout << "========================================================\n\n";
}

void printCameraState() {
    cout << "[Camera] Position = (" << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << ")"
        << " | Front = (" << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << ")"
        << " | Yaw = " << camera.Yaw
        << " | Pitch = " << camera.Pitch
        << endl;
}

void setupCubeBuffers() {
    // pos(3) + normal(3) + uv(2) = 8 floats per vertex
    float cubeVertices[] = {
        // Front face (-Z)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // Right face (+X)
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         // Back face (+Z)
         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         // Left face (-X)
         -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         // Top face (+Y)
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         // Bottom face (-Y)
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int cubeIndices[] = {
        0, 3, 2, 2, 1, 0,
        4, 5, 7, 7, 6, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

glm::mat4 makeModel(const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale) {
    glm::mat4 model = glm::translate(parent, pos);
    model = glm::rotate(model, glm::radians(rotDeg.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotDeg.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotDeg.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void setMaterial(Shader& shader, glm::vec3 color, glm::vec3 specular, float shininess, glm::vec3 emissive) {
    shader.use();
    shader.setVec3("material.ambient", color * 0.22f);
    shader.setVec3("material.diffuse", color);
    shader.setVec3("material.specular", specular);
    shader.setVec3("material.emissive", emissiveOn ? emissive : glm::vec3(0.0f));
    shader.setFloat("material.shininess", shininess);
}

void drawCubeShape(Shader& shader, const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale,
    glm::vec3 color, glm::vec3 specular, float shininess, glm::vec3 emissive) {
    glm::mat4 model = makeModel(parent, pos, rotDeg, scale);
    setMaterial(shader, color, specular, shininess, emissive);
    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

unsigned int loadTexture(const char* path) {
    unsigned int texID;
    glGenTextures(1, &texID);
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &w, &h, &ch, 0);
    if (data) {
        GLenum fmt = (ch == 4) ? GL_RGBA : (ch == 1 ? GL_RED : GL_RGB);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Loaded: " << path << " (" << w << "x" << h << ")" << std::endl;
    }
    else {
        std::cout << "FAILED to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return texID;
}

void drawCubeShapeTextured(Shader& shader, const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale,
    glm::vec3 color, unsigned int texID, glm::vec3 specular, float shininess, glm::vec3 emissive) {
    glm::mat4 model = makeModel(parent, pos, rotDeg, scale);
    setMaterial(shader, color, specular, shininess, emissive);
    shader.setInt("useTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader.setInt("useTexture", 0);
}

void drawSphereShape(Shader& shader, const glm::mat4& parent, glm::vec3 pos, glm::vec3 rotDeg, glm::vec3 scale,
    glm::vec3 color, glm::vec3 specular, float shininess, glm::vec3 emissive) {
    if (!unitSphere) return;
    glm::mat4 model = makeModel(parent, pos, rotDeg, scale);
    unitSphere->ambient = color * 0.22f;
    unitSphere->diffuse = color;
    unitSphere->specular = specular;
    unitSphere->emissive = emissiveOn ? emissive : glm::vec3(0.0f);
    unitSphere->shininess = shininess;
    unitSphere->drawSphere(shader, model);
}

void drawFrontOpeningFrame(Shader& shader, const glm::mat4& parent, glm::vec3 center, float width, float height,
    glm::vec3 frameColor, int mullions, glm::vec3 glow) {
    float t = 0.06f;
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, height * 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(width, t, 0.08f), frameColor, glm::vec3(0.25f), 18.0f, glow);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, -height * 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(width, t, 0.08f), frameColor, glm::vec3(0.25f), 18.0f, glow * 0.4f);
    drawCubeShape(shader, parent, center + glm::vec3(-width * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(t, height, 0.08f), frameColor, glm::vec3(0.25f), 18.0f, glow * 0.4f);
    drawCubeShape(shader, parent, center + glm::vec3(width * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(t, height, 0.08f), frameColor, glm::vec3(0.25f), 18.0f, glow * 0.4f);

    int divs = std::max(2, mullions);
    for (int i = 1; i < divs; ++i) {
        float x = -width * 0.5f + width * (static_cast<float>(i) / divs);
        drawCubeShape(shader, parent, center + glm::vec3(x, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.04f, height, 0.05f), frameColor, glm::vec3(0.22f), 14.0f, glow * 0.25f);
    }
}

void drawSideGlassWall(Shader& shader, const glm::mat4& parent, glm::vec3 center, float depth, float height, glm::vec3 glassColor) {
    glm::vec3 frame(0.78f, 0.80f, 0.84f);
    glm::vec3 glow(0.02f, 0.03f, 0.05f);
    drawCubeShape(shader, parent, center, glm::vec3(0.0f), glm::vec3(0.05f, height, depth), glassColor, glm::vec3(0.45f), 44.0f, glow);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, height * 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(0.08f, 0.05f, depth + 0.08f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, -height * 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(0.08f, 0.05f, depth + 0.08f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, 0.0f, -depth * 0.5f), glm::vec3(0.0f), glm::vec3(0.08f, height + 0.08f, 0.05f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, 0.0f, depth * 0.5f), glm::vec3(0.0f), glm::vec3(0.08f, height + 0.08f, 0.05f), frame, glm::vec3(0.28f), 20.0f);

    int divs = std::max(2, static_cast<int>(depth / 2.0f));
    for (int i = 1; i < divs; ++i) {
        float z = -depth * 0.5f + depth * (static_cast<float>(i) / divs);
        drawCubeShape(shader, parent, center + glm::vec3(0.0f, 0.0f, z), glm::vec3(0.0f), glm::vec3(0.04f, height, 0.04f), frame, glm::vec3(0.24f), 16.0f);
    }
}

void drawWoodAccentPanel(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale) {
    glm::vec3 wood(0.44f, 0.30f, 0.18f);
    glm::vec3 dark(0.30f, 0.20f, 0.12f);
    drawCubeShape(shader, parent, center, glm::vec3(0.0f), scale, wood, glm::vec3(0.14f), 12.0f);

    int slats = std::max(3, static_cast<int>((scale.x > scale.z ? scale.x : scale.z) / 0.5f));
    if (scale.z < scale.x) {
        for (int i = 0; i < slats; ++i) {
            float x = -scale.x * 0.5f + (i + 0.5f) * (scale.x / slats);
            drawCubeShape(shader, parent, center + glm::vec3(x, 0.0f, scale.z * 0.48f), glm::vec3(0.0f), glm::vec3(0.04f, scale.y, 0.02f), dark, glm::vec3(0.08f), 8.0f);
        }
    }
    else {
        for (int i = 0; i < slats; ++i) {
            float z = -scale.z * 0.5f + (i + 0.5f) * (scale.z / slats);
            drawCubeShape(shader, parent, center + glm::vec3(scale.x * 0.48f, 0.0f, z), glm::vec3(0.0f), glm::vec3(0.02f, scale.y, 0.04f), dark, glm::vec3(0.08f), 8.0f);
        }
    }
}

void drawSignPanel(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale, bool twoLines) {
    glm::vec3 board(0.22f, 0.24f, 0.28f);
    glm::vec3 text(0.92f, 0.94f, 0.97f);
    drawCubeShape(shader, parent, center, glm::vec3(0.0f), scale, board, glm::vec3(0.18f), 16.0f, glm::vec3(0.02f));
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, twoLines ? 0.10f : 0.0f, scale.z * 0.52f), glm::vec3(0.0f), glm::vec3(scale.x * 0.72f, 0.08f, 0.01f), text, glm::vec3(0.18f), 12.0f, glm::vec3(0.03f));
    if (twoLines) {
        drawCubeShape(shader, parent, center + glm::vec3(0.0f, -0.10f, scale.z * 0.52f), glm::vec3(0.0f), glm::vec3(scale.x * 0.62f, 0.08f, 0.01f), text, glm::vec3(0.18f), 12.0f, glm::vec3(0.03f));
    }
}

void drawPlanterStrip(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale, glm::vec3 hedgeColor) {
    glm::vec3 concrete(0.76f, 0.77f, 0.80f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, scale.y * 0.25f, 0.0f), glm::vec3(0.0f), glm::vec3(scale.x, scale.y * 0.50f, scale.z), concrete, glm::vec3(0.20f), 16.0f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, scale.y * 0.65f, 0.0f), glm::vec3(0.0f), glm::vec3(scale.x * 0.90f, scale.y * 0.42f, scale.z * 0.85f), hedgeColor, glm::vec3(0.12f), 10.0f);

    int lumps = std::max(2, static_cast<int>(scale.x / 1.1f));
    for (int i = 0; i < lumps; ++i) {
        float x = -scale.x * 0.4f + (static_cast<float>(i) / std::max(1, lumps - 1)) * scale.x * 0.8f;
        drawSphereShape(shader, parent, center + glm::vec3(x, scale.y * 0.96f, 0.0f), glm::vec3(0.0f), glm::vec3(0.23f, 0.18f, 0.23f), hedgeColor * 1.05f, glm::vec3(0.08f), 12.0f);
    }
}

void drawPottedPlant(Shader& shader, const glm::mat4& parent, glm::vec3 center, float scale) {
    glm::vec3 pot(0.52f, 0.34f, 0.18f);
    glm::vec3 leafA(0.22f, 0.58f, 0.24f);
    glm::vec3 leafB(0.18f, 0.50f, 0.22f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, 0.18f * scale, 0.0f), glm::vec3(0.0f), glm::vec3(0.25f, 0.22f, 0.25f) * scale, pot, glm::vec3(0.10f), 8.0f);
    drawSphereShape(shader, parent, center + glm::vec3(0.0f, 0.58f * scale, 0.0f), glm::vec3(0.0f), glm::vec3(0.30f) * scale, leafA, glm::vec3(0.12f), 12.0f);
    drawSphereShape(shader, parent, center + glm::vec3(-0.12f * scale, 0.78f * scale, 0.08f * scale), glm::vec3(0.0f), glm::vec3(0.20f) * scale, leafB, glm::vec3(0.12f), 12.0f);
    drawSphereShape(shader, parent, center + glm::vec3(0.12f * scale, 0.72f * scale, -0.10f * scale), glm::vec3(0.0f), glm::vec3(0.22f) * scale, leafA, glm::vec3(0.12f), 12.0f);
}

glm::vec2 cubicBezier2D(const std::array<glm::vec2, 4>& controlPoints, float t) {
    float u = 1.0f - t;
    return u * u * u * controlPoints[0]
        + 3.0f * u * u * t * controlPoints[1]
        + 3.0f * u * t * t * controlPoints[2]
        + t * t * t * controlPoints[3];
}

glm::vec2 cubicBezierTangent2D(const std::array<glm::vec2, 4>& controlPoints, float t) {
    float u = 1.0f - t;
    return 3.0f * u * u * (controlPoints[1] - controlPoints[0])
        + 6.0f * u * t * (controlPoints[2] - controlPoints[1])
        + 3.0f * t * t * (controlPoints[3] - controlPoints[2]);
}

void initBezierVaseMesh() {
    if (bezierVaseVAO != 0) return;

    struct BezierVaseVertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    const std::array<glm::vec2, 4> controlPoints = {
        glm::vec2(0.18f, 0.00f),
        glm::vec2(0.78f, 0.18f),
        glm::vec2(0.86f, 0.96f),
        glm::vec2(0.30f, 1.48f)
    };

    const int profileSteps = 30;
    const int slices = 40;
    const float twoPi = 6.28318530718f;

    std::vector<BezierVaseVertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve((profileSteps + 1) * (slices + 1));
    indices.reserve(profileSteps * slices * 6);

    for (int i = 0; i <= profileSteps; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(profileSteps);
        glm::vec2 profile = cubicBezier2D(controlPoints, t);
        glm::vec2 tangent = cubicBezierTangent2D(controlPoints, t);

        float radius = std::max(0.06f, profile.x);
        float y = profile.y;

        glm::vec2 normal2D(tangent.y, -tangent.x);
        if (glm::length(normal2D) < 0.0001f) {
            normal2D = glm::vec2(1.0f, 0.0f);
        }
        else {
            normal2D = glm::normalize(normal2D);
        }

        for (int s = 0; s <= slices; ++s) {
            float theta = twoPi * static_cast<float>(s) / static_cast<float>(slices);
            float cs = std::cos(theta);
            float sn = std::sin(theta);

            glm::vec3 pos(radius * cs, y, -radius * sn);
            glm::vec3 normal = glm::normalize(glm::vec3(normal2D.x * cs, normal2D.y, -normal2D.x * sn));
            glm::vec2 uv(static_cast<float>(s) / static_cast<float>(slices), t);

            vertices.push_back({ pos, normal, uv });
        }
    }

    const int ring = slices + 1;
    for (int i = 0; i < profileSteps; ++i) {
        for (int s = 0; s < slices; ++s) {
            unsigned int a = static_cast<unsigned int>(i * ring + s);
            unsigned int b = static_cast<unsigned int>((i + 1) * ring + s);
            unsigned int c = b + 1;
            unsigned int d = a + 1;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    bezierVaseIndexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &bezierVaseVAO);
    glGenBuffers(1, &bezierVaseVBO);
    glGenBuffers(1, &bezierVaseEBO);

    glBindVertexArray(bezierVaseVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bezierVaseVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(BezierVaseVertex)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierVaseEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BezierVaseVertex), reinterpret_cast<void*>(offsetof(BezierVaseVertex, pos)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BezierVaseVertex), reinterpret_cast<void*>(offsetof(BezierVaseVertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BezierVaseVertex), reinterpret_cast<void*>(offsetof(BezierVaseVertex, uv)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void destroyBezierVaseMesh() {
    if (bezierVaseEBO) glDeleteBuffers(1, &bezierVaseEBO);
    if (bezierVaseVBO) glDeleteBuffers(1, &bezierVaseVBO);
    if (bezierVaseVAO) glDeleteVertexArrays(1, &bezierVaseVAO);
    bezierVaseVAO = bezierVaseVBO = bezierVaseEBO = 0;
    bezierVaseIndexCount = 0;
}

void drawBezierVase(Shader& shader, const glm::mat4& parent, glm::vec3 baseCenter, glm::vec3 scale) {
    if (bezierVaseVAO == 0 || bezierVaseIndexCount == 0) return;

    glm::mat4 model = makeModel(parent, baseCenter, glm::vec3(0.0f), scale);
    glm::vec3 vaseColor(0.72f, 0.88f, 0.86f);
    glm::vec3 vaseSpec(0.88f, 0.93f, 0.95f);
    glm::vec3 vaseGlow(0.02f, 0.04f, 0.05f);
    setMaterial(shader, vaseColor, vaseSpec, 96.0f, vaseGlow);
    shader.setMat4("model", model);

    glBindVertexArray(bezierVaseVAO);
    glDrawElements(GL_TRIANGLES, bezierVaseIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    float topY = 1.45f * scale.y;
    float lipRadius = 0.30f * scale.x;
    drawSphereShape(shader, parent, baseCenter + glm::vec3(0.0f, topY, 0.0f), glm::vec3(0.0f),
        glm::vec3(lipRadius * 1.06f, 0.018f * scale.y, lipRadius * 1.06f),
        glm::vec3(0.78f, 0.93f, 0.90f), glm::vec3(0.92f), 96.0f, glm::vec3(0.02f, 0.04f, 0.05f));
    drawSphereShape(shader, parent, baseCenter + glm::vec3(0.0f, topY - 0.012f * scale.y, 0.0f), glm::vec3(0.0f),
        glm::vec3(lipRadius * 0.80f, 0.012f * scale.y, lipRadius * 0.80f),
        glm::vec3(0.10f, 0.13f, 0.12f), glm::vec3(0.22f), 20.0f);

    float bottomY = 0.028f * scale.y;
    float bottomRadius = 0.115f * scale.x;
    drawSphereShape(shader, parent, baseCenter + glm::vec3(0.0f, bottomY, 0.0f), glm::vec3(0.0f),
        glm::vec3(bottomRadius, 0.020f * scale.y, bottomRadius),
        glm::vec3(0.76f, 0.90f, 0.88f), glm::vec3(0.70f), 64.0f, glm::vec3(0.01f, 0.02f, 0.03f));
}

void drawBezierVasePlant(Shader& shader, const glm::mat4& parent, glm::vec3 baseCenter, glm::vec3 scale, float yawDeg) {
    drawBezierVase(shader, parent, baseCenter, scale);

    float soilY = 1.34f * scale.y;
    drawSphereShape(shader, parent, baseCenter + glm::vec3(0.0f, soilY, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.20f * scale.x, 0.032f * scale.y, 0.20f * scale.z),
        glm::vec3(0.30f, 0.22f, 0.15f), glm::vec3(0.10f), 8.0f);

    glm::mat4 plantBase = makeModel(parent, baseCenter + glm::vec3(0.0f, soilY + 0.012f * scale.y, 0.0f),
        glm::vec3(0.0f, yawDeg, 0.0f), glm::vec3(1.0f));

    glm::vec3 stemColor(0.34f, 0.24f, 0.16f);

    drawCubeShape(shader, plantBase, glm::vec3(0.0f, 0.20f * scale.y, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.044f * scale.x, 0.48f * scale.y, 0.044f * scale.z), stemColor, glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, plantBase, glm::vec3(-0.078f * scale.x, 0.39f * scale.y, 0.0f), glm::vec3(0.0f, 0.0f, 34.0f),
        glm::vec3(0.026f * scale.x, 0.27f * scale.y, 0.026f * scale.z), stemColor, glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, plantBase, glm::vec3(0.078f * scale.x, 0.38f * scale.y, 0.0f), glm::vec3(0.0f, 0.0f, -32.0f),
        glm::vec3(0.026f * scale.x, 0.27f * scale.y, 0.026f * scale.z), stemColor, glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, plantBase, glm::vec3(0.0f, 0.45f * scale.y, -0.062f * scale.z), glm::vec3(26.0f, 0.0f, 0.0f),
        glm::vec3(0.024f * scale.x, 0.24f * scale.y, 0.024f * scale.z), stemColor, glm::vec3(0.10f), 8.0f);

    drawSphereShape(shader, plantBase, glm::vec3(-0.155f * scale.x, 0.57f * scale.y, 0.01f * scale.z), glm::vec3(0.0f),
        glm::vec3(0.108f * scale.x, 0.100f * scale.y, 0.108f * scale.z), glm::vec3(0.95f, 0.70f, 0.78f), glm::vec3(0.20f), 16.0f);
    drawSphereShape(shader, plantBase, glm::vec3(0.155f * scale.x, 0.56f * scale.y, -0.01f * scale.z), glm::vec3(0.0f),
        glm::vec3(0.108f * scale.x, 0.100f * scale.y, 0.108f * scale.z), glm::vec3(0.98f, 0.90f, 0.78f), glm::vec3(0.20f), 16.0f);
    drawSphereShape(shader, plantBase, glm::vec3(0.0f, 0.65f * scale.y, -0.102f * scale.z), glm::vec3(0.0f),
        glm::vec3(0.102f * scale.x, 0.094f * scale.y, 0.102f * scale.z), glm::vec3(0.96f, 0.82f, 0.88f), glm::vec3(0.20f), 16.0f);
    drawSphereShape(shader, plantBase, glm::vec3(0.0f, 0.50f * scale.y, 0.118f * scale.z), glm::vec3(0.0f),
        glm::vec3(0.098f * scale.x, 0.090f * scale.y, 0.098f * scale.z), glm::vec3(0.90f, 0.76f, 0.94f), glm::vec3(0.20f), 16.0f);
    drawSphereShape(shader, plantBase, glm::vec3(0.0f, 0.51f * scale.y, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.072f * scale.x, 0.066f * scale.y, 0.072f * scale.z), glm::vec3(0.30f, 0.66f, 0.28f), glm::vec3(0.14f), 12.0f);
}
void drawPoolsideBezierVases(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale) {
    glm::vec3 stone(0.79f, 0.80f, 0.83f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, scale.y * 0.10f, 0.0f), glm::vec3(0.0f),
        glm::vec3(scale.x * 0.92f, scale.y * 0.16f, scale.z * 0.70f), stone, glm::vec3(0.20f), 16.0f);

    const float offsets[4] = { -1.55f, -0.52f, 0.52f, 1.55f };
    const float yaws[4] = { 14.0f, -10.0f, 10.0f, -14.0f };
    const glm::vec3 vaseScales[4] = {
      glm::vec3(0.24f, 0.29f, 0.24f),
      glm::vec3(0.23f, 0.28f, 0.23f),
      glm::vec3(0.24f, 0.29f, 0.24f),
      glm::vec3(0.25f, 0.30f, 0.25f)
    };

    for (int i = 0; i < 4; ++i) {
        glm::vec3 vaseBase = center + glm::vec3(offsets[i], scale.y * 0.19f + 0.003f, 0.0f);
        drawBezierVasePlant(shader, parent, vaseBase, vaseScales[i], yaws[i]);
    }
}

void drawSmallTree(Shader& shader, const glm::mat4& parent, glm::vec3 center, float scale) {
    glm::vec3 bark(0.40f, 0.26f, 0.15f);
    glm::vec3 leafA(0.24f, 0.58f, 0.23f);
    glm::vec3 leafB(0.20f, 0.52f, 0.20f);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, 1.15f * scale, 0.0f), glm::vec3(0.0f), glm::vec3(0.40f, 2.30f, 0.40f) * scale, bark, glm::vec3(0.08f), 8.0f);
    drawSphereShape(shader, parent, center + glm::vec3(0.0f, 3.00f * scale, 0.0f), glm::vec3(0.0f), glm::vec3(1.00f) * scale, leafA, glm::vec3(0.15f), 18.0f);
    drawSphereShape(shader, parent, center + glm::vec3(-0.55f * scale, 2.75f * scale, 0.22f * scale), glm::vec3(0.0f), glm::vec3(0.72f) * scale, leafB, glm::vec3(0.15f), 18.0f);
    drawSphereShape(shader, parent, center + glm::vec3(0.56f * scale, 2.72f * scale, -0.18f * scale), glm::vec3(0.0f), glm::vec3(0.72f) * scale, leafB, glm::vec3(0.15f), 18.0f);
}

void drawFractalBranch(Shader& shader, const glm::mat4& parent, float length, float thickness, int depth) {
    glm::vec3 bark(0.36f, 0.24f, 0.14f);
    glm::vec3 leaf(0.20f, 0.52f, 0.20f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, length * 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(thickness, length, thickness), bark, glm::vec3(0.07f), 8.0f);

    glm::mat4 top = makeModel(parent, glm::vec3(0.0f, length, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    if (depth <= 0) {
        drawSphereShape(shader, top, glm::vec3(0.0f, 0.18f, 0.0f), glm::vec3(0.0f), glm::vec3(0.22f), leaf, glm::vec3(0.12f), 16.0f);
        return;
    }

    drawFractalBranch(shader, makeModel(top, glm::vec3(0.0f), glm::vec3(20.0f, 0.0f, 28.0f), glm::vec3(1.0f)), length * 0.72f, thickness * 0.72f, depth - 1);
    drawFractalBranch(shader, makeModel(top, glm::vec3(0.0f), glm::vec3(18.0f, 0.0f, -28.0f), glm::vec3(1.0f)), length * 0.72f, thickness * 0.72f, depth - 1);
    drawFractalBranch(shader, makeModel(top, glm::vec3(0.0f), glm::vec3(28.0f, 20.0f, 0.0f), glm::vec3(1.0f)), length * 0.60f, thickness * 0.62f, depth - 1);
}

void drawFractalTree(Shader& shader, const glm::mat4& parent, glm::vec3 center, float scale) {
    glm::mat4 treeBase = makeModel(parent, center, glm::vec3(0.0f), glm::vec3(scale));
    drawFractalBranch(shader, treeBase, 3.4f, 0.34f, 4);
}

void drawLoungeChair(Shader& shader, const glm::mat4& parent) {
    glm::vec3 cushion(0.93f, 0.93f, 0.91f);
    glm::vec3 frame(0.55f, 0.38f, 0.21f);
    glm::vec3 rail(0.36f, 0.25f, 0.16f);

    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.26f, 0.04f), glm::vec3(0.0f),
        glm::vec3(1.70f, 0.08f, 0.76f), cushion, glm::vec3(0.20f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.60f, -0.42f), glm::vec3(-28.0f, 0.0f, 0.0f),
        glm::vec3(1.70f, 0.08f, 0.72f), cushion, glm::vec3(0.20f), 18.0f);

    for (int i = 0; i < 4; ++i) {
        float sx = (i < 2) ? -0.68f : 0.68f;
        float sz = (i % 2 == 0) ? -0.25f : 0.25f;
        drawCubeShape(shader, parent, glm::vec3(sx, 0.13f, sz), glm::vec3(0.0f),
            glm::vec3(0.08f, 0.26f, 0.08f), frame, glm::vec3(0.10f), 8.0f);
    }

    drawCubeShape(shader, parent, glm::vec3(-0.77f, 0.48f, -0.16f), glm::vec3(0.0f),
        glm::vec3(0.05f, 0.62f, 0.05f), rail, glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, parent, glm::vec3(0.77f, 0.48f, -0.16f), glm::vec3(0.0f),
        glm::vec3(0.05f, 0.62f, 0.05f), rail, glm::vec3(0.10f), 8.0f);
}

void drawUmbrella(Shader& shader, const glm::mat4& parent) {
    glm::vec3 pole(0.76f, 0.76f, 0.78f);
    glm::vec3 canopy(0.97f, 0.97f, 0.96f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 1.62f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.08f, 3.20f, 0.08f), pole, glm::vec3(0.28f), 22.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 3.18f, 0.0f), glm::vec3(0.0f, 45.0f, 0.0f),
        glm::vec3(1.90f, 0.05f, 1.90f), canopy, glm::vec3(0.22f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 3.14f, 0.0f), glm::vec3(0.0f),
        glm::vec3(1.55f, 0.05f, 1.55f), canopy, glm::vec3(0.20f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 3.25f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.28f, 0.14f, 0.28f), canopy, glm::vec3(0.18f), 14.0f);
}

void drawPool(Shader& shader, const glm::mat4& parent) {
    glm::vec3 deck(0.58f, 0.42f, 0.25f);
    glm::vec3 deckLight(0.64f, 0.47f, 0.28f);
    glm::vec3 coping(0.86f, 0.87f, 0.90f);
    glm::vec3 basin(0.22f, 0.34f, 0.45f);
    glm::vec3 water(0.20f, 0.72f, 0.94f);
    glm::vec3 metal(0.78f, 0.79f, 0.82f);

    const float deckW = 14.2f;
    const float deckD = 9.2f;
    const float poolW = 10.0f;
    const float poolD = 6.1f;

    drawCubeShape(shader, parent, glm::vec3(0.0f, -0.02f, 0.0f), glm::vec3(0.0f),
        glm::vec3(deckW, 0.06f, deckD), deck, glm::vec3(0.12f), 10.0f);

    for (int i = 0; i < 28; ++i) {
        float x = -deckW * 0.48f + i * (deckW * 0.96f / 27.0f);
        drawCubeShape(shader, parent, glm::vec3(x, 0.005f, 0.0f), glm::vec3(0.0f),
            glm::vec3(0.08f, 0.004f, deckD * 0.97f),
            (i % 2 == 0) ? deckLight : deck, glm::vec3(0.10f), 8.0f);
    }

    drawCubeShape(shader, parent, glm::vec3(0.0f, -0.74f, 0.0f), glm::vec3(0.0f),
        glm::vec3(poolW, 1.48f, poolD), basin, glm::vec3(0.16f), 12.0f);

    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.05f, -poolD * 0.5f - 0.18f), glm::vec3(0.0f),
        glm::vec3(poolW + 0.42f, 0.10f, 0.28f), coping, glm::vec3(0.26f), 22.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.05f, poolD * 0.5f + 0.18f), glm::vec3(0.0f),
        glm::vec3(poolW + 0.42f, 0.10f, 0.28f), coping, glm::vec3(0.26f), 22.0f);
    drawCubeShape(shader, parent, glm::vec3(-poolW * 0.5f - 0.18f, 0.05f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.28f, 0.10f, poolD + 0.42f), coping, glm::vec3(0.26f), 22.0f);
    drawCubeShape(shader, parent, glm::vec3(poolW * 0.5f + 0.18f, 0.05f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.28f, 0.10f, poolD + 0.42f), coping, glm::vec3(0.26f), 22.0f);

    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.015f, 0.0f), glm::vec3(0.0f),
        glm::vec3(poolW - 0.18f, 0.025f, poolD - 0.18f),
        water, glm::vec3(0.68f), 82.0f, glm::vec3(0.03f, 0.08f, 0.12f));

  /*  for (int i = 0; i < 6; ++i) {
        float z = -2.0f + i * 0.80f;
        drawCubeShape(shader, parent, glm::vec3(0.0f, 0.028f, z), glm::vec3(0.0f),
            glm::vec3(poolW * 0.85f, 0.004f, 0.035f),
            glm::vec3(0.48f, 0.88f, 1.0f), glm::vec3(0.20f), 14.0f,
            glm::vec3(0.02f, 0.05f, 0.08f));
    }*/

    drawCubeShape(shader, parent, glm::vec3(-3.95f, 0.44f, -2.75f), glm::vec3(0.0f),
        glm::vec3(0.06f, 0.86f, 0.06f), metal, glm::vec3(0.34f), 28.0f);
    drawCubeShape(shader, parent, glm::vec3(-3.55f, 0.44f, -2.75f), glm::vec3(0.0f),
        glm::vec3(0.06f, 0.86f, 0.06f), metal, glm::vec3(0.34f), 28.0f);
    for (int i = 0; i < 3; ++i) {
        drawCubeShape(shader, parent, glm::vec3(-3.75f, 0.12f + i * 0.25f, -2.75f), glm::vec3(0.0f),
            glm::vec3(0.40f, 0.04f, 0.06f), metal, glm::vec3(0.34f), 28.0f);
    }

    drawLoungeChair(shader, makeModel(parent, glm::vec3(-4.8f, 0.02f, 3.55f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(0.92f)));
    drawLoungeChair(shader, makeModel(parent, glm::vec3(-0.5f, 0.02f, 3.75f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(0.92f)));
    drawLoungeChair(shader, makeModel(parent, glm::vec3(3.8f, 0.02f, 3.55f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(0.92f)));
    drawLoungeChair(shader, makeModel(parent, glm::vec3(6.05f, 0.02f, -1.95f), glm::vec3(0.0f, 95.0f, 0.0f), glm::vec3(0.88f)));

    drawUmbrella(shader, makeModel(parent, glm::vec3(-4.9f, 0.0f, 2.65f), glm::vec3(0.0f), glm::vec3(1.0f)));
    drawUmbrella(shader, makeModel(parent, glm::vec3(-0.35f, 0.0f, 2.75f), glm::vec3(0.0f), glm::vec3(1.0f)));
    drawUmbrella(shader, makeModel(parent, glm::vec3(4.1f, 0.0f, 2.65f), glm::vec3(0.0f), glm::vec3(1.0f)));
    drawUmbrella(shader, makeModel(parent, glm::vec3(5.85f, 0.0f, -3.05f), glm::vec3(0.0f), glm::vec3(0.95f)));
}

void drawChair(Shader& shader, const glm::mat4& parent) {
    glm::vec3 seat(0.30f, 0.34f, 0.40f);
    glm::vec3 metal(0.58f, 0.60f, 0.64f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.48f, 0.0f), glm::vec3(0.0f), glm::vec3(0.80f, 0.12f, 0.80f), seat, glm::vec3(0.20f), 14.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.98f, 0.28f), glm::vec3(-8.0f, 0.0f, 0.0f), glm::vec3(0.80f, 0.90f, 0.10f), seat, glm::vec3(0.20f), 14.0f);
    float legX[4] = { -0.30f, 0.30f, -0.30f, 0.30f };
    float legZ[4] = { -0.30f, -0.30f, 0.30f, 0.30f };
    for (int i = 0; i < 4; ++i) {
        drawCubeShape(shader, parent, glm::vec3(legX[i], 0.22f, legZ[i]), glm::vec3(0.0f), glm::vec3(0.08f, 0.44f, 0.08f), metal, glm::vec3(0.25f), 18.0f);
    }
}

void drawMonitor(Shader& shader, const glm::mat4& parent, bool glow) {
    glm::vec3 frame(0.08f, 0.09f, 0.10f);
    glm::vec3 screen(0.10f, 0.38f, 0.78f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.18f, 0.0f), glm::vec3(0.0f), glm::vec3(0.10f, 0.36f, 0.10f), glm::vec3(0.25f), glm::vec3(0.28f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.06f, 0.0f), glm::vec3(0.0f), glm::vec3(0.46f, 0.04f, 0.34f), glm::vec3(0.18f), glm::vec3(0.20f), 12.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.60f, 0.0f), glm::vec3(0.0f), glm::vec3(0.94f, 0.58f, 0.06f), frame, glm::vec3(0.22f), 16.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.60f, 0.035f), glm::vec3(0.0f), glm::vec3(0.78f, 0.44f, 0.01f), screen, glm::vec3(0.32f), 44.0f, glow ? screen * 0.55f : glm::vec3(0.0f));
}

void drawKeyboard(Shader& shader, const glm::mat4& parent) {
    glm::vec3 body(0.10f, 0.11f, 0.13f);
    glm::vec3 key(0.88f, 0.89f, 0.91f);
    glm::vec3 accent(0.18f, 0.50f, 0.86f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.02f, 0.0f), glm::vec3(-8.0f, 0.0f, 0.0f), glm::vec3(0.64f, 0.04f, 0.24f), body, glm::vec3(0.18f), 12.0f);
    for (int row = 0; row < 3; ++row) {
        float z = -0.06f + row * 0.06f;
        for (int col = 0; col < 7; ++col) {
            float x = -0.25f + col * 0.083f;
            drawCubeShape(shader, parent, glm::vec3(x, 0.045f, z), glm::vec3(0.0f), glm::vec3(0.06f, 0.01f, 0.035f),
                row == 0 ? accent : key, glm::vec3(0.14f), 10.0f, row == 0 ? accent * 0.15f : glm::vec3(0.0f));
        }
    }
    drawCubeShape(shader, parent, glm::vec3(0.43f, 0.03f, 0.02f), glm::vec3(0.0f), glm::vec3(0.10f, 0.03f, 0.14f), body, glm::vec3(0.14f), 10.0f);
    drawCubeShape(shader, parent, glm::vec3(0.43f, 0.045f, 0.04f), glm::vec3(0.0f), glm::vec3(0.04f, 0.01f, 0.04f), accent, glm::vec3(0.18f), 12.0f, accent * 0.15f);
}

void drawDesk(Shader& shader, const glm::mat4& parent, bool withMonitor) {
    glm::vec3 wood(0.68f, 0.45f, 0.22f);
    glm::vec3 darkWood(0.48f, 0.30f, 0.16f);
    glm::vec3 cpu(0.10f, 0.11f, 0.13f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.76f, 0.0f), glm::vec3(0.0f), glm::vec3(1.82f, 0.12f, 0.92f), wood, glm::vec3(0.15f), 12.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.56f, -0.32f), glm::vec3(0.0f), glm::vec3(1.42f, 0.28f, 0.08f), darkWood, glm::vec3(0.10f), 8.0f);
    float legX[4] = { -0.76f, 0.76f, -0.76f, 0.76f };
    float legZ[4] = { -0.32f, -0.32f, 0.32f, 0.32f };
    for (int i = 0; i < 4; ++i) {
        drawCubeShape(shader, parent, glm::vec3(legX[i], 0.36f, legZ[i]), glm::vec3(0.0f), glm::vec3(0.10f, 0.72f, 0.10f), darkWood, glm::vec3(0.10f), 8.0f);
    }
    drawCubeShape(shader, parent, glm::vec3(-0.58f, 0.34f, -0.10f), glm::vec3(0.0f), glm::vec3(0.22f, 0.56f, 0.40f), cpu, glm::vec3(0.18f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(-0.58f, 0.48f, 0.11f), glm::vec3(0.0f), glm::vec3(0.16f, 0.08f, 0.02f), glm::vec3(0.12f, 0.44f, 0.82f), glm::vec3(0.18f), 20.0f, glm::vec3(0.02f, 0.05f, 0.10f));
    if (withMonitor) {
        drawMonitor(shader, makeModel(parent, glm::vec3(0.10f, 0.88f, -0.12f), glm::vec3(0.0f), glm::vec3(0.84f)), true);
        drawKeyboard(shader, makeModel(parent, glm::vec3(0.05f, 0.84f, 0.18f), glm::vec3(0.0f), glm::vec3(1.0f)));
    }
    else {
        drawKeyboard(shader, makeModel(parent, glm::vec3(0.00f, 0.84f, 0.08f), glm::vec3(0.0f), glm::vec3(0.85f)));
    }
    drawChair(shader, makeModel(parent, glm::vec3(0.0f, 0.0f, 1.05f), glm::vec3(0.0f), glm::vec3(0.92f)));
}

void drawReceptionDesk(Shader& shader, const glm::mat4& parent) {
    glm::vec3 whiteStone(0.90f, 0.91f, 0.94f);
    glm::vec3 warmWood(0.62f, 0.40f, 0.22f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.92f, 0.0f), glm::vec3(0.0f), glm::vec3(4.10f, 0.16f, 1.20f), whiteStone, glm::vec3(0.40f), 36.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.48f, -0.15f), glm::vec3(0.0f), glm::vec3(3.80f, 0.92f, 0.70f), warmWood, glm::vec3(0.16f), 14.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.52f, 0.40f), glm::vec3(0.0f), glm::vec3(1.70f, 0.68f, 0.28f), whiteStone, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.22f, 0.51f), glm::vec3(0.0f), glm::vec3(1.20f, 0.04f, 0.04f), glm::vec3(0.18f, 0.70f, 0.86f), glm::vec3(0.20f), 24.0f, glm::vec3(0.04f, 0.10f, 0.12f));
    drawMonitor(shader, makeModel(parent, glm::vec3(-0.90f, 1.02f, 0.00f), glm::vec3(0.0f), glm::vec3(0.74f)), true);
    drawMonitor(shader, makeModel(parent, glm::vec3(0.90f, 1.02f, 0.00f), glm::vec3(0.0f), glm::vec3(0.74f)), true);
}

void drawServerRack(Shader& shader, const glm::mat4& parent) {
    glm::vec3 rack(0.12f, 0.13f, 0.16f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 1.30f, 0.0f), glm::vec3(0.0f), glm::vec3(1.05f, 2.60f, 1.05f), rack, glm::vec3(0.25f), 26.0f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, 1.30f, 0.54f), glm::vec3(0.0f), glm::vec3(0.86f, 2.30f, 0.03f), glm::vec3(0.08f, 0.38f, 0.74f), glm::vec3(0.32f), 40.0f, glm::vec3(0.02f, 0.05f, 0.10f));
    for (int i = 0; i < 7; ++i) {
        float y = 0.34f + i * 0.33f;
        drawCubeShape(shader, parent, glm::vec3(0.0f, y, 0.55f), glm::vec3(0.0f), glm::vec3(0.80f, 0.08f, 0.01f), glm::vec3(0.10f, 0.58f, 0.92f), glm::vec3(0.20f), 30.0f, glm::vec3(0.03f, 0.08f, 0.16f));
        drawCubeShape(shader, parent, glm::vec3(-0.28f, y, 0.56f), glm::vec3(0.0f), glm::vec3(0.05f, 0.05f, 0.02f), glm::vec3(0.18f, 0.86f, 0.24f), glm::vec3(0.20f), 16.0f, glm::vec3(0.03f, 0.10f, 0.02f));
        drawCubeShape(shader, parent, glm::vec3(0.24f, y, 0.56f), glm::vec3(0.0f), glm::vec3(0.05f, 0.05f, 0.02f), glm::vec3(0.92f, 0.34f, 0.18f), glm::vec3(0.20f), 16.0f, glm::vec3(0.08f, 0.03f, 0.02f));
    }
}

void drawBookShelf(Shader& shader, const glm::mat4& parent, float width, float height, int columns) {
    glm::vec3 wood(0.54f, 0.38f, 0.22f);
    glm::vec3 darkWood(0.40f, 0.28f, 0.16f);
    drawCubeShape(shader, parent, glm::vec3(0.0f, height * 0.5f, 0.0f), glm::vec3(0.0f), glm::vec3(width, height, 0.40f), wood, glm::vec3(0.12f), 10.0f);
    for (int i = 0; i < 4; ++i) {
        float y = 0.45f + i * (height * 0.22f);
        drawCubeShape(shader, parent, glm::vec3(0.0f, y, 0.03f), glm::vec3(0.0f), glm::vec3(width * 0.92f, 0.05f, 0.34f), darkWood, glm::vec3(0.10f), 8.0f);
    }

    glm::vec3 bookColors[5] = {
        glm::vec3(0.15f, 0.32f, 0.58f),
        glm::vec3(0.68f, 0.18f, 0.20f),
        glm::vec3(0.18f, 0.56f, 0.26f),
        glm::vec3(0.84f, 0.68f, 0.18f),
        glm::vec3(0.48f, 0.28f, 0.64f)
    };

    for (int shelf = 0; shelf < 4; ++shelf) {
        float y = 0.22f + shelf * (height * 0.22f);
        for (int c = 0; c < columns * 3; ++c) {
            float x = -width * 0.40f + c * (width * 0.80f / std::max(1, columns * 3 - 1));
            float w = 0.10f + 0.02f * (c % 3);
            float h = 0.30f + 0.07f * ((c + shelf) % 3);
            drawCubeShape(shader, parent, glm::vec3(x, y + h * 0.5f, 0.22f), glm::vec3(0.0f), glm::vec3(w, h, 0.18f), bookColors[(c + shelf) % 5], glm::vec3(0.10f), 8.0f);
        }
    }
}

void drawDoorPanel(Shader& shader, const glm::mat4& hinge, float angleDeg) {
    glm::mat4 panelBase = glm::rotate(hinge, glm::radians(angleDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 frame(0.22f, 0.24f, 0.28f);
    glm::vec3 glass(0.58f, 0.72f, 0.88f);
    // Door panel — apply door texture
    if (doorTexture > 0) {
        drawCubeShapeTextured(shader, panelBase, glm::vec3(0.50f, 1.05f, 0.0f), glm::vec3(0.0f), glm::vec3(1.00f, 2.10f, 0.06f), frame, doorTexture, glm::vec3(0.22f), 18.0f);
    }
    else {
        drawCubeShape(shader, panelBase, glm::vec3(0.50f, 1.05f, 0.0f), glm::vec3(0.0f), glm::vec3(1.00f, 2.10f, 0.06f), frame, glm::vec3(0.22f), 18.0f);
    }
    drawCubeShape(shader, panelBase, glm::vec3(0.50f, 1.05f, 0.02f), glm::vec3(0.0f), glm::vec3(0.84f, 1.78f, 0.01f), glass, glm::vec3(0.38f), 38.0f, glm::vec3(0.02f, 0.03f, 0.05f));
    drawCubeShape(shader, panelBase, glm::vec3(0.82f, 1.02f, 0.05f), glm::vec3(0.0f), glm::vec3(0.05f, 0.22f, 0.03f), glm::vec3(0.84f, 0.74f, 0.26f), glm::vec3(0.45f), 30.0f);
}

void drawWindowPanel(Shader& shader, const glm::mat4& hinge, float angleDeg) {
    glm::mat4 panelBase = glm::rotate(hinge, glm::radians(angleDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 frame(0.78f, 0.80f, 0.84f);
    glm::vec3 glass(0.60f, 0.76f, 0.90f);
    drawCubeShape(shader, panelBase, glm::vec3(0.55f, 0.90f, 0.0f), glm::vec3(0.0f), glm::vec3(1.10f, 1.80f, 0.05f), frame, glm::vec3(0.24f), 20.0f);
    drawCubeShape(shader, panelBase, glm::vec3(0.55f, 0.90f, 0.03f), glm::vec3(0.0f), glm::vec3(0.92f, 1.58f, 0.01f), glass, glm::vec3(0.38f), 44.0f, glm::vec3(0.02f, 0.03f, 0.05f));
}

void drawWoodDoorPanel(Shader& shader, const glm::mat4& hinge, float angleDeg) {
    glm::mat4 panelBase = glm::rotate(hinge, glm::radians(angleDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 wood(0.67f, 0.44f, 0.22f);
    glm::vec3 trim(0.52f, 0.34f, 0.18f);

    // Full-height solid wood door panel to remove the visible top gap
    if (doorTexture > 0) {
        drawCubeShapeTextured(shader, panelBase, glm::vec3(0.50f, 1.50f, 0.0f), glm::vec3(0.0f), glm::vec3(1.00f, 3.00f, 0.05f), wood, doorTexture, glm::vec3(0.16f), 14.0f);
    }
    else {
        drawCubeShape(shader, panelBase, glm::vec3(0.50f, 1.50f, 0.0f), glm::vec3(0.0f), glm::vec3(1.00f, 3.00f, 0.05f), wood, glm::vec3(0.16f), 14.0f);
    }

    drawCubeShape(shader, panelBase, glm::vec3(0.50f, 2.08f, 0.03f), glm::vec3(0.0f), glm::vec3(0.62f, 0.24f, 0.01f), trim, glm::vec3(0.14f), 10.0f);
    drawCubeShape(shader, panelBase, glm::vec3(0.50f, 0.96f, 0.03f), glm::vec3(0.0f), glm::vec3(0.62f, 0.20f, 0.01f), trim, glm::vec3(0.14f), 10.0f);
    drawCubeShape(shader, panelBase, glm::vec3(0.82f, 1.50f, 0.05f), glm::vec3(0.0f), glm::vec3(0.05f, 0.22f, 0.03f), glm::vec3(0.84f, 0.74f, 0.26f), glm::vec3(0.45f), 30.0f);
}

void drawGlassBarrier(Shader& shader, const glm::mat4& parent, glm::vec3 center, glm::vec3 scale) {
    glm::vec3 glass(0.72f, 0.84f, 0.94f);
    glm::vec3 metal(0.76f, 0.78f, 0.82f);
    drawCubeShape(shader, parent, center, glm::vec3(0.0f), scale, glass, glm::vec3(0.58f), 72.0f, glm::vec3(0.01f, 0.02f, 0.04f));
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, scale.y * 0.52f, 0.0f), glm::vec3(0.0f),
        glm::vec3(scale.x + 0.04f, 0.04f, scale.z + 0.04f), metal, glm::vec3(0.42f), 32.0f);
    drawCubeShape(shader, parent, center - glm::vec3(0.0f, scale.y * 0.48f, 0.0f), glm::vec3(0.0f),
        glm::vec3(scale.x + 0.02f, 0.04f, scale.z + 0.02f), metal, glm::vec3(0.28f), 24.0f);
}

void drawFrontGlassWallWithDoor(Shader& shader, const glm::mat4& parent, glm::vec3 center, float width, float height,
    float doorWidth, float doorHeight, float angleDeg) {
    glm::vec3 glass(0.66f, 0.80f, 0.92f);
    glm::vec3 frame(0.78f, 0.80f, 0.84f);
    glm::vec3 glow(0.02f, 0.03f, 0.05f);

    float sideWidth = std::max(0.30f, (width - doorWidth) * 0.5f);
    float topHeight = std::max(0.20f, height - doorHeight);

    drawCubeShape(shader, parent, center + glm::vec3(-(doorWidth + sideWidth) * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f),
        glm::vec3(sideWidth, height, 0.04f), glass, glm::vec3(0.45f), 44.0f, glow);
    drawCubeShape(shader, parent, center + glm::vec3((doorWidth + sideWidth) * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f),
        glm::vec3(sideWidth, height, 0.04f), glass, glm::vec3(0.45f), 44.0f, glow);
    drawCubeShape(shader, parent, center + glm::vec3(0.0f, doorHeight * 0.5f + topHeight * 0.5f, 0.0f), glm::vec3(0.0f),
        glm::vec3(doorWidth, topHeight, 0.04f), glass, glm::vec3(0.45f), 44.0f, glow);

    drawCubeShape(shader, parent, center + glm::vec3(0.0f, height * 0.5f, 0.0f), glm::vec3(0.0f),
        glm::vec3(width + 0.08f, 0.05f, 0.08f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center - glm::vec3(0.0f, height * 0.5f, 0.0f), glm::vec3(0.0f),
        glm::vec3(width + 0.08f, 0.05f, 0.08f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center + glm::vec3(-width * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.05f, height + 0.08f, 0.08f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center + glm::vec3(width * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.05f, height + 0.08f, 0.08f), frame, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, parent, center + glm::vec3(-doorWidth * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.04f, doorHeight + 0.02f, 0.06f), frame, glm::vec3(0.26f), 18.0f);
    drawCubeShape(shader, parent, center + glm::vec3(doorWidth * 0.5f, 0.0f, 0.0f), glm::vec3(0.0f),
        glm::vec3(0.04f, doorHeight + 0.02f, 0.06f), frame, glm::vec3(0.26f), 18.0f);

    glm::mat4 hinge = makeModel(parent, glm::vec3(center.x - doorWidth * 0.5f, center.y - height * 0.5f, center.z),
        glm::vec3(0.0f), glm::vec3(doorWidth, doorHeight / 2.10f, 1.0f));
    drawDoorPanel(shader, hinge, angleDeg);
}

float animateAngle(float currentAngle, bool opening, float targetAngle, float speed) {
    float target = opening ? targetAngle : 0.0f;
    return currentAngle + (target - currentAngle) * speed * deltaTime;
}

void drawOutdoorScene(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 grass(0.30f, 0.56f, 0.29f);
    glm::vec3 grassFar(0.38f, 0.54f, 0.38f);
    glm::vec3 concrete(0.80f, 0.81f, 0.84f);
    glm::vec3 asphalt(0.20f, 0.21f, 0.24f);
    glm::vec3 hillA(0.48f, 0.60f, 0.50f);
    glm::vec3 hillB(0.42f, 0.55f, 0.46f);

    // Main ground - grass texture (tiled 10x)
    if (grassTexture > 0) {
        shader.setFloat("texRepeat", 25.0f);
        drawCubeShapeTextured(shader, I, glm::vec3(0.0f, -0.72f, 0.0f), glm::vec3(0.0f),
            glm::vec3(105.0f, 1.40f, 105.0f), grass, grassTexture, glm::vec3(0.10f), 8.0f);
        shader.setFloat("texRepeat", 1.0f);
    }
    else {
        drawCubeShape(shader, I, glm::vec3(0.0f, -0.72f, 0.0f), glm::vec3(0.0f),
            glm::vec3(105.0f, 1.40f, 105.0f), grass, glm::vec3(0.10f), 8.0f);
    }

    drawSphereShape(shader, I, glm::vec3(-36.0f, -6.2f, -58.0f), glm::vec3(0.0f), glm::vec3(34.0f, 10.0f, 14.0f), hillA, glm::vec3(0.04f), 6.0f);
    drawSphereShape(shader, I, glm::vec3(4.0f, -7.2f, -62.0f), glm::vec3(0.0f), glm::vec3(46.0f, 12.0f, 16.0f), hillB, glm::vec3(0.04f), 6.0f);
    drawSphereShape(shader, I, glm::vec3(40.0f, -6.3f, -56.0f), glm::vec3(0.0f), glm::vec3(30.0f, 9.0f, 13.0f), hillA, glm::vec3(0.04f), 6.0f);
    drawCubeShape(shader, I, glm::vec3(0.0f, -0.55f, -48.0f), glm::vec3(0.0f),
        glm::vec3(90.0f, 0.10f, 10.0f), grassFar, glm::vec3(0.06f), 6.0f);

    drawCubeShape(shader, I, glm::vec3(-26.0f, -0.48f, 2.0f), glm::vec3(0.0f),
        glm::vec3(4.4f, 0.05f, 42.0f), asphalt, glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(27.0f, -0.48f, 1.0f), glm::vec3(0.0f),
        glm::vec3(4.4f, 0.05f, 40.0f), asphalt, glm::vec3(0.10f), 8.0f);
    for (int i = -5; i <= 5; ++i) {
        drawCubeShape(shader, I, glm::vec3(-26.0f, -0.45f, i * 3.9f), glm::vec3(0.0f),
            glm::vec3(0.30f, 0.01f, 1.5f), glm::vec3(0.92f, 0.88f, 0.22f), glm::vec3(0.10f), 8.0f);
        drawCubeShape(shader, I, glm::vec3(27.0f, -0.45f, i * 3.9f), glm::vec3(0.0f),
            glm::vec3(0.30f, 0.01f, 1.5f), glm::vec3(0.92f, 0.92f, 0.30f), glm::vec3(0.10f), 8.0f);
    }

    drawCubeShape(shader, I, glm::vec3(-1.0f, -0.48f, 12.5f), glm::vec3(0.0f),
        glm::vec3(22.0f, 0.05f, 6.0f), concrete, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(9.0f, -0.48f, 18.2f), glm::vec3(0.0f),
        glm::vec3(15.6f, 0.05f, 9.9f), concrete, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(-18.6f, -0.48f, 10.5f), glm::vec3(0.0f),
        glm::vec3(6.2f, 0.05f, 20.5f), concrete, glm::vec3(0.12f), 10.0f);

    for (int i = 0; i < 7; ++i) {
        drawCubeShape(shader, I, glm::vec3(-19.3f + i * 2.25f, -0.46f, 12.6f - i * 0.10f), glm::vec3(0.0f),
            glm::vec3(1.15f, 0.04f, 0.60f), concrete, glm::vec3(0.10f), 8.0f);
    }

    drawCubeShape(shader, I, glm::vec3(-0.6f, -0.14f, 9.2f), glm::vec3(0.0f),
        glm::vec3(8.2f, 0.24f, 1.6f), glm::vec3(0.69f, 0.70f, 0.73f), glm::vec3(0.18f), 12.0f);
    for (int i = 0; i < 4; ++i) {
        drawCubeShape(shader, I, glm::vec3(-0.6f, -0.01f, 10.05f + i * 0.42f), glm::vec3(0.0f),
            glm::vec3(7.2f - i * 1.0f, 0.06f, 0.35f), concrete, glm::vec3(0.16f), 10.0f);
    }

    drawPlanterStrip(shader, I, glm::vec3(-8.6f, -0.02f, 10.6f), glm::vec3(8.0f, 0.55f, 1.35f));
    drawPlanterStrip(shader, I, glm::vec3(2.4f, -0.02f, 10.6f), glm::vec3(5.0f, 0.55f, 1.35f));
    drawPlanterStrip(shader, I, glm::vec3(1.9f, -0.02f, 4.5f), glm::vec3(3.8f, 0.55f, 2.8f));
    drawPlanterStrip(shader, I, glm::vec3(7.8f, -0.02f, 10.0f), glm::vec3(2.5f, 0.42f, 1.2f));
    drawPlanterStrip(shader, I, glm::vec3(15.6f, -0.02f, 12.9f), glm::vec3(2.8f, 0.36f, 1.4f));
    drawPoolsideBezierVases(shader, I, glm::vec3(10.2f, -0.02f, 13.0f), glm::vec3(4.6f, 0.30f, 1.1f));

    drawPool(shader, makeModel(I, glm::vec3(8.6f, 0.0f, 18.0f), glm::vec3(0.0f), glm::vec3(1.0f)));

    drawSmallTree(shader, I, glm::vec3(-21.0f, 0.0f, 14.8f), 1.12f);
    drawSmallTree(shader, I, glm::vec3(-15.5f, 0.0f, 12.0f), 0.96f);
    drawSmallTree(shader, I, glm::vec3(-10.2f, 0.0f, 11.6f), 0.86f);
    drawSmallTree(shader, I, glm::vec3(4.0f, 0.0f, 11.5f), 0.82f);
    // Removed the right-front greenery that was leaking into the upper-right facade view.
    drawSmallTree(shader, I, glm::vec3(-22.5f, 0.0f, -8.5f), 1.00f);
   // drawSmallTree(shader, I, glm::vec3(22.5f, 0.0f, -9.5f), 1.04f);
    drawFractalTree(shader, I, glm::vec3(26.0f, 0.0f, -18.0f), 1.18f);
}

void drawBuildingEnvelope(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 whiteBand(0.93f, 0.94f, 0.96f);
    glm::vec3 slabUnderside(0.25f, 0.26f, 0.30f);
    glm::vec3 innerWall(0.89f, 0.90f, 0.92f);
    glm::vec3 wood(0.44f, 0.30f, 0.18f);
    glm::vec3 glass(0.67f, 0.80f, 0.91f);
    glm::vec3 greenRoof(0.33f, 0.62f, 0.29f);
    glm::vec3 rail(0.23f, 0.25f, 0.28f);

    drawCubeShape(shader, I, glm::vec3(0.0f, -0.02f, 0.0f), glm::vec3(0.0f), glm::vec3(16.6f, 0.12f, 9.8f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(0.0f, -0.18f, 0.0f), glm::vec3(0.0f), glm::vec3(16.6f, 0.18f, 9.8f), slabUnderside, glm::vec3(0.12f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(0.0f, 0.08f, 9.25f), glm::vec3(0.0f), glm::vec3(16.95f, 0.16f, 0.30f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(-16.25f, 0.08f, 0.0f), glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.32f, 0.16f, 8.9f), whiteBand, glm::vec3(0.18f), 14.0f);

    // FIXED upper-left slab block
    drawCubeShape(shader, I, glm::vec3(-10.0f, 3.98f, 4.90f), glm::vec3(0.0f), glm::vec3(8.0f, 0.12f, 7.90f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(-10.0f, 3.82f, 4.90f), glm::vec3(0.0f), glm::vec3(8.0f, 0.18f, 7.90f), slabUnderside, glm::vec3(0.12f), 8.0f);

    drawCubeShape(shader, I, glm::vec3(9.4f, 3.98f, 0.15f), glm::vec3(0.0f), glm::vec3(7.6f, 0.12f, 9.9f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(9.4f, 3.82f, 0.15f), glm::vec3(0.0f), glm::vec3(7.6f, 0.18f, 9.9f), slabUnderside, glm::vec3(0.12f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(0.6f, 3.98f, -4.4f), glm::vec3(0.0f), glm::vec3(4.8f, 0.12f, 5.4f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(0.6f, 3.82f, -4.4f), glm::vec3(0.0f), glm::vec3(4.8f, 0.18f, 5.4f), slabUnderside, glm::vec3(0.12f), 8.0f);

    drawCubeShape(shader, I, glm::vec3(-9.2f, 4.08f, 9.26f), glm::vec3(0.0f), glm::vec3(7.6f, 0.18f, 0.30f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(9.4f, 4.08f, 9.26f), glm::vec3(0.0f), glm::vec3(7.8f, 0.18f, 0.30f), whiteBand, glm::vec3(0.18f), 14.0f);

    drawCubeShape(shader, I, glm::vec3(-9.0f, 8.06f, -0.5f), glm::vec3(0.0f), glm::vec3(7.9f, 0.10f, 10.3f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(9.2f, 8.06f, -0.5f), glm::vec3(0.0f), glm::vec3(7.9f, 0.10f, 10.3f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(-9.0f, 7.90f, -0.5f), glm::vec3(0.0f), glm::vec3(7.9f, 0.16f, 10.3f), slabUnderside, glm::vec3(0.12f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(9.2f, 7.90f, -0.5f), glm::vec3(0.0f), glm::vec3(7.9f, 0.16f, 10.3f), slabUnderside, glm::vec3(0.12f), 8.0f);

    drawCubeShape(shader, I, glm::vec3(-8.4f, 8.18f, -0.8f), glm::vec3(0.0f), glm::vec3(6.2f, 0.12f, 8.0f), greenRoof, glm::vec3(0.08f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(8.7f, 8.18f, -0.8f), glm::vec3(0.0f), glm::vec3(6.2f, 0.12f, 8.0f), greenRoof, glm::vec3(0.08f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(3.6f, 8.34f, -3.9f), glm::vec3(0.0f), glm::vec3(9.2f, 0.12f, 5.2f), glm::vec3(0.76f, 0.77f, 0.80f), glm::vec3(0.14f), 12.0f);

    drawCubeShape(shader, I, glm::vec3(-15.12f, 2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 9.3f), wood, glm::vec3(0.12f), 10.0f);
    // drawCubeShape(shader, I, glm::vec3(15.12f, 2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 9.3f), wood, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(-15.12f, 2.0f, -2.215f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 13.73f), wood, glm::vec3(0.12f), 10.0f);
    // drawCubeShape(shader, I, glm::vec3(-15.12f, 6.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 9.3f), wood, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(-15.12f, 6.0f, -2.215f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 13.73f), wood, glm::vec3(0.12f), 10.0f);
    // drawCubeShape(shader, I, glm::vec3(15.12f, 6.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 9.3f), wood, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(15.12f, 6.0f, -2.215f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 13.73f), wood, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(15.12f, 2.0f, -2.215f), glm::vec3(0.0f), glm::vec3(0.24f, 4.0f, 13.73f), wood, glm::vec3(0.12f), 10.0f);

    // drawCubeShape(shader, I, glm::vec3(0.0f, 2.0f, -9.08f), glm::vec3(0.0f), glm::vec3(15.3f, 4.0f, 0.22f), innerWall, glm::vec3(0.16f), 12.0f);
   // drawCubeShape(shader, I, glm::vec3(0.0f, 2.0f, -9.08f), glm::vec3(0.0f), glm::vec3(30.5f, 4.0f, 0.22f), innerWall, glm::vec3(0.16f), 12.0f);
    // drawCubeShape(shader, I, glm::vec3(0.0f, 6.0f, -9.08f), glm::vec3(0.0f), glm::vec3(15.3f, 4.0f, 0.22f), innerWall, glm::vec3(0.16f), 12.0f);
    // drawCubeShape(shader, I, glm::vec3(0.0f, 6.0f, -9.08f), glm::vec3(0.0f), glm::vec3(30.5f, 4.0f, 0.22f), innerWall, glm::vec3(0.16f), 12.0f);

    drawWoodAccentPanel(shader, I, glm::vec3(-12.7f, 2.0f, 9.12f), glm::vec3(5.0f, 4.0f, 0.18f));
    drawWoodAccentPanel(shader, I, glm::vec3(12.6f, 2.0f, 9.12f), glm::vec3(5.0f, 4.0f, 0.18f));
    drawWoodAccentPanel(shader, I, glm::vec3(-0.8f, 6.0f, -6.2f), glm::vec3(5.4f, 4.0f, 0.18f));

    drawFrontOpeningFrame(shader, I, glm::vec3(-10.4f, 1.95f, 9.08f), 8.2f, 3.55f, glm::vec3(0.78f, 0.80f, 0.84f), 5, glm::vec3(0.03f, 0.05f, 0.08f));
    drawFrontOpeningFrame(shader, I, glm::vec3(10.4f, 1.95f, 9.08f), 8.2f, 3.55f, glm::vec3(0.78f, 0.80f, 0.84f), 5, glm::vec3(0.02f, 0.04f, 0.06f));
    drawFrontOpeningFrame(shader, I, glm::vec3(-10.0f, 5.95f, 9.08f), 8.3f, 3.45f, glm::vec3(0.78f, 0.80f, 0.84f), 6, glm::vec3(0.02f, 0.03f, 0.05f));
    drawFrontOpeningFrame(shader, I, glm::vec3(10.2f, 5.95f, 9.08f), 8.5f, 3.45f, glm::vec3(0.78f, 0.80f, 0.84f), 6, glm::vec3(0.02f, 0.03f, 0.05f));

    drawSideGlassWall(shader, I, glm::vec3(-14.88f, 1.95f, 5.0f), 7.3f, 3.55f, glass);
    drawSideGlassWall(shader, I, glm::vec3(14.88f, 1.95f, 5.0f), 7.3f, 3.55f, glass);
    drawSideGlassWall(shader, I, glm::vec3(-14.88f, 5.95f, 5.15f), 7.6f, 3.55f, glass);
    drawSideGlassWall(shader, I, glm::vec3(14.88f, 5.95f, 5.15f), 7.6f, 3.55f, glass);

    drawCubeShape(shader, I, glm::vec3(-3.40f, 2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.20f, 4.0f, 9.3f), innerWall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(5.40f, 2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.20f, 4.0f, 9.3f), innerWall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(-3.40f, 6.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.20f, 4.0f, 9.3f), innerWall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(3.80f, 6.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.20f, 4.0f, 9.3f), innerWall, glm::vec3(0.12f), 10.0f);

    // drawCubeShape(shader, I, glm::vec3(0.9f, 2.0f, 0.8f), glm::vec3(0.0f), glm::vec3(8.8f, 4.0f, 0.18f), innerWall, glm::vec3(0.12f), 10.0f);  // removed: was blocking bridge path
    // drawCubeShape(shader, I, glm::vec3(0.4f, 6.0f, 0.8f), glm::vec3(0.0f), glm::vec3(7.5f, 4.0f, 0.18f), innerWall, glm::vec3(0.12f), 10.0f);

    // drawCubeShape(shader, I, glm::vec3(0.8f, 5.06f, 8.95f), glm::vec3(0.0f), glm::vec3(8.9f, 1.0f, 0.10f), glm::vec3(0.82f, 0.84f, 0.88f), glm::vec3(0.16f), 12.0f);  // removed: front railing band
    // drawCubeShape(shader, I, glm::vec3(0.8f, 4.60f, 8.98f), glm::vec3(0.0f), glm::vec3(8.3f, 0.06f, 0.04f), glm::vec3(0.70f, 0.78f, 0.86f), glm::vec3(0.18f), 16.0f);  // removed: front railing trim

    drawGlassBarrier(shader, I, glm::vec3(-7.0f, 4.55f, 8.82f), glm::vec3(14.0f, 0.85f, 0.03f));
    drawGlassBarrier(shader, I, glm::vec3(10.0f, 4.55f, 8.82f), glm::vec3(8.0f, 0.85f, 0.03f));
    drawGlassBarrier(shader, I, glm::vec3(0.9f, 4.55f, 8.82f), glm::vec3(4.0f, 0.85f, 0.03f));

    //drawSignPanel(shader, I, glm::vec3(-14.96f, 2.20f, 2.8f), glm::vec3(4.2f, 1.9f, 0.04f), true);
    //drawSignPanel(shader, I, glm::vec3(12.8f, 8.90f, 1.8f), glm::vec3(6.6f, 1.9f, 0.05f), true);
    //drawSignPanel(shader, I, glm::vec3(15.15f, 6.20f, 2.7f), glm::vec3(2.5f, 1.3f, 0.03f), false);

    // drawDoorPanel(shader, makeModel(I, glm::vec3(15.00f, 0.0f, 6.55f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f)), doorAngle);
}

void drawServerRoom(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 floor(0.18f, 0.20f, 0.24f);
    glm::vec3 backWall(0.86f, 0.88f, 0.91f);

    drawCubeShape(shader, I, glm::vec3(-10.40f, 0.08f, 4.85f), glm::vec3(0.0f), glm::vec3(8.20f, 0.03f, 7.35f), floor, glm::vec3(0.16f), 12.0f);
    // Server room back wall — wall texture
    if (wallTexture > 0) {
        drawCubeShapeTextured(shader, I, glm::vec3(-10.40f, 1.60f, 1.18f), glm::vec3(0.0f), glm::vec3(8.20f, 3.20f, 0.08f), backWall, wallTexture, glm::vec3(0.12f), 10.0f);
    }
    else {
        drawCubeShape(shader, I, glm::vec3(-10.40f, 1.60f, 1.18f), glm::vec3(0.0f), glm::vec3(8.20f, 3.20f, 0.08f), backWall, glm::vec3(0.12f), 10.0f);
    }
    drawSideGlassWall(shader, I, glm::vec3(-14.48f, 1.60f, 4.82f), 7.28f, 3.20f);
    drawSideGlassWall(shader, I, glm::vec3(-6.32f, 1.60f, 4.82f), 7.28f, 3.20f);
    drawFrontGlassWallWithDoor(shader, I, glm::vec3(-10.40f, 1.60f, 8.42f), 8.16f, 3.20f, 1.32f, 2.14f, -serverDoorAngle);

    drawCubeShape(shader, I, glm::vec3(-10.40f, 0.10f, 8.42f), glm::vec3(0.0f), glm::vec3(8.16f, 0.03f, 0.14f), glm::vec3(0.82f, 0.84f, 0.88f), glm::vec3(0.18f), 16.0f);
    drawCubeShape(shader, I, glm::vec3(-10.40f, 3.28f, 8.34f), glm::vec3(0.0f), glm::vec3(2.20f, 0.28f, 0.03f), glm::vec3(0.12f, 0.34f, 0.64f), glm::vec3(0.20f), 20.0f, glm::vec3(0.02f, 0.04f, 0.08f));

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            float x = -13.00f + col * 2.10f;
            float z = 3.15f + row * 2.15f;
            drawServerRack(shader, makeModel(I, glm::vec3(x, 0.02f, z), glm::vec3(0.0f), glm::vec3(0.96f)));
        }
    }

    drawCubeShape(shader, I, glm::vec3(-7.20f, 0.52f, 2.20f), glm::vec3(0.0f), glm::vec3(0.82f, 0.92f, 1.60f), glm::vec3(0.22f, 0.24f, 0.28f), glm::vec3(0.16f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(-7.20f, 3.15f, 7.85f), glm::vec3(0.0f), glm::vec3(1.40f, 0.42f, 0.04f), glm::vec3(0.10f, 0.55f, 0.18f), glm::vec3(0.18f), 18.0f, glm::vec3(0.02f, 0.10f, 0.03f));
}

void drawLibraryRoom(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 woodFloor(0.66f, 0.47f, 0.28f);
    glm::vec3 backWall(0.88f, 0.89f, 0.92f);
    glm::vec3 carpet(0.74f, 0.64f, 0.48f);
    glm::vec3 sofaA(0.45f, 0.56f, 0.34f);
    glm::vec3 sofaB(0.56f, 0.52f, 0.34f);
    glm::vec3 tableWood(0.62f, 0.44f, 0.24f);

    drawCubeShape(shader, I, glm::vec3(-1.15f, 0.08f, 4.70f), glm::vec3(0.0f), glm::vec3(4.20f, 0.03f, 7.20f), woodFloor, glm::vec3(0.12f), 10.0f);
    // Library room back wall — wall texture
    if (wallTexture > 0) {
        drawCubeShapeTextured(shader, I, glm::vec3(-1.15f, 1.60f, 1.12f), glm::vec3(0.0f), glm::vec3(4.20f, 3.20f, 0.08f), backWall, wallTexture, glm::vec3(0.12f), 10.0f);
    }
    else {
        drawCubeShape(shader, I, glm::vec3(-1.15f, 1.60f, 1.12f), glm::vec3(0.0f), glm::vec3(4.20f, 3.20f, 0.08f), backWall, glm::vec3(0.12f), 10.0f);
    }
    drawSideGlassWall(shader, I, glm::vec3(-3.22f, 1.60f, 4.66f), 7.10f, 3.20f);
    drawSideGlassWall(shader, I, glm::vec3(0.92f, 1.60f, 4.66f), 7.10f, 3.20f);
    drawFrontGlassWallWithDoor(shader, I, glm::vec3(-1.15f, 1.60f, 8.22f), 4.16f, 3.20f, 1.18f, 3.20f, libraryDoorAngle);

    drawBookShelf(shader, makeModel(I, glm::vec3(-1.15f, 0.02f, 1.58f), glm::vec3(0.0f), glm::vec3(1.0f)), 3.80f, 3.00f, 5);
    drawBookShelf(shader, makeModel(I, glm::vec3(-2.90f, 0.02f, 4.50f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.82f)), 4.20f, 2.70f, 4);

    // Lift the carpet slightly above the wood floor so the library no longer flickers.
    drawCubeShape(shader, I, glm::vec3(-1.10f, 0.11f, 5.00f), glm::vec3(0.0f), glm::vec3(2.40f, 0.02f, 1.60f), carpet, glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(-1.10f, 0.84f, 4.78f), glm::vec3(0.0f), glm::vec3(2.10f, 0.10f, 0.90f), tableWood, glm::vec3(0.12f), 10.0f);
    for (int i = 0; i < 4; ++i) {
        float x = -1.95f + (i % 2) * 1.70f;
        float z = 4.78f + (i < 2 ? -0.92f : 0.92f);
        float rot = (i < 2) ? 180.0f : 0.0f;
        drawChair(shader, makeModel(I, glm::vec3(x, 0.02f, z), glm::vec3(0.0f, rot, 0.0f), glm::vec3(0.78f)));
    }

    drawCubeShape(shader, I, glm::vec3(-2.15f, 0.40f, 6.35f), glm::vec3(0.0f), glm::vec3(1.20f, 0.68f, 0.76f), sofaA, glm::vec3(0.14f), 12.0f);
    drawCubeShape(shader, I, glm::vec3(0.00f, 0.40f, 6.35f), glm::vec3(0.0f), glm::vec3(1.20f, 0.68f, 0.76f), sofaB, glm::vec3(0.14f), 12.0f);
    drawCubeShape(shader, I, glm::vec3(-2.15f, 0.86f, 6.64f), glm::vec3(0.0f), glm::vec3(1.20f, 0.56f, 0.24f), sofaA, glm::vec3(0.14f), 12.0f);
    drawCubeShape(shader, I, glm::vec3(0.00f, 0.86f, 6.64f), glm::vec3(0.0f), glm::vec3(1.20f, 0.56f, 0.24f), sofaB, glm::vec3(0.14f), 12.0f);
    drawCubeShape(shader, I, glm::vec3(-1.08f, 0.36f, 5.95f), glm::vec3(0.0f), glm::vec3(0.82f, 0.18f, 0.72f), tableWood, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, I, glm::vec3(-1.08f, 0.52f, 5.95f), glm::vec3(0.0f), glm::vec3(0.26f, 0.08f, 0.26f), glm::vec3(0.86f, 0.86f, 0.88f), glm::vec3(0.18f), 18.0f);
    drawPottedPlant(shader, I, glm::vec3(0.30f, 0.0f, 6.05f), 0.88f);
    drawPottedPlant(shader, I, glm::vec3(-2.70f, 0.0f, 6.15f), 0.88f);
}

void drawReceptionRoom(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 lounge(0.52f, 0.56f, 0.64f);
    glm::vec3 table(0.84f, 0.84f, 0.86f);
    glm::vec3 backWall(0.88f, 0.89f, 0.92f);

    drawCubeShape(shader, I, glm::vec3(10.40f, 0.08f, 4.85f), glm::vec3(0.0f), glm::vec3(8.20f, 0.03f, 7.35f), glm::vec3(0.84f, 0.84f, 0.86f), glm::vec3(0.16f), 12.0f);
    // Reception room back wall — wall texture
    if (wallTexture > 0) {
        drawCubeShapeTextured(shader, I, glm::vec3(10.40f, 1.60f, 1.18f), glm::vec3(0.0f), glm::vec3(8.20f, 3.20f, 0.08f), backWall, wallTexture, glm::vec3(0.12f), 10.0f);
    }
    else {
        drawCubeShape(shader, I, glm::vec3(10.40f, 1.60f, 1.18f), glm::vec3(0.0f), glm::vec3(8.20f, 3.20f, 0.08f), backWall, glm::vec3(0.12f), 10.0f);
    }
    drawSideGlassWall(shader, I, glm::vec3(6.32f, 1.60f, 4.82f), 7.28f, 3.20f);
    drawSideGlassWall(shader, I, glm::vec3(14.48f, 1.60f, 4.82f), 7.28f, 3.20f);
    drawFrontGlassWallWithDoor(shader, I, glm::vec3(10.40f, 1.60f, 8.42f), 8.16f, 3.20f, 1.36f, 3.20f, receptionDoorAngle);

    drawWoodAccentPanel(shader, I, glm::vec3(10.80f, 2.00f, 1.26f), glm::vec3(5.80f, 4.00f, 0.08f));
    drawSignPanel(shader, I, glm::vec3(10.80f, 2.82f, 1.36f), glm::vec3(4.40f, 0.90f, 0.02f), false);
    drawCubeShape(shader, I, glm::vec3(10.80f, 2.24f, 2.00f), glm::vec3(0.0f), glm::vec3(3.90f, 1.82f, 0.04f), glm::vec3(0.12f, 0.36f, 0.66f), glm::vec3(0.22f), 24.0f, glm::vec3(0.03f, 0.05f, 0.10f));

    drawReceptionDesk(shader, makeModel(I, glm::vec3(10.40f, 0.02f, 5.15f), glm::vec3(0.0f), glm::vec3(1.0f)));
    drawCubeShape(shader, I, glm::vec3(13.00f, 0.40f, 6.05f), glm::vec3(0.0f), glm::vec3(1.70f, 0.70f, 0.90f), lounge, glm::vec3(0.16f), 12.0f);
    drawCubeShape(shader, I, glm::vec3(13.00f, 0.86f, 6.32f), glm::vec3(0.0f), glm::vec3(1.70f, 0.54f, 0.22f), lounge, glm::vec3(0.16f), 12.0f);
    drawCubeShape(shader, I, glm::vec3(7.85f, 0.28f, 6.10f), glm::vec3(0.0f), glm::vec3(1.10f, 0.12f, 0.86f), table, glm::vec3(0.18f), 14.0f);
    drawChair(shader, makeModel(I, glm::vec3(7.35f, 0.02f, 6.95f), glm::vec3(0.0f, 135.0f, 0.0f), glm::vec3(0.82f)));
    drawChair(shader, makeModel(I, glm::vec3(8.35f, 0.02f, 6.95f), glm::vec3(0.0f, -135.0f, 0.0f), glm::vec3(0.82f)));
}

void drawCentralAtrium(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 floor(0.78f, 0.79f, 0.82f);
    glm::vec3 metal(0.62f, 0.63f, 0.66f);
    glm::vec3 glass(0.66f, 0.80f, 0.92f);
    glm::vec3 glassSpec(0.72f, 0.78f, 0.84f);
    glm::vec3 glassGlow(0.03f, 0.06f, 0.10f);
    glm::vec3 edgeMetal(0.74f, 0.76f, 0.80f);
    float glassShin = 96.0f;

    drawCubeShape(shader, I, glm::vec3(1.0f, 0.08f, 4.7f), glm::vec3(0.0f), glm::vec3(4.2f, 0.03f, 7.8f), floor, glm::vec3(0.16f), 12.0f);
    drawPlanterStrip(shader, I, glm::vec3(1.0f, -0.02f, 3.5f), glm::vec3(2.8f, 0.42f, 1.7f));

    for (int i = 0; i < 15; ++i) {
        float y = 0.12f + i * 0.26f;
        float z = 7.0f - i * 0.40f;
        drawCubeShape(shader, I, glm::vec3(2.6f, y, z), glm::vec3(0.0f), glm::vec3(2.6f, 0.12f, 0.36f), floor, glm::vec3(0.16f), 12.0f);
    }
    drawCubeShape(shader, I, glm::vec3(1.25f, 1.8f, 4.1f), glm::vec3(43.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.15f, 6.5f), metal, glm::vec3(0.32f), 22.0f);  // removed: stair climber side stand
    drawCubeShape(shader, I, glm::vec3(3.95f, 2.10f, 4.1f), glm::vec3(43.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.15f, 6.5f), metal, glm::vec3(0.32f), 22.0f);  // removed: stair climber side stand

    auto drawGlassDeck = [&](glm::vec3 center, glm::vec3 deckScale) {
        drawCubeShape(shader, I, center, glm::vec3(0.0f), deckScale, glass, glassSpec, glassShin, glassGlow);
        drawCubeShape(shader, I, center - glm::vec3(0.0f, deckScale.y * 0.78f, 0.0f), glm::vec3(0.0f),
            glm::vec3(deckScale.x, deckScale.y * 0.70f, deckScale.z), glm::vec3(0.18f, 0.30f, 0.44f), glm::vec3(0.20f), 24.0f, glassGlow * 0.5f);
        };

    drawGlassDeck(glm::vec3(1.00f, 4.09f, 1.30f), glm::vec3(6.10f, 0.07f, 2.70f));
    drawGlassDeck(glm::vec3(-0.20f, 4.09f, -0.55f), glm::vec3(4.40f, 0.07f, 1.35f));
    drawGlassDeck(glm::vec3(1.00f, 4.09f, 4.25f), glm::vec3(3.10f, 0.07f, 4.55f));
    drawGlassDeck(glm::vec3(0.15f, 4.09f, 6.95f), glm::vec3(7.00f, 0.07f, 2.60f));
    drawGlassDeck(glm::vec3(-4.95f, 4.09f, 6.95f), glm::vec3(3.45f, 0.07f, 2.60f));
    drawGlassDeck(glm::vec3(5.15f, 4.09f, 6.95f), glm::vec3(3.45f, 0.07f, 2.60f));

    drawCubeShape(shader, I, glm::vec3(1.00f, 4.10f, 2.62f), glm::vec3(0.0f), glm::vec3(6.14f, 0.08f, 0.04f), edgeMetal, glm::vec3(0.38f), 28.0f);
    drawCubeShape(shader, I, glm::vec3(-0.20f, 4.10f, 0.12f), glm::vec3(0.0f), glm::vec3(4.44f, 0.08f, 0.04f), edgeMetal, glm::vec3(0.38f), 28.0f);
    drawCubeShape(shader, I, glm::vec3(0.15f, 4.10f, 8.24f), glm::vec3(0.0f), glm::vec3(13.90f, 0.08f, 0.04f), edgeMetal, glm::vec3(0.38f), 28.0f);

    /* drawGlassBarrier(shader, I, glm::vec3(-2.05f, 4.55f, 1.30f), glm::vec3(0.03f, 0.85f, 2.65f));
     drawGlassBarrier(shader, I, glm::vec3(4.05f, 4.55f, 1.30f), glm::vec3(0.03f, 0.85f, 2.65f));
     drawGlassBarrier(shader, I, glm::vec3(-2.38f, 4.55f, -0.55f), glm::vec3(0.03f, 0.85f, 1.34f));
     drawGlassBarrier(shader, I, glm::vec3(2.00f, 4.55f, -0.55f), glm::vec3(0.03f, 0.85f, 1.34f));*/
     // Fill the open stair-side gap with a solid upper slab
    drawCubeShape(shader, I, glm::vec3(2.35f, 4.09f, -0.55f), glm::vec3(0.0f),
        glm::vec3(2.10f, 0.08f, 1.35f), floor, glm::vec3(0.16f), 12.0f);

    // Underside support so it does not look hollow from below
    drawCubeShape(shader, I, glm::vec3(2.35f, 3.88f, -0.55f), glm::vec3(0.0f),
        glm::vec3(2.10f, 0.30f, 1.35f), glm::vec3(0.76f, 0.78f, 0.82f), glm::vec3(0.12f), 10.0f);
    /* drawGlassBarrier(shader, I, glm::vec3(-0.58f, 4.55f, 4.25f), glm::vec3(0.03f, 0.85f, 4.50f));
     drawGlassBarrier(shader, I, glm::vec3(2.58f, 4.55f, 4.25f), glm::vec3(0.03f, 0.85f, 4.50f));
     drawGlassBarrier(shader, I, glm::vec3(0.15f, 4.55f, 8.22f), glm::vec3(13.90f, 0.85f, 0.03f));*/

    drawCubeShape(shader, I, glm::vec3(-1.90f, 2.00f, 1.10f), glm::vec3(0.0f), glm::vec3(0.10f, 4.00f, 0.10f), edgeMetal, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, I, glm::vec3(3.90f, 2.00f, 1.10f), glm::vec3(0.0f), glm::vec3(0.10f, 4.00f, 0.10f), edgeMetal, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, I, glm::vec3(-5.90f, 2.00f, 6.95f), glm::vec3(0.0f), glm::vec3(0.10f, 4.00f, 0.10f), edgeMetal, glm::vec3(0.28f), 20.0f);
    drawCubeShape(shader, I, glm::vec3(6.20f, 2.00f, 6.95f), glm::vec3(0.0f), glm::vec3(0.10f, 4.00f, 0.10f), edgeMetal, glm::vec3(0.28f), 20.0f);
}

void drawUpperLeftClassroom(Shader& shader) {
    glm::mat4 upper = makeModel(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    glm::vec3 wall(0.88f, 0.89f, 0.92f);

    drawCubeShape(shader, upper, glm::vec3(-10.0f, 0.08f, 4.9f), glm::vec3(0.0f), glm::vec3(8.0f, 0.03f, 7.9f), glm::vec3(0.84f, 0.84f, 0.86f), glm::vec3(0.16f), 12.0f);
    drawCubeShape(shader, upper, glm::vec3(-10.0f, 3.72f, 4.9f), glm::vec3(0.0f), glm::vec3(8.0f, 0.04f, 7.9f), glm::vec3(0.22f, 0.24f, 0.28f), glm::vec3(0.10f), 8.0f);
    drawCubeShape(shader, upper, glm::vec3(-10.0f, 2.20f, 1.46f), glm::vec3(0.0f), glm::vec3(5.6f, 1.72f, 0.06f), glm::vec3(0.18f, 0.20f, 0.22f), glm::vec3(0.16f), 18.0f);
    drawCubeShape(shader, upper, glm::vec3(-10.0f, 2.20f, 1.52f), glm::vec3(0.0f), glm::vec3(4.6f, 1.22f, 0.02f), glm::vec3(0.90f, 0.92f, 0.94f), glm::vec3(0.22f), 28.0f, glm::vec3(0.02f, 0.02f, 0.02f));
    // drawSignPanel(shader, upper, glm::vec3(-11.7f, 3.25f, 3.0f), glm::vec3(2.6f, 0.58f, 0.04f), false);
    // drawSignPanel(shader, upper, glm::vec3(-5.8f, 3.35f, 3.3f), glm::vec3(2.8f, 0.52f, 0.04f), false);

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            float x = -13.1f + col * 2.55f;
            float z = 4.2f + row * 2.05f;
            drawDesk(shader, makeModel(upper, glm::vec3(x, 0.02f, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.92f)), true);
        }
    }

    // Upper-left classroom back wall — wall texture
    if (wallTexture > 0) {
        drawCubeShapeTextured(shader, upper, glm::vec3(-10.0f, 2.0f, 0.95f), glm::vec3(0.0f), glm::vec3(8.0f, 4.0f, 0.08f), wall, wallTexture, glm::vec3(0.12f), 10.0f);
    }
    else {
        drawCubeShape(shader, upper, glm::vec3(-10.0f, 2.0f, 0.95f), glm::vec3(0.0f), glm::vec3(8.0f, 4.0f, 0.08f), wall, glm::vec3(0.12f), 10.0f);
    }
    drawCubeShape(shader, upper, glm::vec3(-14.0f, 2.0f, 4.90f), glm::vec3(0.0f), glm::vec3(0.08f, 4.0f, 7.90f), wall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, upper, glm::vec3(-6.02f, 2.0f, 3.48f), glm::vec3(0.0f), glm::vec3(0.08f, 4.0f, 5.05f), wall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, upper, glm::vec3(-6.02f, 2.0f, 8.34f), glm::vec3(0.0f), glm::vec3(0.08f, 4.0f, 1.42f), wall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, upper, glm::vec3(-6.02f, 3.28f, 6.92f), glm::vec3(0.0f), glm::vec3(0.08f, 0.56f, 1.45f), wall, glm::vec3(0.12f), 10.0f);
    drawWoodDoorPanel(shader, makeModel(upper, glm::vec3(-6.06f, 0.0f, 6.00f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.30f, 1.0f, 1.0f)), upperLeftDoorAngle);
    drawCubeShape(shader, upper, glm::vec3(-5.55f, 0.10f, 6.92f), glm::vec3(0.0f), glm::vec3(1.10f, 0.04f, 1.35f), glm::vec3(0.70f, 0.82f, 0.93f), glm::vec3(0.34f), 48.0f, glm::vec3(0.02f, 0.04f, 0.06f));

    drawCubeShape(shader, upper, glm::vec3(-6.75f, 0.90f, 2.6f), glm::vec3(0.0f), glm::vec3(0.70f, 1.70f, 1.00f), glm::vec3(0.82f, 0.84f, 0.86f), glm::vec3(0.14f), 12.0f);
    // drawCubeShape(shader, upper, glm::vec3(-6.42f, 1.05f, 4.2f), glm::vec3(0.0f), glm::vec3(0.22f, 0.52f, 1.10f), glm::vec3(0.20f, 0.22f, 0.26f), glm::vec3(0.18f), 16.0f);
    // drawCubeShape(shader, upper, glm::vec3(-6.42f, 1.05f, 5.7f), glm::vec3(0.0f), glm::vec3(0.22f, 0.52f, 1.10f), glm::vec3(0.20f, 0.22f, 0.26f), glm::vec3(0.18f), 16.0f);
     // drawBookShelf(shader, makeModel(upper, glm::vec3(-6.65f, 0.02f, 8.05f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.78f)), 3.2f, 2.3f, 4);  // removed: bookshelf near door
}

void drawUpperRightClassroom(Shader& shader) {
    glm::mat4 upper = makeModel(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    glm::vec3 wall(0.88f, 0.89f, 0.92f);

    drawCubeShape(shader, upper, glm::vec3(10.2f, 0.08f, 4.9f), glm::vec3(0.0f), glm::vec3(8.3f, 0.03f, 7.9f), glm::vec3(0.84f, 0.84f, 0.86f), glm::vec3(0.16f), 12.0f);
    drawCubeShape(shader, upper, glm::vec3(10.2f, 3.72f, 4.9f), glm::vec3(0.0f), glm::vec3(8.3f, 0.04f, 7.9f), glm::vec3(0.22f, 0.24f, 0.28f), glm::vec3(0.10f), 8.0f);

    drawCubeShape(shader, upper, glm::vec3(10.2f, 2.20f, 1.46f), glm::vec3(0.0f), glm::vec3(5.2f, 1.70f, 0.06f), glm::vec3(0.18f, 0.20f, 0.22f), glm::vec3(0.16f), 18.0f);
    drawCubeShape(shader, upper, glm::vec3(10.2f, 2.20f, 1.52f), glm::vec3(0.0f), glm::vec3(4.3f, 1.22f, 0.02f), glm::vec3(0.90f, 0.92f, 0.94f), glm::vec3(0.22f), 28.0f, glm::vec3(0.02f, 0.02f, 0.02f));
    // Removed dark blue sign panel and white line on the back wall
    // drawCubeShape(shader, upper, glm::vec3(12.8f, 2.25f, 1.48f), glm::vec3(0.0f), glm::vec3(3.1f, 1.85f, 0.06f), glm::vec3(0.12f, 0.24f, 0.46f), glm::vec3(0.20f), 28.0f, glm::vec3(0.03f, 0.06f, 0.12f));
    // drawCubeShape(shader, upper, glm::vec3(12.8f, 3.45f, 1.56f), glm::vec3(0.0f), glm::vec3(1.2f, 0.22f, 0.03f), glm::vec3(0.84f, 0.84f, 0.86f), glm::vec3(0.18f), 16.0f);

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            float x = 7.25f + col * 2.15f;
            float z = 4.2f + row * 2.0f;
            drawDesk(shader, makeModel(upper, glm::vec3(x, 0.02f, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.88f)), true);
        }
    }

    // Upper-right classroom back wall — wall texture
    if (wallTexture > 0) {
        drawCubeShapeTextured(shader, upper, glm::vec3(10.2f, 2.0f, 0.95f), glm::vec3(0.0f), glm::vec3(8.3f, 4.0f, 0.08f), wall, wallTexture, glm::vec3(0.12f), 10.0f);
    }
    else {
        drawCubeShape(shader, upper, glm::vec3(10.2f, 2.0f, 0.95f), glm::vec3(0.0f), glm::vec3(8.3f, 4.0f, 0.08f), wall, glm::vec3(0.12f), 10.0f);
    }

    drawCubeShape(shader, upper, glm::vec3(6.05f, 2.0f, 3.48f), glm::vec3(0.0f), glm::vec3(0.08f, 4.0f, 5.05f), wall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, upper, glm::vec3(6.05f, 2.0f, 8.34f), glm::vec3(0.0f), glm::vec3(0.08f, 4.0f, 1.42f), wall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, upper, glm::vec3(6.05f, 3.28f, 6.92f), glm::vec3(0.0f), glm::vec3(0.08f, 0.56f, 1.45f), wall, glm::vec3(0.12f), 10.0f);
    drawWoodDoorPanel(shader, makeModel(upper, glm::vec3(5.98f, 0.0f, 6.32f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.18f, 0.99f, 1.0f)), -upperRightDoorAngle);
    drawCubeShape(shader, upper, glm::vec3(5.50f, 0.10f, 6.92f), glm::vec3(0.0f), glm::vec3(1.10f, 0.04f, 1.35f), glm::vec3(0.70f, 0.82f, 0.93f), glm::vec3(0.34f), 48.0f, glm::vec3(0.02f, 0.04f, 0.06f));

    drawDesk(shader, makeModel(upper, glm::vec3(13.20f, 0.02f, 3.15f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.85f)), false);
    drawCubeShape(shader, upper, glm::vec3(14.0f, 1.00f, 6.6f), glm::vec3(0.0f), glm::vec3(0.18f, 2.0f, 0.18f), glm::vec3(0.58f, 0.58f, 0.60f), glm::vec3(0.20f), 18.0f);
    //drawCubeShape(shader, upper, glm::vec3(14.0f, 1.78f, 6.6f), glm::vec3(0.0f), glm::vec3(0.70f, 0.18f, 0.70f), glm::vec3(0.82f, 0.82f, 0.84f), glm::vec3(0.22f), 18.0f);
   // drawPottedPlant(shader, upper, glm::vec3(13.55f, 0.0f, 7.55f), 0.90f);
}

void drawWashroom(Shader& shader, const glm::mat4& parent) {
    glm::vec3 tile(0.90f, 0.91f, 0.93f);
    glm::vec3 white(0.96f, 0.97f, 0.98f);
    glm::vec3 metal(0.72f, 0.73f, 0.76f);
    glm::vec3 mirror(0.64f, 0.78f, 0.90f);
    glm::vec3 partition(0.82f, 0.84f, 0.88f);

    drawCubeShape(shader, parent, glm::vec3(0.0f, 0.08f, 0.0f), glm::vec3(0.0f), glm::vec3(2.2f, 0.03f, 4.1f), tile, glm::vec3(0.16f), 14.0f);
    drawCubeShape(shader, parent, glm::vec3(0.25f, 1.2f, -0.20f), glm::vec3(0.0f), glm::vec3(0.06f, 2.0f, 1.9f), partition, glm::vec3(0.16f), 12.0f);

    drawCubeShape(shader, parent, glm::vec3(-0.62f, 0.26f, -1.10f), glm::vec3(0.0f), glm::vec3(0.62f, 0.12f, 0.72f), white, glm::vec3(0.26f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(-0.62f, 0.62f, -1.28f), glm::vec3(0.0f), glm::vec3(0.38f, 0.78f, 0.16f), white, glm::vec3(0.26f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(-0.62f, 0.42f, -0.85f), glm::vec3(0.0f), glm::vec3(0.28f, 0.28f, 0.34f), white, glm::vec3(0.26f), 18.0f);

    drawCubeShape(shader, parent, glm::vec3(0.72f, 0.92f, 0.95f), glm::vec3(0.0f), glm::vec3(0.82f, 0.14f, 0.46f), white, glm::vec3(0.26f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(0.72f, 0.70f, 1.08f), glm::vec3(0.0f), glm::vec3(0.20f, 0.32f, 0.20f), white, glm::vec3(0.26f), 18.0f);
    drawCubeShape(shader, parent, glm::vec3(0.72f, 1.62f, 1.34f), glm::vec3(0.0f), glm::vec3(0.06f, 0.28f, 0.06f), metal, glm::vec3(0.32f), 24.0f);
    drawCubeShape(shader, parent, glm::vec3(0.72f, 2.05f, 1.45f), glm::vec3(0.0f), glm::vec3(0.98f, 1.10f, 0.04f), mirror, glm::vec3(0.34f), 32.0f, glm::vec3(0.02f, 0.03f, 0.05f));
    drawCubeShape(shader, parent, glm::vec3(0.72f, 2.72f, 1.48f), glm::vec3(0.0f), glm::vec3(1.02f, 0.12f, 0.02f), glm::vec3(0.82f, 0.90f, 1.0f), glm::vec3(0.22f), 24.0f, glm::vec3(0.03f, 0.05f, 0.08f));
}

void drawUpperSupportZone(Shader& shader) {
    glm::mat4 upper = makeModel(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    glm::vec3 floor(0.84f, 0.84f, 0.86f);
    glm::vec3 wall(0.88f, 0.89f, 0.92f);
    glm::vec3 ceiling(0.22f, 0.24f, 0.28f);
    glm::vec3 trim(0.92f, 0.93f, 0.95f);
    glm::vec3 supportFill(0.76f, 0.78f, 0.82f);
    glm::vec3 lowWall(0.78f, 0.80f, 0.84f);

    // ===== SIMPLE CONNECTED UPPER LANDING =====

    // Main upper hub
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 0.09f, -3.55f), glm::vec3(0.0f),
        glm::vec3(5.90f, 0.08f, 5.10f), floor, glm::vec3(0.16f), 12.0f);

    // Front strip
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 0.09f, -0.95f), glm::vec3(0.0f),
        glm::vec3(6.10f, 0.08f, 0.80f), floor, glm::vec3(0.16f), 12.0f);

    // Middle support fill
    drawCubeShape(shader, upper, glm::vec3(-0.05f, -0.18f, -2.10f), glm::vec3(0.0f),
        glm::vec3(6.10f, 0.50f, 3.20f), supportFill, glm::vec3(0.12f), 10.0f);

    // Ceiling
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 3.72f, -3.55f), glm::vec3(0.0f),
        glm::vec3(5.90f, 0.04f, 5.10f), ceiling, glm::vec3(0.10f), 8.0f);

    // Front trim beam
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 4.02f, -1.10f), glm::vec3(0.0f),
        glm::vec3(6.00f, 0.08f, 0.16f), trim, glm::vec3(0.16f), 12.0f);

    // Side walls
    drawCubeShape(shader, upper, glm::vec3(-2.95f, 2.0f, -3.55f), glm::vec3(0.0f),
        glm::vec3(0.10f, 4.0f, 5.10f), wall, glm::vec3(0.12f), 10.0f);
    drawCubeShape(shader, upper, glm::vec3(2.85f, 2.0f, -3.55f), glm::vec3(0.0f),
        glm::vec3(0.10f, 4.0f, 5.10f), wall, glm::vec3(0.12f), 10.0f);

    // Back wall
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 2.0f, -5.95f), glm::vec3(0.0f),
        glm::vec3(5.90f, 4.0f, 0.08f), wall, glm::vec3(0.12f), 10.0f);

    // Half walls
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 0.55f, -1.02f), glm::vec3(0.0f),
        glm::vec3(6.00f, 1.00f, 0.12f), lowWall, glm::vec3(0.18f), 14.0f);

    drawCubeShape(shader, upper, glm::vec3(-2.70f, 0.55f, -2.80f), glm::vec3(0.0f),
        glm::vec3(0.12f, 1.00f, 3.50f), lowWall, glm::vec3(0.18f), 14.0f);

    drawCubeShape(shader, upper, glm::vec3(2.60f, 0.55f, -2.80f), glm::vec3(0.0f),
        glm::vec3(0.12f, 1.00f, 3.50f), lowWall, glm::vec3(0.18f), 14.0f);

    // Existing front cap
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 0.12f, -0.55f), glm::vec3(0.0f),
        glm::vec3(6.00f, 0.08f, 0.35f), floor, glm::vec3(0.16f), 12.0f);

    // =====================================================
    // NEW: fill LEFT marked open area
    // =====================================================
    drawCubeShape(shader, upper, glm::vec3(-2.05f, 0.09f, -0.15f), glm::vec3(0.0f),
        glm::vec3(1.60f, 0.08f, 1.35f), floor, glm::vec3(0.16f), 12.0f);

    drawCubeShape(shader, upper, glm::vec3(-2.05f, -0.18f, -0.15f), glm::vec3(0.0f),
        glm::vec3(1.60f, 0.50f, 1.35f), supportFill, glm::vec3(0.12f), 10.0f);

    // =====================================================
    // NEW: fill RIGHT marked open area (stairs-side gap)
    // =====================================================
    drawCubeShape(shader, upper, glm::vec3(2.05f, 0.09f, -0.15f), glm::vec3(0.0f),
        glm::vec3(1.70f, 0.08f, 1.35f), floor, glm::vec3(0.16f), 12.0f);

    drawCubeShape(shader, upper, glm::vec3(2.05f, -0.18f, -0.15f), glm::vec3(0.0f),
        glm::vec3(1.70f, 0.50f, 1.35f), supportFill, glm::vec3(0.12f), 10.0f);

    // Optional top lip to make the front edge look fully connected
    drawCubeShape(shader, upper, glm::vec3(-0.05f, 0.12f, -0.18f), glm::vec3(0.0f),
        glm::vec3(6.00f, 0.08f, 1.05f), floor, glm::vec3(0.16f), 12.0f);

    // Keep washroom
    drawWashroom(shader, makeModel(upper, glm::vec3(-1.55f, 0.0f, -4.10f), glm::vec3(0.0f), glm::vec3(1.0f)));
}

float stableNoise2D(int x, int z) {
    float v = std::sin((x * 127.1f + z * 311.7f) * 0.137f) * 43758.5453f;
    return v - std::floor(v);
}

void drawRain(Shader& shader) {
    if (!rainEnabled) return;

    glm::mat4 I(1.0f);
    glm::vec3 rainColor(0.68f, 0.82f, 0.98f);
    const float fallHeight = 13.0f;

    for (int gx = -30; gx <= 30; gx += 3) {
        for (int gz = -18; gz <= 36; gz += 3) {
            if (gx > -15 && gx < 15 && gz > -9 && gz < 10) continue;

            float n1 = stableNoise2D(gx, gz);
            float n2 = stableNoise2D(gx + 19, gz - 7);
            float n3 = stableNoise2D(gx - 11, gz + 13);

            float x = gx + (n1 - 0.5f) * 1.6f;
            float z = gz + (n2 - 0.5f) * 1.6f;
            float y = 11.5f - std::fmod(sceneClock * 14.0f + n3 * fallHeight, fallHeight);

            drawCubeShape(shader, I, glm::vec3(x, y, z), glm::vec3(18.0f, 0.0f, 6.0f), glm::vec3(0.035f, 0.78f, 0.035f), rainColor, glm::vec3(0.10f), 8.0f, glm::vec3(0.04f, 0.06f, 0.10f));
        }
    }
}

void drawGroundFloorRooms(Shader& shader) {
    drawServerRoom(shader);
    drawLibraryRoom(shader);
    drawReceptionRoom(shader);
    drawCentralAtrium(shader);
}

void drawUpperFloorRooms(Shader& shader) {
    drawUpperLeftClassroom(shader);
    drawUpperRightClassroom(shader);
    drawUpperSupportZone(shader);
}

void drawRoofPavilion(Shader& shader) {
    glm::mat4 I(1.0f);
    glm::vec3 whiteBand(0.92f, 0.93f, 0.95f);
    glm::vec3 darkRoof(0.22f, 0.24f, 0.28f);
    glm::vec3 wood(0.42f, 0.28f, 0.18f);
    glm::vec3 glass(0.64f, 0.78f, 0.90f);
    glm::vec3 greenRoof(0.34f, 0.64f, 0.30f);

    drawCubeShape(shader, I, glm::vec3(-9.0f, 8.18f, -1.1f), glm::vec3(0.0f), glm::vec3(6.4f, 0.12f, 8.0f), greenRoof, glm::vec3(0.08f), 8.0f);
    drawCubeShape(shader, I, glm::vec3(9.0f, 8.18f, -1.1f), glm::vec3(0.0f), glm::vec3(6.4f, 0.12f, 8.0f), greenRoof, glm::vec3(0.08f), 8.0f);

    drawCubeShape(shader, I, glm::vec3(3.2f, 8.35f, -3.7f), glm::vec3(0.0f), glm::vec3(8.8f, 0.12f, 4.8f), glm::vec3(0.76f, 0.77f, 0.80f), glm::vec3(0.14f), 12.0f);
    drawWoodAccentPanel(shader, I, glm::vec3(-0.8f, 9.35f, -3.7f), glm::vec3(7.8f, 2.0f, 0.10f));
    drawCubeShape(shader, I, glm::vec3(5.0f, 9.35f, -0.65f), glm::vec3(0.0f), glm::vec3(6.2f, 2.0f, 0.08f), glass, glm::vec3(0.36f), 40.0f, glm::vec3(0.02f, 0.03f, 0.05f));
    for (int i = 0; i < 6; ++i) {
        float x = 1.9f + i * 1.0f;
        drawCubeShape(shader, I, glm::vec3(x, 9.35f, -0.60f), glm::vec3(0.0f), glm::vec3(0.05f, 2.0f, 0.12f), glm::vec3(0.78f, 0.80f, 0.84f), glm::vec3(0.18f), 18.0f);
    }
    drawCubeShape(shader, I, glm::vec3(4.0f, 10.45f, -3.7f), glm::vec3(0.0f, 0.0f, -8.0f), glm::vec3(8.6f, 0.16f, 5.3f), whiteBand, glm::vec3(0.18f), 14.0f);
    drawCubeShape(shader, I, glm::vec3(-7.4f, 8.28f, -3.6f), glm::vec3(0.0f, 0.0f, -8.0f), glm::vec3(4.4f, 0.10f, 2.2f), glm::vec3(0.10f, 0.22f, 0.46f), glm::vec3(0.18f), 20.0f, glm::vec3(0.02f, 0.03f, 0.06f));

    drawWindowPanel(shader, makeModel(I, glm::vec3(-1.4f, 8.4f, -0.85f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f)), windowAngle);
}

void renderITPark(Shader& shader) {
    drawOutdoorScene(shader);
    drawBuildingEnvelope(shader);
    drawGroundFloorRooms(shader);
    drawUpperFloorRooms(shader);
    drawRoofPavilion(shader);
    drawRain(shader);
}

void configureLighting(Shader& shader, const glm::vec3& viewPos, const glm::vec3& spotPos, const glm::vec3& spotDir) {
    shader.use();
    shader.setVec3("viewPos", viewPos);
    shader.setInt("ambientEnabled", masterLight && ambientOn ? 1 : 0);
    shader.setInt("diffuseEnabled", masterLight && diffuseOn ? 1 : 0);
    shader.setInt("specularEnabled", masterLight && specularOn ? 1 : 0);

    shader.setVec3("dirLight.direction", glm::vec3(-0.35f, -1.0f, -0.22f));
    shader.setVec3("dirLight.ambient", glm::vec3(0.32f, 0.33f, 0.36f));
    shader.setVec3("dirLight.diffuse", glm::vec3(0.66f, 0.64f, 0.62f));
    shader.setVec3("dirLight.specular", glm::vec3(0.28f, 0.28f, 0.26f));
    shader.setInt("dirLight.enabled", masterLight && directionalOn ? 1 : 0);

    glm::vec3 pointAmbient[NR_POINT_LIGHTS] = {
        glm::vec3(0.25f, 0.20f, 0.17f),
        glm::vec3(0.18f, 0.20f, 0.24f),
        glm::vec3(0.08f, 0.12f, 0.22f),
        glm::vec3(0.24f, 0.20f, 0.14f),
        glm::vec3(0.20f, 0.21f, 0.24f),
        glm::vec3(0.08f, 0.18f, 0.24f),
        glm::vec3(0.08f, 0.18f, 0.24f)
    };
    glm::vec3 pointDiffuse[NR_POINT_LIGHTS] = {
        glm::vec3(0.98f, 0.82f, 0.64f),
        glm::vec3(0.90f, 0.92f, 0.96f),
        glm::vec3(0.28f, 0.62f, 1.00f),
        glm::vec3(0.96f, 0.82f, 0.60f),
        glm::vec3(0.92f, 0.94f, 0.98f),
        glm::vec3(0.30f, 0.82f, 1.00f),
        glm::vec3(0.30f, 0.82f, 1.00f)
    };
    glm::vec3 pointSpecular[NR_POINT_LIGHTS] = {
        glm::vec3(1.00f, 0.90f, 0.78f),
        glm::vec3(1.00f, 1.00f, 1.00f),
        glm::vec3(0.62f, 0.90f, 1.00f),
        glm::vec3(1.00f, 0.92f, 0.78f),
        glm::vec3(1.00f, 1.00f, 1.00f),
        glm::vec3(0.70f, 0.94f, 1.00f),
        glm::vec3(0.70f, 0.94f, 1.00f)
    };

    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        string idx = "pointLights[" + to_string(i) + "]";
        shader.setVec3(idx + ".position", pointLightPositions[i]);
        shader.setVec3(idx + ".ambient", pointAmbient[i]);
        shader.setVec3(idx + ".diffuse", pointDiffuse[i]);
        shader.setVec3(idx + ".specular", pointSpecular[i]);
        shader.setFloat(idx + ".k_c", 1.0f);
        shader.setFloat(idx + ".k_l", 0.050f);
        shader.setFloat(idx + ".k_q", 0.010f);
        shader.setInt(idx + ".enabled", masterLight && pointLightsOn && pointLightEnabled[i] ? 1 : 0);
    }

    shader.setVec3("spotLight.position", spotPos);
    shader.setVec3("spotLight.direction", glm::normalize(spotDir));
    shader.setFloat("spotLight.cutOff", std::cos(glm::radians(12.5f)));
    shader.setVec3("spotLight.ambient", glm::vec3(0.10f, 0.10f, 0.10f));
    shader.setVec3("spotLight.diffuse", glm::vec3(0.92f, 0.92f, 0.88f));
    shader.setVec3("spotLight.specular", glm::vec3(1.0f));
    shader.setFloat("spotLight.k_c", 1.0f);
    shader.setFloat("spotLight.k_l", 0.09f);
    shader.setFloat("spotLight.k_q", 0.032f);
    shader.setInt("spotLight.enabled", masterLight && spotLightOn ? 1 : 0);
}

void renderLightCubes(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    (void)shader;
    (void)view;
    (void)projection;
}

void renderView(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& projection,
    const glm::vec3& eye, const glm::vec3& spotPos, const glm::vec3& spotDir) {
    lightingShader.use();
    lightingShader.setMat4("view", view);
    lightingShader.setMat4("projection", projection);
    lightingShader.setInt("textureMode", textureMode);
    lightingShader.setInt("shadingMode", useGouraudShading ? 1 : 0);
    lightingShader.setInt("useTexture", 0);   // default off; textured draws override per-object
    lightingShader.setInt("texture1", 0);
    lightingShader.setFloat("texRepeat", 1.0f);
    configureLighting(lightingShader, eye, spotPos, spotDir);
    renderITPark(lightingShader);
    renderLightCubes(lampShader, view, projection);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.MoveForward(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.MoveBackward(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.MoveLeft(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.MoveRight(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.MoveDown(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.MoveUp(deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        camera.AddYaw(-camera.RotationSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        camera.AddYaw(camera.RotationSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        camera.AddPitch(camera.RotationSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        camera.AddPitch(-camera.RotationSpeed * deltaTime);
    }
}

void toggleNearestDoor() {
    struct DoorHotspot {
        glm::vec3 position;
        bool* state;
    };

    DoorHotspot hotspots[] = {
        { glm::vec3(15.00f, 1.00f, 6.55f), &doorOpening },
        { glm::vec3(-10.40f, 1.00f, 8.20f), &serverDoorOpening },
        { glm::vec3(-1.15f, 1.00f, 8.00f), &libraryDoorOpening },
        { glm::vec3(10.40f, 1.00f, 8.20f), &receptionDoorOpening },
        { glm::vec3(-6.00f, 5.00f, 6.92f), &upperLeftDoorOpening },
        { glm::vec3(6.05f, 5.00f, 6.92f), &upperRightDoorOpening },
        { glm::vec3(-1.55f, 5.00f, -1.05f), &washroomDoorOpening },
        { glm::vec3(1.25f, 5.00f, -1.05f), &innovationDoorOpening }
    };

    int bestIndex = -1;
    float bestDist2 = 36.0f;
    for (int i = 0; i < static_cast<int>(sizeof(hotspots) / sizeof(hotspots[0])); ++i) {
        glm::vec3 d = camera.Position - hotspots[i].position;
        float dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
        if (dist2 < bestDist2) {
            bestDist2 = dist2;
            bestIndex = i;
        }
    }

    if (bestIndex >= 0) {
        *hotspots[bestIndex].state = !*hotspots[bestIndex].state;
    }
}

void updateAnimations() {
    doorAngle = animateAngle(doorAngle, doorOpening, 78.0f, 3.0f);
    serverDoorAngle = animateAngle(serverDoorAngle, serverDoorOpening, 82.0f, 3.4f);
    libraryDoorAngle = animateAngle(libraryDoorAngle, libraryDoorOpening, 82.0f, 3.4f);
    receptionDoorAngle = animateAngle(receptionDoorAngle, receptionDoorOpening, 82.0f, 3.4f);
    upperLeftDoorAngle = animateAngle(upperLeftDoorAngle, upperLeftDoorOpening, 92.0f, 3.2f);
    upperRightDoorAngle = animateAngle(upperRightDoorAngle, upperRightDoorOpening, 92.0f, 3.2f);
    washroomDoorAngle = animateAngle(washroomDoorAngle, washroomDoorOpening, 86.0f, 3.0f);
    innovationDoorAngle = animateAngle(innovationDoorAngle, innovationDoorOpening, 86.0f, 3.0f);
    windowAngle = animateAngle(windowAngle, windowOpening, 65.0f, 3.0f);
    sceneClock += deltaTime;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_O || key == GLFW_KEY_SPACE) {
        toggleNearestDoor();
        cout << "[Door] Toggled nearest door" << endl;
    }
    if (key == GLFW_KEY_P) {
        windowOpening = !windowOpening;
        cout << "[Roof Window] " << (windowOpening ? "Open" : "Closed") << endl;
    }
    if (key == GLFW_KEY_T) {
        rainEnabled = !rainEnabled;
        cout << "[Rain] " << (rainEnabled ? "On" : "Off") << endl;
    }

    if (key == GLFW_KEY_0) {
        masterLight = !masterLight;
        cout << "[Master Light] " << (masterLight ? "On" : "Off") << endl;
    }
    if (key == GLFW_KEY_1) {
        directionalOn = !directionalOn;
        cout << "[Directional Light] " << (directionalOn ? "On" : "Off") << endl;
    }
    if (key == GLFW_KEY_2) {
        pointLightsOn = !pointLightsOn;
        cout << "[Point Lights] " << (pointLightsOn ? "On" : "Off") << " (all point lights)" << endl;
    }
    if (key == GLFW_KEY_3) {
        spotLightOn = !spotLightOn;
        cout << "[Spot Light] " << (spotLightOn ? "On" : "Off") << endl;
    }
    if (key == GLFW_KEY_4) {
        emissiveOn = !emissiveOn;
        cout << "[Emissive Light] " << (emissiveOn ? "On" : "Off") << endl;
    }
    if (key == GLFW_KEY_5) {
        ambientOn = !ambientOn;
        cout << "[Ambient] " << (ambientOn ? "On" : "Off") << endl;
    }
    if (key == GLFW_KEY_6) {
        diffuseOn = !diffuseOn;
        cout << "[Diffuse] " << (diffuseOn ? "On" : "Off") << endl;
    }
    if (key == GLFW_KEY_7) {
        specularOn = !specularOn;
        cout << "[Specular] " << (specularOn ? "On" : "Off") << endl;
    }

    if (key == GLFW_KEY_G) {
        useGouraudShading = !useGouraudShading;
        cout << "[Shading] " << (useGouraudShading ? "Gouraud" : "Phong") << endl;
    }
    if (key == GLFW_KEY_V) {
        textureMode = (textureMode + 1) % 3;
        const char* modes[] = { "Material color only", "Texture only", "Blended (texture x material)" };
        std::cout << "[Texture Mode] " << modes[textureMode] << std::endl;
    }
    if (key == GLFW_KEY_M) {
        printCameraState();
    }
    if (key == GLFW_KEY_F1) {
        printControls();
    }

    if (key == GLFW_KEY_HOME) {
        camera.reset(defaultCameraPos, defaultLookTarget);
        camera.setFloorHeight(sampleFloorHeight(camera.Position), 1.0f);
        lastSafeCameraPos = camera.Position;
        printCameraState();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    SCR_WIDTH = static_cast<unsigned int>(width);
    SCR_HEIGHT = static_cast<unsigned int>(height);
    glViewport(0, 0, width, height);
}

float sampleFloorHeight(const glm::vec3& pos) {
    float x = pos.x;
    float z = pos.z;

    bool onEntranceSteps = (x > -4.5f && x < 3.5f && z > 9.3f && z < 11.8f);
    if (onEntranceSteps) {
        float t = (11.7f - z) / 2.4f;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return 0.38f * t;
    }

    bool onAtriumStairs = (x > 1.1f && x < 4.2f && z < 7.1f && z > 1.0f);
    if (onAtriumStairs) {
        float t = (7.0f - z) / 6.0f;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return 4.0f * t;
    }

    bool onUpperHub = (x > -2.35f && x < 4.35f && z > -1.25f && z < 2.55f);
    bool onUpperSpine = (x > -0.65f && x < 2.70f && z > 1.90f && z < 6.60f);
    bool onUpperBridge = (x > -6.70f && x < 6.90f && z > 5.60f && z < 8.35f);
    bool inUpperLeftClass = (x > -14.10f && x < -5.75f && z > 1.05f && z < 8.95f);
    bool inUpperRightClass = (x > 5.75f && x < 14.60f && z > 1.05f && z < 8.95f);
    bool inUpperSupport = (x > -3.05f && x < 2.95f && z > -6.00f && z < -0.80f);

    if ((onUpperHub || onUpperSpine || onUpperBridge || inUpperLeftClass || inUpperRightClass || inUpperSupport) && pos.y > 2.30f) {
        return 4.0f;
    }

    return 0.0f;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    (void)window;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void applyCameraWalkConstraints() {
    camera.Position.x = glm::clamp(camera.Position.x, -28.0f, 24.0f);
    camera.Position.z = glm::clamp(camera.Position.z, -7.0f, 34.0f);

    if (camera.Position.x > 3.2f && camera.Position.x < 14.2f &&
        camera.Position.z > 14.6f && camera.Position.z < 21.5f &&
        camera.Position.y < 2.4f) {
        float left = camera.Position.x - 3.2f;
        float right = 14.2f - camera.Position.x;
        float nearZ = camera.Position.z - 14.6f;
        float farZ = 21.5f - camera.Position.z;
        float minEdge = left;
        int edge = 0;
        if (right < minEdge) { minEdge = right; edge = 1; }
        if (nearZ < minEdge) { minEdge = nearZ; edge = 2; }
        if (farZ < minEdge) { minEdge = farZ; edge = 3; }
        if (edge == 0) camera.Position.x = 3.05f;
        if (edge == 1) camera.Position.x = 14.35f;
        if (edge == 2) camera.Position.z = 14.42f;
        if (edge == 3) camera.Position.z = 21.62f;
    }

    if (camera.Position.y > 3.20f) {
        // Keep the user safely on the upper walkable slabs / rooms without snapping across rooms.
        bool inHub = (camera.Position.x > -2.35f && camera.Position.x < 4.35f &&
            camera.Position.z > -1.25f && camera.Position.z < 2.55f);
        bool inSpine = (camera.Position.x > -0.65f && camera.Position.x < 2.70f &&
            camera.Position.z > 1.90f && camera.Position.z < 6.60f);
        bool inBridge = (camera.Position.x > -6.70f && camera.Position.x < 6.90f &&
            camera.Position.z > 5.60f && camera.Position.z < 8.35f);
        bool inUpperLeft = (camera.Position.x > -14.10f && camera.Position.x < -5.75f &&
            camera.Position.z > 1.05f && camera.Position.z < 8.95f);
        bool inUpperRight = (camera.Position.x > 5.75f && camera.Position.x < 14.60f &&
            camera.Position.z > 1.05f && camera.Position.z < 8.95f);
        bool inUpperSupport = (camera.Position.x > -3.05f && camera.Position.x < 2.95f &&
            camera.Position.z > -6.00f && camera.Position.z < -0.80f);

        if (inHub) {
            camera.Position.x = glm::clamp(camera.Position.x, -2.35f, 4.35f);
            camera.Position.z = glm::clamp(camera.Position.z, -1.25f, 2.55f);
        }
        else if (inSpine) {
            camera.Position.x = glm::clamp(camera.Position.x, -0.65f, 2.70f);
            camera.Position.z = glm::clamp(camera.Position.z, 1.90f, 6.60f);
        }
        else if (inBridge) {
            camera.Position.x = glm::clamp(camera.Position.x, -6.70f, 6.90f);
            camera.Position.z = glm::clamp(camera.Position.z, 5.60f, 8.30f);
        }
        else if (inUpperLeft) {
            camera.Position.x = glm::clamp(camera.Position.x, -14.10f, -5.75f);
            camera.Position.z = glm::clamp(camera.Position.z, 1.05f, 8.95f);
        }
        else if (inUpperRight) {
            camera.Position.x = glm::clamp(camera.Position.x, 5.75f, 14.60f);
            camera.Position.z = glm::clamp(camera.Position.z, 1.05f, 8.95f);
        }
        else if (inUpperSupport) {
            camera.Position.x = glm::clamp(camera.Position.x, -3.05f, 2.95f);
            camera.Position.z = glm::clamp(camera.Position.z, -6.00f, -0.80f);
        }
    }

    float floorY = sampleFloorHeight(camera.Position);
    bool onAtriumStairs = (camera.Position.x > 1.1f && camera.Position.x < 4.2f &&
        camera.Position.z < 7.1f && camera.Position.z > 1.0f);

    // If the camera slips off an upper slab, restore the last safe position instead of dropping down.
    if (camera.Position.y > 3.20f && floorY < 3.50f && !onAtriumStairs) {
        camera.Position.x = lastSafeCameraPos.x;
        camera.Position.z = lastSafeCameraPos.z;
        floorY = sampleFloorHeight(camera.Position);
    }

    camera.setFloorHeight(floorY, deltaTime);
    lastSafeCameraPos = camera.Position;
}

