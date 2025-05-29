//
//  sphere_scene.cpp
//  Rasterizer based on the provided sphere_scene.c
//
//

#include <stdio.h>
#include <math.h>
#include <glm/vec3.hpp> // Include GLM for vec3 type
#include "sphere_scene.h"

// Global variables
int         gNumVertices = 0;        // Number of 3D vertices.
int         gNumTriangles = 0;        // Number of triangles.
int* gIndexBuffer = nullptr;  // Vertex indices for the triangles.
glm::vec3* gVertexBuffer = nullptr;  // Vertex coordinates array (using glm::vec3)

// Function to create the sphere geometry
void create_scene()
{
    // Define sphere parameters (resolution)
    int width = 32; // Number of divisions around the equator
    int height = 16; // Number of divisions from pole to pole

    float theta, phi;
    int t; // Vertex counter

    // Calculate total number of vertices and triangles
    gNumVertices = (height - 2) * width + 2; // vertices in the middle + 2 poles
    // 수정: 삼각형 개수 계산 수정 (HW6에서 사용된 것과 일치하도록)
    gNumTriangles = (height - 3) * (width - 1) * 2 + (width - 1) * 2;

    // 1. Allocate the vertex buffer array
    gVertexBuffer = new glm::vec3[gNumVertices];
    if (!gVertexBuffer) {
        fprintf(stderr, "Error allocating memory for vertex buffer\n");
        return;
    }

    // Allocate the index buffer array
    gIndexBuffer = new int[3 * gNumTriangles];
    if (!gIndexBuffer) {
        fprintf(stderr, "Error allocating memory for index buffer\n");
        delete[] gVertexBuffer;
        gVertexBuffer = nullptr;
        return;
    }


    t = 0;

    for (int j = 1; j < height - 1; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
            theta = (float)j / (height - 1) * M_PI;
            phi = (float)i / (width - 1) * M_PI * 2;

            float   x = sinf(theta) * cosf(phi);
            float   y = cosf(theta);
            float   z = -sinf(theta) * sinf(phi);

            gVertexBuffer[t] = glm::vec3(x, y, z);
            t++;
        }
    }

    gVertexBuffer[t] = glm::vec3(0.0f, 1.0f, 0.0f);
    t++;

    gVertexBuffer[t] = glm::vec3(0.0f, -1.0f, 0.0f);
    t++;

    t = 0;

    // 인덱스 버퍼 생성 수정 (HW6에서 사용된 것과 일치하도록)
    for (int j = 0; j < height - 3; ++j)
    {
        for (int i = 0; i < width - 1; ++i)
        {
            gIndexBuffer[t++] = j * width + i;
            gIndexBuffer[t++] = (j + 1) * width + (i + 1);
            gIndexBuffer[t++] = j * width + (i + 1);

            gIndexBuffer[t++] = j * width + i;
            gIndexBuffer[t++] = (j + 1) * width + i;
            gIndexBuffer[t++] = (j + 1) * width + (i + 1);
        }
    }

    int northPoleIndex = (height - 2) * width;
    int southPoleIndex = northPoleIndex + 1;

    for (int i = 0; i < width - 1; ++i)
    {
        gIndexBuffer[t++] = northPoleIndex;
        gIndexBuffer[t++] = i;
        gIndexBuffer[t++] = i + 1;
    }

    int bottomStripStart = (height - 3) * width;
    for (int i = 0; i < width - 1; ++i)
    {
        gIndexBuffer[t++] = southPoleIndex;
        gIndexBuffer[t++] = bottomStripStart + (i + 1);
        gIndexBuffer[t++] = bottomStripStart + i;
    }
}

// Function to delete the sphere geometry and free memory
void delete_scene()
{
    if (gIndexBuffer) {
        delete[] gIndexBuffer;
        gIndexBuffer = nullptr;
        gNumTriangles = 0;
    }

    if (gVertexBuffer) {
        delete[] gVertexBuffer;
        gVertexBuffer = nullptr;
        gNumVertices = 0;
    }
}