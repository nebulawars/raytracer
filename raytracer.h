#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

typedef struct vector_t vector_t;
struct vector_t {
	double x, y, z;
};

typedef struct polygon_t polygon_t;
struct polygon_t {
	vector_t p0, p1, p2;
};

typedef struct color_t color_t;
struct color_t {
	double r, g, b, a;
};

typedef struct light_t light_t;
struct light_t {
	vector_t position;
	color_t  color;
};

inline double vectorLength(vector_t vector);
inline vector_t vectorScale(vector_t vector, double scale);
inline vector_t vectorNormalize(vector_t vector);
inline double vectorDotProduct(vector_t vector1, vector_t vector2);
inline vector_t vectorCrossProduct(vector_t vector1, vector_t vector2);
inline vector_t vectorAdd(vector_t vector1, vector_t vector2);
inline vector_t vectorSub(vector_t vector1, vector_t vector2);
inline void vectorPrint(vector_t vector);

typedef struct rayscene_t rayscene_t;

/*
 * Create a scene setup
 */
rayscene_t* rayCreateScene();

/*
 * Delete the scene setup
 */
void rayDeleteScene(rayscene_t *rayscene);

/*
 * Cast a ray
 */
color_t rayCastRaySphere(vector_t cameraPos, vector_t rayDirection, /* */ vector_t spherePosition, double radius, light_t light);
color_t rayCastRayPolygon(vector_t cameraPos, vector_t rayDirection, /* */ polygon_t polygon, light_t light);

#endif /* __RAYTRACER_H__ */
