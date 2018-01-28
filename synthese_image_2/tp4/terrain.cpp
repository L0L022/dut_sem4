///////////////////////////////////////////////////////////////////////////////
// IUT d'Aix-Marseille Université, site d'Arles
// Département Informatique
// 2ème année
// ----------------------------------------------------------------------------
// Auteur : 
// ----------------------------------------------------------------------------
// Classe définissant un terrain 3D.
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "terrain.h"
#include "pgm.h"



///////////////////////////////////////////////////////////////////////////////
// Constructeur de la classe Terrain.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
//
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
Terrain::Terrain()
{
    points_terrain = nullptr;
    liste_indices = nullptr;
}



///////////////////////////////////////////////////////////////////////////////
// Destructeur de la classe Terrain.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
//
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
Terrain::~Terrain()
{
    delete[] points_terrain;
    delete[] liste_indices;
}



///////////////////////////////////////////////////////////////////////////////
// Affichage du terrain.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
//
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
void Terrain::affiche()
{
	//    On étudie les points 4 par 4, en les utilisant pour tracer
	//    deux triangles comme suit :
	//
	//         0---3      1er  triangle : 0 -> 1 -> 3
	//	       |  /|      2ème triangle : 1 -> 2 -> 3
	//         | / |
	//         1/--2
	//
    text.utiliser();
    glInterleavedArrays( GL_T2F_N3F_V3F, 0, points_terrain );
    glDrawElements( GL_TRIANGLES, nb_faces * 3, GL_UNSIGNED_INT, liste_indices );
}



///////////////////////////////////////////////////////////////////////////////
// Création du terrain à partir d'une image.
//-----------------------------------------------------------------------------
// Paramètres :
//    cx, cy, cz     (in) : coefficients d'agrandissement selon les 3 axes.
//    image_hauteurs (in) : image au format pgm dont les niveaux de gris
//                          définissent les hauteurs d'un terrain.
//
// Retour :
//    true si le terrain a pu être crée (l'image a pu être chargée),
//    false sinon.
///////////////////////////////////////////////////////////////////////////////
bool Terrain::creation(	float dx, float dy, float dz, const char *image_hauteurs )
{
	// Tableau de byte servant à stocker les pixels en niveaux de gris de
	// l'image au format pgm "image_hauteurs".
	byte	*img;

	// Chargement d'une image en niveaux de gris. Chaque pixel correspondra
	// à un point du terrain.
	img = load_PGM( image_hauteurs, &nb_pt_x, &nb_pt_z );

	// Si l'image n'a pas pu être chargée, alors on quitte la fonction.
    if( img == nullptr )
		return false;

    nb_faces = (nb_pt_x - 1) * (nb_pt_z - 1) * 2;

	dist_x = dx;
	dist_y = dy;
	dist_z = dz;

    delete[] points_terrain;
    delete[] liste_indices;

	points_terrain = new Point_terrain[nb_pt_x * nb_pt_z];
    liste_indices = new GLuint[nb_faces * 3];

	int x, z, num = 0;

    size_t i = 0;

	for( z=0; z<nb_pt_z; z++ )
	{
		for( x=0; x<nb_pt_x; x++ )
        {
            points_terrain[num].s = x/float(nb_pt_x);
            points_terrain[num].t = z/float(nb_pt_z);

            points_terrain[num].nx = 0;
            points_terrain[num].ny = 0;
            points_terrain[num].nz = 0;

			points_terrain[num].x = x * dist_x;
			points_terrain[num].z = z * dist_z;
			points_terrain[num].hauteur = img[num] * dist_y;

            if (x + 1 < nb_pt_x && z + 1 < nb_pt_z) {
                //
                //         0---3      1er  triangle : 0 -> 1 -> 3
                //	       |  /|      2ème triangle : 1 -> 2 -> 3
                //         | / |
                //         1/--2
                //
                size_t s_0 = x + z * nb_pt_x;
                size_t s_1 = s_0 + nb_pt_x;
                size_t s_2 = s_1 + 1;
                size_t s_3 = s_0 + 1;

                liste_indices[i] = s_0;
                liste_indices[++i] = s_1;
                liste_indices[++i] = s_3;

                liste_indices[++i] = s_1;
                liste_indices[++i] = s_2;
                liste_indices[++i] = s_3;
                ++i;
            }

            num++;
		}
	}

    for (size_t i = 0; i < nb_faces * 3; ++i) {
        size_t s_0 = liste_indices[i], s_1 = liste_indices[++i], s_2 = liste_indices[++i];

        Vector3f v_0(points_terrain[s_0].x, points_terrain[s_0].hauteur, points_terrain[s_0].z)
                ,v_1(points_terrain[s_1].x, points_terrain[s_1].hauteur, points_terrain[s_1].z)
                ,v_2(points_terrain[s_2].x, points_terrain[s_2].hauteur, points_terrain[s_2].z);

        Vector3f face_normal = ((v_1 - v_0) ^ (v_2 - v_0));

        points_terrain[s_0].nx += face_normal.x;
        points_terrain[s_0].ny += face_normal.y;
        points_terrain[s_0].nz += face_normal.z;

        points_terrain[s_1].nx += face_normal.x;
        points_terrain[s_1].ny += face_normal.y;
        points_terrain[s_1].nz += face_normal.z;

        points_terrain[s_2].nx += face_normal.x;
        points_terrain[s_2].ny += face_normal.y;
        points_terrain[s_2].nz += face_normal.z;
    }

    for (size_t i = 0; i < nb_pt_x * nb_pt_z; ++i) {
        Vector3f v(points_terrain[i].nx, points_terrain[i].ny, points_terrain[i].nz);
        v.normalize();
        points_terrain[i].nx = v.x;
        points_terrain[i].ny = v.y;
        points_terrain[i].nz = v.z;
    }


	delete[] img;

	terrain_ambient[0]=0.6f;  terrain_ambient[1]=0.6f;  terrain_ambient[2]=0.6f;  terrain_ambient[3]=1.0f;
	terrain_diffuse[0]=0.9f;  terrain_diffuse[1]=0.9f;  terrain_diffuse[2]=0.9f;  terrain_diffuse[3]=1.0f;
	terrain_specular[0]=0.0f; terrain_specular[1]=0.0f; terrain_specular[2]=0.0f; terrain_specular[3]=1.0f;
	terrain_shininess[0]=1.0f;

	return true;
}
