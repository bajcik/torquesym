#ifndef _WND_IMGGRAPH_H_
#define _WND_IMGGRAPH_H_

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
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "imgtorque.h"


typedef struct
{
	GtkWidget    *wnd;
	GtkWidget    *draImgGraph, *draZoom;
	GtkWidget    *tglPath, *tglCalibr1, *tglCalibr2, *tglCalibr3;
	GtkWidget    *spnCalibrRpm, *spnCalibrValue;
	GdkPixbuf    *pixbuf;
	guchar       *pixels;
	gboolean      updating_ui;
	int           mousex, mousey;
	int           zoomw, zoomh;
	int           mode;
	imgtorque_t  *it;
	car_t        *car;
} wnd_imggraph_t;

/*
typedef struct
{
	int              w,h;
	coordpoint_t     p1,p2,p3;
	double           a,b,c,d,e,f;  // coord matrix
	int              type;
	double          *torque; // better than int in km<->nm conv
} imgtorque_t;
*/

gboolean           wnd_imgtorqueNewCreate(char *fname, car_t *car, GError **err);
wnd_imggraph_t    *wnd_imgtorqueNew();
void               wnd_imgtorqueFree(wnd_imggraph_t *wnd);


#endif

