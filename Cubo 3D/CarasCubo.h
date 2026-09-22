#pragma once
#ifndef CARASCUBO_H
#define CARASCUBO_H
#include <glm/glm.hpp>

// -----------------------------------------------------------------
// Estructura que describe como se arma la matriz de modelo de UNA
// cara del cubo a partir del cuadrado base: traslacion desde el
// centro, y una rotacion opcional (angulo en grados + eje).
//
// Si una cara no necesita rotacion (frente/atras), angulo = 0.0f
// y el eje puede quedar en cualquier vector, no se usa.
// -----------------------------------------------------------------
struct CaraCubo
{
    glm::vec3 traslacion;
    float angulo; // en grados
    glm::vec3 eje;
};

// -----------------------------------------------------------------
// Las 6 caras del cubo.
//
// Frente/atras: sin rotacion, solo se trasladan en Z.
//
// Arriba/abajo: rotan sobre el eje X. IMPORTANTE: usan signos
// OPUESTOS (-90 / +90) entre si. Esto es a proposito: la normal de
// una cara depende de la rotacion, no de la traslacion, entonces si
// ambas usaran el mismo angulo terminarian con la MISMA normal en
// vez de normales opuestas (una hacia +Y, otra hacia -Y). Con el
// mismo angulo, el winding de una de las dos queda invertido y esa
// cara "mira hacia adentro" del cubo.
//
// Izquierda/derecha: rotan sobre el eje Y, con signos opuestos por
// la misma razon, y ya venian correctas desde el codigo original.
// -----------------------------------------------------------------
static const CaraCubo carasCubo[] = {
    // Frente
    { glm::vec3(0.0f, 0.0f,  0.3f),   0.0f, glm::vec3(0.0f, 0.0f, 1.0f) },
    // Atras
    { glm::vec3(0.0f, 0.0f, -0.3f),   0.0f, glm::vec3(0.0f, 0.0f, 1.0f) },
    // Arriba
    { glm::vec3(0.0f, 0.3f,  0.0f), -90.0f, glm::vec3(1.0f, 0.0f, 0.0f) },
    // Abajo
    { glm::vec3(0.0f, -0.3f, 0.0f),  90.0f, glm::vec3(1.0f, 0.0f, 0.0f) },
    // Derecha
    { glm::vec3(0.3f, 0.0f,  0.0f),  90.0f, glm::vec3(0.0f, 1.0f, 0.0f) },
    // Izquierda
    { glm::vec3(-0.3f, 0.0f, 0.0f), -90.0f, glm::vec3(0.0f, 1.0f, 0.0f) },
};

static const unsigned int CANTIDAD_CARAS_CUBO = 6;

#endif