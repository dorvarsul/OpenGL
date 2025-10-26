
// noise.cpp - Procedural Noise Generation Functions
// Implements Perlin Noise, Simplex Noise, and Diamond-Square algorithms
// Author: Generated for OpenGL Terrain Generation
// Date: 2025

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

// =============================================================================
// PERLIN NOISE IMPLEMENTATION
// Based on Ken Perlin's improved noise function (2002)
// =============================================================================

class PerlinNoise {
private:
  // Permutation table
  std::vector<int> p;

  // Fade function as defined by Ken Perlin
  // Improves distribution: 6t^5 - 15t^4 + 10t^3
  double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }

  // Linear interpolation
  double lerp(double t, double a, double b) { return a + t * (b - a); }

  // Gradient function
  double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

public:
  // Constructor with seed
  PerlinNoise(unsigned int seed = 0) {
    p.resize(256);

    // Fill p with values from 0 to 255
    std::iota(p.begin(), p.end(), 0);

    // Shuffle using the seed
    std::default_random_engine engine(seed);
    std::shuffle(p.begin(), p.end(), engine);

    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());
  }

  // 3D Perlin noise (can be used for 2D by passing z=0)
  double noise(double x, double y, double z) {
    // Find unit cube that contains point
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    // Find relative x, y, z of point in cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute fade curves for x, y, z
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add blended results from 8 corners of cube
    double res = lerp(
        w,
        lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)),
             lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))),
        lerp(v,
             lerp(u, grad(p[AA + 1], x, y, z - 1),
                  grad(p[BA + 1], x - 1, y, z - 1)),
             lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                  grad(p[BB + 1], x - 1, y - 1, z - 1))));

    return (res + 1.0) / 2.0; // Normalize to [0, 1]
  }

  // 2D Perlin noise convenience function
  double noise2D(double x, double y) { return noise(x, y, 0.0); }
};

// Standalone function for Perlin noise generation
double generatePerlinNoise(double x, double y, double z = 0.0,
                           unsigned int seed = 0) {
  static PerlinNoise perlin(seed);
  return perlin.noise(x, y, z);
}

// =============================================================================
// SIMPLEX NOISE IMPLEMENTATION
// Based on Stefan Gustavson's implementation (2012)
// More efficient than Perlin noise, especially in higher dimensions
// =============================================================================

class SimplexNoise {
private:
  // Permutation table
  std::vector<int> perm;

  // Gradients for 2D noise
  static const int grad3[12][3];

  // Skewing and unskewing factors for 2D
  const double F2 = 0.5 * (sqrt(3.0) - 1.0);
  const double G2 = (3.0 - sqrt(3.0)) / 6.0;

  // Fast floor
  int fastfloor(double x) {
    int xi = (int)x;
    return x < xi ? xi - 1 : xi;
  }

  double dot(const int g[], double x, double y) { return g[0] * x + g[1] * y; }

public:
  SimplexNoise(unsigned int seed = 0) {
    perm.resize(512);

    std::vector<int> p(256);
    std::iota(p.begin(), p.end(), 0);

    std::default_random_engine engine(seed);
    std::shuffle(p.begin(), p.end(), engine);

    // Fill perm array with doubled permutation
    for (int i = 0; i < 512; i++) {
      perm[i] = p[i & 255];
    }
  }

  // 2D Simplex noise
  double noise2D(double xin, double yin) {
    double n0, n1, n2; // Noise contributions from the three corners

    // Skew the input space to determine which simplex cell we're in
    double s = (xin + yin) * F2;
    int i = fastfloor(xin + s);
    int j = fastfloor(yin + s);

    double t = (i + j) * G2;
    double X0 = i - t; // Unskew the cell origin back to (x,y) space
    double Y0 = j - t;
    double x0 = xin - X0; // The x,y distances from the cell origin
    double y0 = yin - Y0;

    // Determine which simplex we are in
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) {
      i1 = 1;
      j1 = 0;
    } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else {
      i1 = 0;
      j1 = 1;
    } // upper triangle, YX order: (0,0)->(0,1)->(1,1)

