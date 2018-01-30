#include <errno.h>
#include <unistd.h>

#include <cstdio>

#include <cstring>
#include <cstdlib>
#include <cmath>

#include <iostream>

#include <GL/gl.h>

#include "objet3d.h"



Objet3D::Objet3D() {
	strcpy(nom, "nil");
	nbsommets=0;
	nbfaces=0;
	id=0;
}

Objet3D::~Objet3D() {
	delete [] lpoints;
	delete [] lfaces;
}

void Objet3D::calcul_normales_points()
{

    lpointsnormales = new struct point3D[nbsommets];

    for (unsigned int s = 0; s < nbsommets; s++) {
        std::cerr << "calcul normales points, traitement de " << s << "/" << nbsommets << std::endl;
        //on cherche pour chaque sommets la liste des faces auquelles il appartient
        unsigned int tab_faces[20];
        unsigned int cpt_faces=0;

        for (unsigned int f = 0; f < nbfaces ; f++) {
            if ( (lfaces[f].S1 == s) ||  (lfaces[f].S2 == s) ||  (lfaces[f].S3 == s)) {
                tab_faces[cpt_faces] = f;
                cpt_faces++;
            }
        }

        //on calcule ensuite pour ce sommet la moyenne des normales des faces
        struct point3D moyenne;
        moyenne.x = 0.0f;
        moyenne.y = 0.0f;
        moyenne.z = 0.0f;

        for (unsigned int sf = 0; sf < cpt_faces ; cpt_faces++) {
            moyenne.x += lfacesnormales[tab_faces[sf]].x;
            moyenne.y += lfacesnormales[tab_faces[sf]].y;
            moyenne.z += lfacesnormales[tab_faces[sf]].z;
        }

        //affectation de la normale
        lpointsnormales[s].x = moyenne.x / float(cpt_faces);
        lpointsnormales[s].y = moyenne.y / float(cpt_faces);
        lpointsnormales[s].z = moyenne.z / float(cpt_faces);
        }

}


void Objet3D::calcul_normales_faces()
{
    struct point3D v1;
    struct point3D v2;

    lfacesnormales = new struct point3D[nbfaces];

    for (unsigned int i = 0; i < nbfaces; i++)
    {

        v1 = VecteurFromPoints(&lpoints[lfaces[i].S1], &lpoints[lfaces[i].S2]);
        v2 = VecteurFromPoints(&lpoints[lfaces[i].S3], &lpoints[lfaces[i].S1]);

        lfacesnormales[i] = produit_vectoriel(&v2, &v1);
        normalize(&lfacesnormales[i]);
    }
    std::cerr << "Fin calcul normales faces, traitement de " << nbfaces << " faces" << std::endl;
}




void Objet3D::charge_OFF(const char* nom_fichier)
{
    unsigned int i;
    FILE * fichier;
    unsigned int n;

    char magic[10];

    if ((fichier=fopen(nom_fichier,"r"))==NULL)
            {
            printf("%s\n",nom_fichier);
            perror("Erreur a l'ouverture du fichier OFF");
                exit(1);
            }

    fscanf(fichier,"%s\n",magic);

    if (strcmp(magic, "OFF")!=0) {
        perror("Erreur dans l'en tête du fichier OFF");
            exit(0);
    }

    fscanf(fichier,"%u %u %u\n",&nbsommets, &nbfaces, &i);

    //Allocation des listes de données
    lfaces=new struct indexedface[nbfaces];
    lpoints=new struct point3D[nbsommets];

    //Remplissage de la liste de points
    for (i=0;i<nbsommets;i++)
    {
        int r,g,b,a;
        float f1,f2;
        fscanf(fichier,"%lf %lf %lf\n",&lpoints[i].x,&lpoints[i].y,&lpoints[i].z);
        if (min.x>lpoints[i].x)
            min.x=lpoints[i].x;
        if (min.y>lpoints[i].y)
            min.y=lpoints[i].y;
        if (min.z>lpoints[i].z)
            min.z=lpoints[i].z;

        if (max.x<lpoints[i].x)
            max.x=lpoints[i].x;
        if (max.y<lpoints[i].y)
            max.y=lpoints[i].y;
        if (max.z<lpoints[i].z)
            max.z=lpoints[i].z;
    }

    //Remplissage de la liste de faces
    for (i=0;i<nbfaces;i++)
    {
       fscanf(fichier,"%u %u %u %u\n",&n, &lfaces[i].S1, &lfaces[i].S2,&lfaces[i].S3);
        if (n!=3) {
            perror("Erreur au chargement des faces, non triangulaires");
            exit(1);
            }
    }


    //calcul du centroid de l'objet à partir de la boîte min/max
    centroid.x = (max.x + min.x) / 2.0f;
    centroid.y = (max.y + min.y) / 2.0f;
    centroid.z = (max.z + min.z) / 2.0f;

    std::cerr << "Chargement du fichier " << nom_fichier << std::endl;
    std::cerr << "\t" << nom_fichier << " : " << nbsommets << " points - " << nbfaces << " arêtes" << std::endl;
    std::cerr << "\t" << nom_fichier << " MAX : " << max.x << " " << max.y << " " << max.z << std::endl;
    std::cerr << "\t" << nom_fichier << " MIN : " << min.x << " " << min.y << " " << min.z << std::endl;
    std::cerr << "\t" << nom_fichier << " CENTROID : " << centroid.x << " " << centroid.y << " " << centroid.z << std::endl;

    calcul_normales_faces();

    //calcul_normales_points();

    fclose(fichier);
}



void Objet3D::affiche_infos()
{
    unsigned int i;
    fprintf(stderr,"Objet %s nbsommets %u nbfaces %u\n",nom, nbsommets, nbfaces);

    fprintf(stderr,"\n SOMMETS\n");
        for (i=0;i<nbsommets;i++)
            fprintf(stderr,"Sommets %u [%.2f %.2f %.2f]\n",i,lpoints[i].x,lpoints[i].y,lpoints[i].z);

    fprintf(stderr,"\n FACES\n");
        for (i=0;i<nbfaces;i++)
                fprintf(stderr,"Face %u [%u %u %u]\n",i, lfaces[i].S1, lfaces[i].S2, lfaces[i].S3);


}


void Objet3D::affiche()
{

    glBegin(GL_TRIANGLES);
    for(int i=0; i<nbfaces; i++)
    {
        glNormal3dv( (double*)&lfacesnormales[i] ); glVertex3dv( (double*)&lpoints[lfaces[i].S1] );
        glNormal3dv( (double*)&lfacesnormales[i] ); glVertex3dv( (double*)&lpoints[lfaces[i].S2] );
        glNormal3dv( (double*)&lfacesnormales[i] ); glVertex3dv( (double*)&lpoints[lfaces[i].S3] );
    }
    glEnd();
}



void Objet3D::interpolation( Objet3D *start, Objet3D *end, float t)
{
	for (size_t i = 0;  i < nbsommets; ++i) {
        lpoints[i].x = ( 1 - t ) * start->lpoints[i].x + t * end->lpoints[i].x;
        lpoints[i].y = ( 1 - t ) * start->lpoints[i].y + t * end->lpoints[i].y;
        lpoints[i].z = ( 1 - t ) * start->lpoints[i].z + t * end->lpoints[i].z;
    }
}
