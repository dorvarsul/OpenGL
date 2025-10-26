
// noise.h - Procedural Noise Generation Header
// Declares functions for Perlin Noise, Simplex Noise, and Diamond-Square
// algorithms

#ifndef NOISE_H
#define NOISE_H

#include <vector>

// =============================================================================
// PERLIN NOISE
// =============================================================================

/**
 * Generate 3D Perlin noise value at given coordinates
 * @param x X coordinate
 * @param y Y coordinate
 * @param z Z coordinate (use 0.0 for 2D noise)
 * @param seed Random seed for noise generation
 * @return Noise value in range [0, 1]
 */
double generatePerlinNoise(double x, double y, double z = 0.0,
                           unsigned int seed = 0);

/**
 * Generate multi-octave Perlin noise (Fractional Brownian Motion)
 * @param x X coordinate
 * @param y Y coordinate
 * @param octaves Number of noise layers to combine (typically 4-8)
 * @param persistence Amplitude multiplier for each octave (typically 0.5)
 * @param lacunarity Frequency multiplier for each octave (typically 2.0)
 * @param seed Random seed for noise generation
 * @return Noise value in range [0, 1]
 */
double generatePerlinFBM(double x, double y, int octaves = 4,
                         double persistence = 0.5, double lacunarity = 2.0,
                         unsigned int seed = 0);

// =============================================================================
// SIMPLEX NOISE
// =============================================================================

/**
 * Generate 2D Simplex noise value at given coordinates
 * More efficient than Perlin noise, especially in higher dimensions
 * @param x X coordinate
 * @param y Y coordinate
 * @param seed Random seed for noise generation
 * @return Noise value in range [0, 1]
 */
double generateSimplexNoise(double x, double y, unsigned int seed = 0);

/**
 * Generate multi-octave Simplex noise (Fractional Brownian Motion)
 * @param x X coordinate
 * @param y Y coordinate
 * @param octaves Number of noise layers to combine (typically 4-8)
 * @param persistence Amplitude multiplier for each octave (typically 0.5)
 * @param lacunarity Frequency multiplier for each octave (typically 2.0)
 * @param seed Random seed for noise generation
 * @return Noise value in range [0, 1]
 */
double generateSimplexFBM(double x, double y, int octaves = 4,
                          double persistence = 0.5, double lacunarity = 2.0,
                          unsigned int seed = 0);

// =============================================================================
// DIAMOND-SQUARE ALGORITHM
// =============================================================================

/**
 * Generate a heightmap using the Diamond-Square algorithm
 * @param size Size of heightmap (must be 2^n + 1, e.g., 129, 257, 513, 1025)
 * @param roughness Controls terrain roughness (0.0 = smooth, 1.0 = rough)
 * @param seed Random seed for generation
 * @param wrap Whether to wrap edges for seamless tiling
 * @return 2D vector of height values in range [0, 1]
 */
std::vector<std::vector<float>>
generateDiamondSquareNoise(int size, float roughness = 0.5f,
                           unsigned int seed = 0, bool wrap = false);

#endif // NOISE_H