    // Offsets for middle corner in (x,y) unskewed coords
    double x1 = x0 - i1 + G2;
    double y1 = y0 - j1 + G2;
    // Offsets for last corner in (x,y) unskewed coords
    double x2 = x0 - 1.0 + 2.0 * G2;
    double y2 = y0 - 1.0 + 2.0 * G2;

    // Work out the hashed gradient indices of the three simplex corners
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii + perm[jj]] % 12;
    int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
    int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

    // Calculate the contribution from the three corners
    double t0 = 0.5 - x0 * x0 - y0 * y0;
    if (t0 < 0)
      n0 = 0.0;
    else {
      t0 *= t0;
      n0 = t0 * t0 * dot(grad3[gi0], x0, y0);
    }

    double t1 = 0.5 - x1 * x1 - y1 * y1;
    if (t1 < 0)
      n1 = 0.0;
    else {
      t1 *= t1;
      n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }

    double t2 = 0.5 - x2 * x2 - y2 * y2;
    if (t2 < 0)
      n2 = 0.0;
    else {
      t2 *= t2;
      n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }

    // Add contributions from each corner and scale to [0,1]
    return (70.0 * (n0 + n1 + n2) + 1.0) / 2.0;
  }
};

// Gradient table for Simplex noise
const int SimplexNoise::grad3[12][3] = {
    {1, 1, 0},  {-1, 1, 0},  {1, -1, 0}, {-1, -1, 0}, {1, 0, 1},  {-1, 0, 1},
    {1, 0, -1}, {-1, 0, -1}, {0, 1, 1},  {0, -1, 1},  {0, 1, -1}, {0, -1, -1}};

// Standalone function for Simplex noise generation
double generateSimplexNoise(double x, double y, unsigned int seed = 0) {
  static SimplexNoise simplex(seed);
  return simplex.noise2D(x, y);
}

// =============================================================================
// DIAMOND-SQUARE ALGORITHM IMPLEMENTATION
// Generates heightmap using fractal midpoint displacement
// =============================================================================

class DiamondSquare {
private:
  std::vector<std::vector<float>> heightmap;
  int size;
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist;

  // Get height at position, wrapping edges for seamless tiling (optional)
  float getHeight(int x, int y, bool wrap = false) {
    if (wrap) {
      x = (x + size) % size;
      y = (y + size) % size;
    }

    if (x < 0 || x >= size || y < 0 || y >= size) {
      return 0.0f; // Return 0 for out of bounds
    }

    return heightmap[x][y];
  }

  // Set height at position
  void setHeight(int x, int y, float value) {
    if (x >= 0 && x < size && y >= 0 && y < size) {
      heightmap[x][y] = value;
    }
  }

  // Diamond step: calculate center of square
  void diamondStep(int x, int y, int stepSize, float randomRange) {
    float avg = (getHeight(x - stepSize, y - stepSize) +
                 getHeight(x + stepSize, y - stepSize) +
                 getHeight(x - stepSize, y + stepSize) +
                 getHeight(x + stepSize, y + stepSize)) /
                4.0f;

    float randomValue = dist(rng) * randomRange * 2.0f - randomRange;
    setHeight(x, y, avg + randomValue);
  }

  // Square step: calculate center of diamond
  void squareStep(int x, int y, int stepSize, float randomRange,
                  bool wrap = false) {
    int count = 0;
    float sum = 0.0f;

    // Average the four diamond corners
    if (wrap || (y - stepSize >= 0)) {
      sum += getHeight(x, y - stepSize, wrap);
      count++;
    }
    if (wrap || (x + stepSize < size)) {
      sum += getHeight(x + stepSize, y, wrap);
      count++;
    }
    if (wrap || (y + stepSize < size)) {
      sum += getHeight(x, y + stepSize, wrap);
      count++;
    }
    if (wrap || (x - stepSize >= 0)) {
      sum += getHeight(x - stepSize, y, wrap);
      count++;
    }

    float avg = sum / (float)count;
    float randomValue = dist(rng) * randomRange * 2.0f - randomRange;
    setHeight(x, y, avg + randomValue);
  }

public:
  // Constructor: size must be 2^n + 1 (e.g., 129, 257, 513, 1025)
  DiamondSquare(int mapSize, unsigned int seed = 0)
      : size(mapSize), rng(seed), dist(-1.0f, 1.0f) {
    heightmap.resize(size, std::vector<float>(size, 0.0f));
  }

