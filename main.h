#ifndef _MAIN_H_
#define _MAIN_H_

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

#include "car.h"

typedef struct
{
	GtkWidget *wndMain, *wndTorque, *wndWheelForce, *wndTime, *wndPerf, *wndImgGraph;
	GtkWidget *btnUpdate;
	GtkSpinButton *spinWeight;
	GtkSpinButton *spinGear[MAXGEARS];
	GtkLabel      *lblVMin[MAXGEARS];
	GtkLabel      *lblVMax[MAXGEARS];
	GtkLabel      *lblMaxTorque, *lblMaxHP;
	GtkScale *scaleGears;
	GtkSpinButton *spinMinRpm, *spinMaxRpm;
	GtkSpinButton *spinRim, *spinTireWidth, *spinTireRatio, *spinDiffer;
	GtkLabel      *lblWheelHeight, *lblWheelPerimeter;
	GtkStatusbar *sbMain;
	
	GtkWidget *daTorque;
	GtkSpinButton *spinMaxTorque, *spinMaxHP;

	GtkWidget *daWheelForce;
	GtkSpinButton *spinMaxWForce, *spinMaxSpeed;

	GtkWidget *daTime;
	GtkSpinButton *spinTAccel, *spinTSpeed, *spinTRoad, *spinTSeconds;

	GtkLabel *lblTV[VRECORDS], *lblTS[SRECORDS];
} widgets_t;

extern widgets_t *w;
extern car_t     *car;
extern char      *file_name;


#endif


