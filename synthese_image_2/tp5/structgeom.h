#ifndef _H_STRUCTGEOM
#define _H_STRUCTGEOM

struct indexedface {
	unsigned int S1;
	unsigned int S2;
	unsigned int S3;
};

struct point3D {
	double x;
	double y;
	double z;
};

//fonctions sur ces objets
void normalize(struct point3D *);
float produit_scalaire(const struct point3D * , const struct point3D * );
struct point3D produit_vectoriel(const struct point3D * , const struct point3D * );
struct point3D VecteurFromPoints(const struct point3D *, const struct point3D *);

#endif