  // Generate heightmap using diamond-square algorithm
  void generate(float roughness = 0.5f, bool wrap = false) {
    // Initialize corners with random values
    heightmap[0][0] = dist(rng);
    heightmap[size - 1][0] = dist(rng);
    heightmap[0][size - 1] = dist(rng);
    heightmap[size - 1][size - 1] = dist(rng);

    float randomRange = 1.0f;

    // Iterate through grid at decreasing step sizes
    for (int stepSize = size - 1; stepSize > 1; stepSize /= 2) {
      int halfStep = stepSize / 2;

      // Diamond step
      for (int y = halfStep; y < size; y += stepSize) {
        for (int x = halfStep; x < size; x += stepSize) {
          diamondStep(x, y, halfStep, randomRange);
        }
      }

      // Square step
      for (int y = 0; y < size; y += halfStep) {
        for (int x = (y + halfStep) % stepSize; x < size; x += stepSize) {
          squareStep(x, y, halfStep, randomRange, wrap);
        }
      }

      // Reduce random range for next iteration (roughness control)
      randomRange *= pow(2.0f, -roughness);
    }

    // Normalize heightmap to [0, 1] range
    normalize();
  }

  // Normalize heightmap values to [0, 1]
  void normalize() {
    float minVal = heightmap[0][0];
    float maxVal = heightmap[0][0];

    // Find min and max
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        minVal = std::min(minVal, heightmap[x][y]);
        maxVal = std::max(maxVal, heightmap[x][y]);
      }
    }

    float range = maxVal - minVal;
    if (range > 0.0001f) {
      for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
          heightmap[x][y] = (heightmap[x][y] - minVal) / range;
        }
      }
    }
  }

  // Get height at specific coordinates
  float getHeightAt(int x, int y) { return getHeight(x, y); }

  // Get the heightmap
  const std::vector<std::vector<float>> &getHeightmap() const {
    return heightmap;
  }

  int getSize() const { return size; }
};

// Standalone function for Diamond-Square generation
std::vector<std::vector<float>>
generateDiamondSquareNoise(int size, float roughness = 0.5f,
                           unsigned int seed = 0, bool wrap = false) {
  DiamondSquare ds(size, seed);
  ds.generate(roughness, wrap);
  return ds.getHeightmap();
}

// =============================================================================
// UTILITY FUNCTIONS FOR MULTI-OCTAVE NOISE (FBM - Fractal Brownian Motion)
// =============================================================================

// Generate multi-octave Perlin noise
double generatePerlinFBM(double x, double y, int octaves = 4,
                         double persistence = 0.5, double lacunarity = 2.0,
                         unsigned int seed = 0) {
  PerlinNoise perlin(seed);
  double total = 0.0;
  double frequency = 1.0;
  double amplitude = 1.0;
  double maxValue = 0.0;

  for (int i = 0; i < octaves; i++) {
    total += perlin.noise2D(x * frequency, y * frequency) * amplitude;
    maxValue += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }

  return total / maxValue;
}

// Generate multi-octave Simplex noise
double generateSimplexFBM(double x, double y, int octaves = 4,
                          double persistence = 0.5, double lacunarity = 2.0,
                          unsigned int seed = 0) {
  SimplexNoise simplex(seed);
  double total = 0.0;
  double frequency = 1.0;
  double amplitude = 1.0;
  double maxValue = 0.0;

  for (int i = 0; i < octaves; i++) {
    total += simplex.noise2D(x * frequency, y * frequency) * amplitude;
    maxValue += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }

  return total / maxValue;
}
