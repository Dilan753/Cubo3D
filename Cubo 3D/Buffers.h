#pragma once
#ifndef BUFFERS_H
#define BUFFERS_H
#include <glad/glad.h>
#include <cstddef>

// -----------------------------------------------------------------
// Clase Mesh
// Encapsula un VAO, VBO y EBO. Guarda internamente como esta
// configurada la geometria y expone solo dos acciones desde afuera:
// crearla (constructor) y dibujarla (draw).
//
// Cada vertice ahora tiene 8 floats: posicion(3) + color(3) + coordenadas
// de textura / UV (2). El orden dentro de cada vertice en el arreglo
// de floats debe ser: x, y, z, r, g, b, u, v.
// -----------------------------------------------------------------
class Mesh
{
public:
    // vertices: arreglo de floats (posicion + color + UV, 8 floats por vertice)
    // verticesSize: tamaño en bytes del arreglo de vertices (usa sizeof(vertices))
    // indices: arreglo de indices para el EBO
    // indexCount: cuantos indices hay (la cantidad real, no bytes)
    Mesh(const float* vertices, size_t verticesSize,
        const unsigned int* indices, unsigned int indexCount)
        : indexCount(indexCount)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        // Cada vertice: posicion(3) + color(3) + texCoord(2) = 8 floats
        const int stride = 8 * sizeof(float);

        // posicion -> location 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        // color -> location 1
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // coordenadas de textura (U, V) -> location 2
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Dibuja la malla. Asume que ya hiciste glUseProgram del shader correcto
    // (y glBindTexture si aplica) antes de llamar esto.
    void draw() const
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Libera los buffers de la GPU. Llamalo al cerrar el programa.
    void destroy()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int indexCount = 0;
};

#endif