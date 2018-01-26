#include <stdio.h>
#include <cctype>
#include "pgm.h"



byte* load_PGM( const char *fname, int *width, int *height )
{
	char	c,c2;
	unsigned int depth;
	FILE	*f=NULL;
	byte	*img;

	if(!(f=fopen(fname,"rb")))
		return NULL;

	c = fgetc(f);
	c2= fgetc(f);

	if(c!='P' && c2!='5')
		return NULL;

	do
	{
		c=fgetc(f); if(c=='#') while((c=fgetc(f))!='\n');
	} 
	while(!isdigit(c));

	ungetc(c,f);

	fscanf(f,"%d", width);

	do
	{
		c=fgetc(f); if(c=='#') while((c=fgetc(f))!='\n');
	} 
	while(!isdigit(c));

	ungetc(c,f);

	fscanf(f,"%d", height);

	do
	{
		c=fgetc(f); if(c=='#') while((c=fgetc(f))!='\n');
	} 
	while(!isdigit(c));

	ungetc(c,f);

	fscanf(f,"%d", &depth);

	img = new byte[*width * *height];

	fgetc(f);

	fread( img, 1, *width * *height, f );

	fclose(f);

	return img;
}
