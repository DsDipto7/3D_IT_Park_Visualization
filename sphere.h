#ifndef SPHERE_H
#define SPHERE_H

#include <algorithm>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class Sphere {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;

    Sphere(
        float radius = 1.0f,
        int sectorCount = 18,
        int stackCount = 18,
        glm::vec3 amb = glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3 diff = glm::vec3(0.8f, 0.8f, 0.8f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3 emis = glm::vec3(0.0f),
        float shiny = 32.0f)
        : radius(radius),
        sectorCount(std::max(3, sectorCount)),
        stackCount(std::max(2, stackCount)),
        ambient(amb),
        diffuse(diff),
        specular(spec),
        emissive(emis),
        shininess(shiny),
        verticesStride(6 * sizeof(float)),
        sphereVAO(0) {
        buildCoordinatesAndIndices();
        buildVertices();
        setupBuffers();
    }

    ~Sphere() {}

    unsigned int getIndexCount() const {
        return static_cast<unsigned int>(indices.size());
    }

    void drawSphere(Shader& shader, const glm::mat4& model) const {
        shader.use();
        shader.setVec3("material.ambient", ambient);
        shader.setVec3("material.diffuse", diffuse);
        shader.setVec3("material.specular", specular);
        shader.setVec3("material.emissive", emissive);
        shader.setFloat("material.shininess", shininess);
        shader.setMat4("model", model);

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }

private:
    static constexpr float PI = 3.14159265359f;

    unsigned int sphereVAO;
    float radius;
    int sectorCount;
    int stackCount;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
    std::vector<float> coordinates;
    int verticesStride;

    void setupBuffers() {
        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        unsigned int sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        unsigned int sphereEBO;
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, verticesStride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, verticesStride, (void*)(sizeof(float) * 3));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void buildCoordinatesAndIndices() {
        coordinates.clear();
        normals.clear();
        indices.clear();

        float x, y, z, xz;
        float nx, ny, nz;
        float lengthInv = 1.0f / radius;

        float sectorStep = 2.0f * PI / static_cast<float>(sectorCount);
        float stackStep = PI / static_cast<float>(stackCount);

        for (int i = 0; i <= stackCount; ++i) {
            float stackAngle = PI / 2.0f - i * stackStep;
            xz = radius * std::cos(stackAngle);
            y = radius * std::sin(stackAngle);

            for (int j = 0; j <= sectorCount; ++j) {
                float sectorAngle = j * sectorStep;
                x = xz * std::cos(sectorAngle);
                z = xz * std::sin(sectorAngle);

                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }

        for (int i = 0; i < stackCount; ++i) {
            int k1 = i * (sectorCount + 1);
            int k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stackCount - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }

    void buildVertices() {
        vertices.clear();
        for (size_t i = 0; i < coordinates.size(); i += 3) {
            vertices.push_back(coordinates[i + 0]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);
            vertices.push_back(normals[i + 0]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);
        }
    }
};

#endif
