#ifndef _IMGTORQUE_H_
#define _IMGTORQUE_H_

/*
    Torque Simulator
    Copyright (C) 2005  Krzysztof Garus <bajcik@kolos.math.uni.lodz.pl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <glib.h>
#include "car.h"


typedef struct
{
	gboolean set;
	double x,y;         // img coord
	double rpm, value;  // HP or KW or NM
} coordpoint_t;

typedef struct
{
	int              w,h;
	coordpoint_t     cp[3];
	double           a,b,c,d,e,f;  // coord matrix
	int              type;
	double          *torque; // better than int in km<->nm conv
} imgtorque_t;


imgtorque_t   *imgtorqueNew(int w, int h);
void           imgtorqueResize(imgtorque_t *it, int neww, int newh);
void           imgtorqueMkMatrix(imgtorque_t *it);
void           imgtorqueCopy2Car(imgtorque_t *it, car_t *car);
void           imgtorqueFree(imgtorque_t *it);


#endif

