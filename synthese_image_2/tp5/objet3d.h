#ifndef OBJET3D
#define OBJET3D

#include "structgeom.h"

class Objet3D {

  public :
	char nom[40];

	unsigned int nbsommets;
	unsigned int nbfaces;

	unsigned int id;

	struct point3D *lpoints;
	struct indexedface *lfaces;
	struct point3D * lpointsnormales;
	struct point3D * lfacesnormales;
	
	struct point3D min;
	struct point3D max;
	struct point3D centroid;

	Objet3D();
	Objet3D(const Objet3D&);
	~Objet3D();
	
	void charge_OFF(const char* nom_fichier);
	void calcul_normales_points();
	void calcul_normales_faces();
	
	void affiche_infos();
	void affiche();
	void interpolation( Objet3D *start, Objet3D *end, float t);
};



#endif


