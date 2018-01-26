//-------------------------------------------------------------------
// Vector3f.cpp
//-------------------------------------------------------------------
// Classe definissant un vecteur.
// Derniere modification : 12/05/1999
//-------------------------------------------------------------------
// Sebastien Thon
//-------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include "vector3f.h"



float Vector3f::operator % ( const Vector3f& v ) const
{
	return x*v.x + y*v.y + z*v.z;
}



Vector3f Vector3f::operator ^ ( const Vector3f& v ) const
{
	return Vector3f( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x );
}



void Vector3f::normalize()
{
	float	len = (float)sqrt( x*x + y*y + z*z );

	if( len == 0 )
	{
		x=0;
		y=0;
		z=0;
	}
	else
	{
		x/=len;
		y/=len;
		z/=len;
	}
}



Vector3f Vector3f::operator + ( const Vector3f& v ) const
{
	return Vector3f( x+v.x, y+v.y, z+v.z );
}



Vector3f Vector3f::operator + ( float d ) const
{
	return Vector3f( x+d, y+d, z+d );
}



Vector3f operator + ( const Vector3f& v1, const Vector3f& v2 )
{
	return Vector3f( v1.x+v2.x, v1.y+v2.y, v1.z+v2.z );
}



Vector3f Vector3f::operator - ( const Vector3f& v ) const
{
	return Vector3f( x-v.x, y-v.y, z-v.z );
}



Vector3f Vector3f::operator - ( float d ) const
{
	return Vector3f( x-d, y-d, z-d );
}



Vector3f operator - ( const Vector3f& v1, const Vector3f& v2 )
{
	return Vector3f( v1.x-v2.x, v1.y-v2.y, v1.z-v2.z );
}



Vector3f Vector3f::operator - () const
{
	return Vector3f( -x, -y, -z );
}



Vector3f Vector3f::operator * ( const Vector3f& v ) const
{
	return Vector3f( x*v.x, y*v.y, z*v.z );
}



Vector3f operator * ( const Vector3f& v1, const Vector3f& v2 )
{
	return Vector3f( v1.x*v2.x, v1.y*v2.y, v1.z*v2.z );
}



Vector3f Vector3f::operator * ( float d ) const
{
	return Vector3f( x*d, y*d, z*d );
}



Vector3f Vector3f::operator / ( float d ) const
{
	return Vector3f( x/d, y/d, z/d );
}



Vector3f Vector3f::operator / ( const Vector3f& v ) const
{
	return Vector3f( x/v.x, y/v.y, z/v.z );
}



Vector3f operator / ( const Vector3f& v1, const Vector3f& v2 )
{
	return Vector3f( v1.x/v2.x, v1.y/v2.y, v1.z/v2.z );
}



int Vector3f::operator == ( const Vector3f& v )
{
	return( (x == v.x) && (y == v.y) && (z == v.z) );
}



int Vector3f::operator != ( const Vector3f& v )
{
	return( (x != v.x) || (y != v.y) || (z != v.z) );
}



int Vector3f::operator > ( const Vector3f& v )
{
	return( (x > v.x) || (y > v.y) || (z > v.z) );
}



int Vector3f::operator < ( const Vector3f& v )
{
	return( (x < v.x) || (y < v.y) || (z < v.z) );
}



Vector3f& Vector3f::operator = ( const Vector3f& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}



Vector3f& Vector3f::operator += ( const Vector3f& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}



Vector3f& Vector3f::operator -= ( const Vector3f& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}



Vector3f& Vector3f::operator += ( float d )
{
	x += d;
	y += d;
	z += d;
	return *this;
}



Vector3f& Vector3f::operator -= ( float d )
{
	x -= d;
	y -= d;
	z -= d;
	return *this;
}



Vector3f& Vector3f::operator *= ( float d )
{
	x *= d;
	y *= d;
	z *= d;
	return *this;
}



Vector3f& Vector3f::operator /= ( float d )
{
	x /= d;
	y /= d;
	z /= d;
	return *this;
}



float Vector3f::length() const
{
	return (float)sqrt( x*x + y*y + z*z );
}



float Vector3f::distance( const Vector3f& v ) const
{
	return (float)sqrt( (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z) );
}
