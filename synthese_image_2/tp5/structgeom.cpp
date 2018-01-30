#include <cmath>
#include "structgeom.h"

void normalize(struct point3D* _v) {

float lenght = sqrtf(produit_scalaire(_v, _v));
	_v->x /= lenght;
	_v->y /= lenght;
	_v->z /= lenght;
}

float produit_scalaire(const struct point3D * _v1, const struct point3D * _v2) {

float res;
	res = _v1 -> x * _v2->x + _v1->y * _v2->y + _v1->z * _v2->z;

return res;
}

struct point3D produit_vectoriel(const struct point3D * _v1 , const struct point3D * _v2) {

struct point3D res;
	res.x = _v1->y * _v2->z - _v1->z * _v2->y;
	res.y = _v1->z * _v2->x - _v1->x * _v2->z;
	res.z = _v1->x * _v2->y - _v1->y * _v2->x;
return res;

}


struct point3D VecteurFromPoints(const struct point3D * _p1, const struct point3D * _p2) {
struct point3D res;

	res.x = _p2->x - _p1->x;
	res.y = _p2->y - _p1->y;
	res.z = _p2->z - _p1->z;
	
return res;
}


