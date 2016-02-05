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

#include "imgtorque.h"

/**
 * imgtorqueNew:
 * @int w, int h
 *
 *
 * Returns: 
 */
imgtorque_t *
imgtorqueNew(int w, int h)
{
	imgtorque_t *it = g_new0(imgtorque_t,1);
	it->w = w;
	it->h = h;
	it->torque = g_new0(double, w);
	return it;
}


/**
 * imgtorqueResize:
 * @int neww, int newh
 *
 *
 * Returns: 
 */
void
imgtorqueResize(imgtorque_t *it, int neww, int newh)
{
	it->torque = g_renew(double, it->torque, neww);
	it->w = neww;
	it->h = newh;
}

void
eval_3equation(double *reta, double *retb, double *retc,
	double r1, double x1, double y1,
	double r2, double x2, double y2,
	double r3, double x3, double y3)
{
	double a,b,c;
	double y21 = y2 - y1;
	double y31 = y3 - y1;
	g_assert(reta && retb && retc);

	a = ((r3-r1)*y21 - (r2-r1)*y31) / ((x3-x1)*y21 - (x2-x1)*y31);
	b = ((r2-r1) - a*(x2-x1)) / y21;
	c = r1 - a*x1 - b*y1;

	*reta = a;
	*retb = b;
	*retc = c;
}

/*
void
imgtorqueMkMatrix(imgtorque_t *it)
{
	g_assert(it->p1.set && it->p2.set && it->p3.set);
	
	eval_3equation(&it->a, &it->b, &it->c,
		it->p1.rpm, it->p1.x, it->p1.y,
		it->p2.rpm, it->p2.x, it->p2.y,
		it->p3.rpm, it->p3.x, it->p3.y);
	
	eval_3equation(&it->d, &it->e, &it->f,
		it->p1.value, it->p1.x, it->p1.y,
		it->p2.value, it->p2.x, it->p2.y,
		it->p3.value, it->p3.x, it->p3.y);
}
*/


void
imgtorqueMkMatrix(imgtorque_t *it)
{
	g_assert(it->cp[0].set && it->cp[1].set && it->cp[2].set);
	
	eval_3equation(&it->a, &it->b, &it->c,
		it->cp[0].rpm, it->cp[0].x, it->cp[0].y,
		it->cp[1].rpm, it->cp[1].x, it->cp[1].y,
		it->cp[2].rpm, it->cp[2].x, it->cp[2].y);
	
	eval_3equation(&it->d, &it->e, &it->f,
		it->cp[0].value, it->cp[0].x, it->cp[0].y,
		it->cp[1].value, it->cp[1].x, it->cp[1].y,
		it->cp[2].value, it->cp[2].x, it->cp[2].y);
}


void
imgtorqueCopy2Car(imgtorque_t *it, car_t *car)
{
	
}


/**
 * imgtorqueFree:
 * @imgtorque_t *it
 *
 *
 * Returns: 
 */
void           
imgtorqueFree(imgtorque_t *it)
{
	g_free(it->torque);
	g_free(it);
}


