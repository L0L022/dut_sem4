#ifndef PGM_H
#define PGM_H



#ifndef byte
typedef unsigned char byte;
#endif



byte* load_PGM( const char *fname, int *width, int *height );



#endif
