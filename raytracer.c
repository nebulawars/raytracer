#include "raytracer.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

double vectorLength(vector_t vector) {
	return sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

vector_t vectorScale(vector_t vector, double scale) {
	vector_t result = { vector.x * scale, vector.y*scale, vector.z*scale };
	return result;
}

vector_t vectorNormalize(vector_t vector) {
	float length = vectorLength(vector);
	vector_t result = { vector.x/length, vector.y/length, vector.z/length };
	return result;
}

double vectorDotProduct(vector_t vector1, vector_t vector2) {
	return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z;
}

vector_t vectorCrossProduct(vector_t vector1, vector_t vector2) {
	vector_t result = {
		vector1.y*vector2.z - vector1.z*vector2.y,
		vector1.z*vector2.x - vector1.x*vector2.z,
		vector1.x*vector2.y - vector1.y*vector2.x,
	};
	return result;
}

vector_t vectorAdd(vector_t v1, vector_t v2) {
	vector_t result = { v1.x+v2.x, v1.y+v2.y, v1.z+v2.z };
	return result;
}

vector_t vectorSub(vector_t v1, vector_t v2) {
	vector_t result = { v1.x-v2.x, v1.y-v2.y, v1.z-v2.z };
	return result;
}

void vectorPrint(vector_t vector) {
	printf("[%f, %f, %f]\n", vector.x, vector.y, vector.z);
}

struct rayscene_t {
};

/*
 * Create a scene setup
 */
rayscene_t* rayCreateScene() {
}

/*
 * Delete the scene setup
 */
void rayDeleteScene(rayscene_t *rayscene) {
}

/*
 * Add a sphere to a scene
 */
void rayCreateSolidSphere(rayscene_t *scene, vector_t position, double radius) {
}

double vectorRadian(vector_t v1, vector_t v2) {
	return acosf(vectorDotProduct(vectorNormalize(v1), vectorNormalize(v2)));	
}

/*
 * Cast a ray
 */
color_t rayCastRaySphere(vector_t camPosition, vector_t rayDirection, /* */ vector_t spherePosition, double radius, light_t light) {
	color_t result = { 0,0,0 };
	vector_t c = vectorSub(spherePosition, camPosition);
	vector_t n = vectorNormalize(rayDirection);
	// d = (I*C) +- sqrt( (I*C)^2 - c^2 + r^2 )
	double dotNC = vectorDotProduct(n,c);
	double dotC2 = vectorDotProduct(c,c);
	double r = radius;
	double t = dotNC*dotNC - dotC2 + r*r;
	//vectorPrint(position);
	//vectorPrint(direction);
	double selectedSolution;
	if( t < 0.0 ) {
		//printf("No Solution\n");
		return result;
	}
	else if( t == 0.0 ) {
		double d = dotNC;
		//printf("One Solution:\n", d);
		//vectorPrint(vectorAdd(position, vectorScale(direction, d)));
		selectedSolution = d;
	}
	else {
		double s = sqrtf(t);
		double d1 = dotNC + s;
		double d2 = dotNC - s;
		//printf("Two Solutions:\n", d1, d2);
		//vectorPrint(vectorScale(direction, d1));
		//vectorPrint(vectorScale(direction, d2));
		selectedSolution = d1 < d2 ? d1:d2;
	}

	vector_t normal = vectorNormalize(vectorSub(vectorScale(rayDirection, selectedSolution), c));
	{ // Add a light
		color_t  color = light.color;
		double a = vectorRadian(light.position, normal) * 180.0 / M_PI;
		if( a <= 90.0 ) {
			result.r += (1.0-a/90.0) * color.r;
			result.g += (1.0-a/90.0) * color.g;
			result.b += (1.0-a/90.0) * color.b;
		}
	}

	result.r = result.r < 1.0 ? result.r : 1.0;
	result.g = result.g < 1.0 ? result.g : 1.0;
	result.b = result.b < 1.0 ? result.b : 1.0;

	return result;
}

color_t rayCastRayPolygon(vector_t camPosition, vector_t rayDirection, /* */ polygon_t polygon, light_t light) {
	color_t result = { 0,0,0 };

	vector_t t0 = polygon.p0;
	vector_t t1 = polygon.p1;
	vector_t t2 = polygon.p2;

	vector_t u = vectorSub(t1, t0);
	vector_t v = vectorSub(t2, t0);

	vector_t p0 = t0;
	vector_t n  = vectorNormalize(vectorCrossProduct(u, v));
	vector_t l0 = camPosition;
	vector_t ld = vectorNormalize(rayDirection);

	// check if ray hits plane
	double dotLDN = vectorDotProduct(ld, n);

	if( dotLDN ) { 
		// calculate ray plane instersection point
		double d = vectorDotProduct(vectorSub(p0, l0), n) / dotLDN;
		vector_t lp = vectorAdd(l0, vectorScale(ld, d));
		// Check if inside triangle
		vector_t w = vectorSub(lp, t0);
		double dotVV = vectorDotProduct(v,v);
		double dotUU = vectorDotProduct(u,u);
		double dotUV = vectorDotProduct(u,v);
		double denom = dotUV*dotUV - dotUU*dotVV;
		//printf("-----\n");
		//printf("ray [%f, %f, %f] [%f, %f, %f]\n", l0.x, l0.y, l0.z, ld.x, ld.y, ld.z);
		//printf("plane [%f, %f, %f] [%f, %f, %f]\n", p0.x, p0.y, p0.z, n.x, n.y, n.z);
		//printf("lp xyz [%f, %f, %f]\n", lp.x, lp.y, lp.z);
		//printf("vv:%f uu:%f uv:%f denom:%f\n", dotVV, dotUU, dotUV, denom);
		if( denom ) {
			double dotWU = vectorDotProduct(w,u);
			double dotWV = vectorDotProduct(w,v);
			double dotUV = vectorDotProduct(u,v);
			double s = (dotUV*dotWV - dotVV*dotWU) / denom;
			double t = (dotUV*dotWU - dotUU*dotWV) / denom;
			//printf("wu:% wv:%f uv:%f s:%f t:%f\n", dotWU, dotWV, dotUV, s, t);
			if( (s>=0.0&&s<=1.0) && (t>=0.0&&t<=1.0) && (s+t>=0&&s+t<=1.0) )
			{ // Add a light
				//printf(" s, t: %f %f\n", s, t);
				color_t  color = light.color;
				double a = vectorRadian(light.position, n) * 180.0 / M_PI;
				if( a <= 90.0 ) {
					result.r += (1.0-a/90.0) * color.r;
					result.g += (1.0-a/90.0) * color.g;
					result.b += (1.0-a/90.0) * color.b;
				}
			}
		}
	}

	result.r = result.r < 1.0 ? result.r : 1.0;
	result.g = result.g < 1.0 ? result.g : 1.0;
	result.b = result.b < 1.0 ? result.b : 1.0;

	return result;
}
/*
int main() {
	vector_t p = { 0,0,0 };
	vector_t d = { 1,0,1 };
	rayCastRay2(NULL, p, d);
	return EXIT_SUCCESS;
}
*/
/*
void renderImage() {
	int x, y;
	color_t image[100][100];
	unsigned char targaHeader[18];
	int fd;

	memset(targaHeader, 0, 18);
	
	targaHeader[2]  = 2;
	targaHeader[12] = 100;
	targaHeader[14] = 100;
	targaHeader[16] = 24;
	
	for(y=0; y<100; y++) {
		for(x=0; x<100; x++) {
			vector_t position = { 0,0,10 };
			vector_t direction = { (x-50)/50.0,(y-50)/50.0, 1 };
			float radius = 5.0;
			vector_t light = { -1, 0, -1 };
			color_t pixel = rayCastRay(NULL, position, direction, radius, light);
			image[y][x] = pixel;
		}
	}

	fd = open("screen.tga", O_WRONLY | O_CREAT | O_TRUNC );
	write(fd, targaHeader, 18);
	for(y=0; y<100; y++) {
		for(x=0; x<100; x++) {
			unsigned char c[3];
			c[0] = image[y][x].r * 256;
			c[1] = image[y][x].g * 256;
			c[2] = image[y][x].b * 256;
			write(fd, c, 3);
		}
	}
	close(fd);	
}

int main() {
	renderImage();

	return EXIT_SUCCESS;
}
*/
