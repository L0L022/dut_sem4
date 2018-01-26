//-------------------------------------------------------------------
// Vector3f.h
//-------------------------------------------------------------------
// Classe definissant un vecteur.
// Derniere modification : 12/05/1999
//-------------------------------------------------------------------
// Sebastien Thon
//-------------------------------------------------------------------

#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
using namespace std;


class Vector3f
{
public:
	Vector3f():
		   x(0), y(0), z(0) {}
	Vector3f (float x1, float y1, float z1):
		   x(x1), y(y1), z(z1) {}
	Vector3f ( const Vector3f &v ):
		   x(v.x),
		   y(v.y),
		   z(v.z) {}

	float distance( const Vector3f& v ) const;

	float  operator %  ( const Vector3f& v ) const;		// produit scalaire
	Vector3f operator ^  ( const Vector3f& v ) const;		// produit vectoriel
	void normalize();

	Vector3f operator + ( const Vector3f& v ) const;
	Vector3f operator + ( float d ) const;
	Vector3f operator - ( const Vector3f& v ) const;
	Vector3f operator - ( float d ) const;
	Vector3f operator - () const;

	Vector3f operator *  ( const Vector3f& v ) const;
	Vector3f operator *  ( float d ) const;
	Vector3f operator /  ( const Vector3f& v ) const;
	Vector3f operator /  ( float d ) const;

	int    operator >  ( const Vector3f& v );
	int    operator <  ( const Vector3f& v );
	int    operator == ( const Vector3f& v );
	int    operator != ( const Vector3f& v );

	Vector3f& operator =  ( const Vector3f& v );
	Vector3f& operator += ( const Vector3f& v );
	Vector3f& operator -= ( const Vector3f& v );
	Vector3f& operator += ( float d );
	Vector3f& operator -= ( float d );
	Vector3f& operator *= ( float d );
	Vector3f& operator /= ( float d );
	
	friend Vector3f operator + ( float d, const Vector3f& v )
	{
		return Vector3f( v.x+d, v.y+d, v.z+d );
	}

	friend Vector3f operator * ( float d, const Vector3f& v )
	{
		return Vector3f( v.x*d, v.y*d, v.z*d );
	}

	friend ostream& operator << ( ostream& os, const Vector3f& v )
	{
		return os << v.x << ", " << v.y << ", " << v.z;
	}

	float length() const;


	float x, y, z;
};

#endif
