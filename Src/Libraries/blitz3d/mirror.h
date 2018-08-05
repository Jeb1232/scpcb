
#ifndef MIRROR_H
#define MIRROR_H

#include "object.h"

class Mirror : public Object{
public:
	Mirror();
	Mirror( const Mirror &t );
	~Mirror();

	//Entity interface
    virtual Object* clone(){ return d_new Mirror( *this ); }
	Mirror *getMirror(){ return this; }
};

#endif