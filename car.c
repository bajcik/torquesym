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


#include <math.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "car.h"


/**
 * carNew:
 * @
 *
 *
 * Returns: 
 */
car_t *
carNew()
{
	car_t *car = g_new0(car_t, 1);
	
	car->parts = CAR_PART_CAR;
	car->name = g_strdup("Your car");
	car->checkerror = g_string_new("");
	carRPMRange(car, 400, 7000, TRUE);
	carSeconds(car, 26);
	car->gears = 5;
	car->gear[0] = 3.2;
	car->gear[1] = 2.4;
	car->gear[2] = 1.6;
	car->gear[3] = 1.0;
	car->gear[4] = 0.8;
	car->differential = 3.2;
	car->tire_width = 185;
	car->tire_ratio = 70;
	car->tire_rim = 15.0;
	car->rwheel = WHEEL2R(car);
	car->weight = 1200;
	car->vmax = carV(car, car->gears-1, car->maxrpm);
	car->vmaxkmh = MS2KMH(car->vmax);
	carCountHP(car);
	carCountWheelTorques(car);
	carCountTimeline(car);
	
	return car;
}

void
carFree(car_t *car)
{
	g_free(car->accel);
	g_free(car->speed);
	g_free(car->road);
	g_free(car);
}


/**
 * carVMax:
 * @car_t *car
 *
 *
 * Returns: max speed in m/s
 */
double
carV(car_t *car, int gear, int rpm)
{
	g_assert(gear >=0 && gear < car->gears && rpm >= car->minrpm && rpm <= car->maxrpm);
	return RPM2RADS(rpm)
		/ car->gear[gear]
		/ car->differential
		* car->rwheel;
}

/* RPM can be out of range minrpm...maxrpm */
int
carV2rpm(car_t *car, int gear, double v)
{
	g_assert(gear >=0 && gear < car->gears && v >= 0);
	return RADS2RPM(
		v
		* car->gear[gear]
		* car->differential
		/ car->rwheel);
}


double
carWheelForce(car_t *car, int gear, int rpm)
{
	g_assert(gear >=0 && gear < car->gears && rpm >= car->minrpm && rpm <= car->maxrpm);
	return car->torque[rpm]
		* car->gear[gear]
		* car->differential
		/ car->rwheel;
	/*
	for (g=0; g<car->gears; g++)
		for (i=car->minrpm; i<=car->maxrpm; i++)
			car->wheeltorq[g][i] = RPM2RADS(car->torque[i])
				* car->gear[g]
				/ car->differential;
	*/
}

/**
 * carRPMRange:
 * @car_t *car, int minrpm, int maxrpm
 *
 *
 * Returns: 
 */
void
carRPMRange(car_t *car, int minrpm, int maxrpm, gboolean mksin)
{
	int ts, i;
	g_assert(car && minrpm>0 && maxrpm>minrpm);
	
	ts = maxrpm-minrpm+1;
	if (mksin)
		for (i=0; i<ts; i++)
		{
			double w = (double)(i)/(double)ts; /* 0..1 */
			car->torque[minrpm+i] = 10.0+200.0*sqrt(w)*sqrt(1-w);
		}
	
	car->minrpm = minrpm;
	car->maxrpm = maxrpm;
	car->ts = ts;
}

void
carSeconds(car_t *car, int seconds)
{
	g_free(car->accel);
	g_free(car->speed);
	g_free(car->road);
	car->seconds = seconds;
	car->accel = g_new(double, seconds*SECSAMPL);
	car->speed = g_new(double, seconds*SECSAMPL);
	car->road = g_new(double, seconds*SECSAMPL);
}

void
carCountHP(car_t *car)
{
	int i;
	int top_hp = 0;
	int top_torque = 0;
	for (i=car->minrpm; i<=car->maxrpm; i++)
	{
		car->hp[i] = WATT2HP(RPM2RADS(i) * car->torque[i]);
		if (car->hp[i] > top_hp)
		{
			car->maxhpat = i;
			top_hp = car->hp[i];
		}
		if (car->torque[i] > top_torque)
		{
			car->maxtorqueat = i;
			top_torque = car->torque[i];
		}
	}
}


void
carCountWheelTorques(car_t *car)
{
	int g,i;
	for (g=0; g<car->gears; g++)
		for (i=car->minrpm; i<=car->maxrpm; i++)
			car->wheeltorq[g][i] = RPM2RADS(car->torque[i])
				* car->gear[g]
				* car->differential;
}


/**
 * carCheck:
 * @
 *
 *
 * Returns: 
 */

#define CHKERR(str, a...) do {g_string_printf(car->checkerror, str, ## a); goto err; } while (0)

gboolean
carCheck(car_t *car)
{
	int i;

	if (car->gears < 1 || car->gears > 7)
		CHKERR("Car has %d gears, should have 1..7", car->gears);

	if (car->gear[0] < 1.0)
		CHKERR("First gear ratio must be >= 1.0");
	
	for (i=1; i<car->gears; i++)
		if (car->gear[i] >= car->gear[i-1])
			CHKERR("Gear %d ratio must be less than %d", i+1, i);
	
	if (car->differential < 0.5 || car->differential > 20)
		CHKERR("Differential %f not in (0,5..20)", car->differential);

	car->check = TRUE;
	return TRUE;
	
err:
	car->check = FALSE;
	return FALSE;
}



double
carForce(car_t *car, double v, int *gear)
{
	int rpm,rpm2;
	double f,f2;
	
	rpm = carV2rpm(car, *gear, v);
	rpm = CLAMP(rpm, car->minrpm, car->maxrpm);
	
	/* clutch at 1st gear */
	if (*gear == 0)
		rpm = MAX(rpm, car->maxtorqueat);
	
	f = carWheelForce(car, *gear, rpm);

	if (*gear < car->gears-1)
	{
		rpm2 = carV2rpm(car, *gear+1, v);
		rpm2 = CLAMP(rpm2, car->minrpm, car->maxrpm);
		f2 = carWheelForce(car, *gear+1, rpm2);
		if (f2 > f)
		{
			//printf("G/rpm/F: %d/%d/%f -> %d/%d/%f\n", *gear, rpm, f, *gear+1, rpm2, f2);
			f = f2;
			(*gear)++;
		}
	}
	
	return f;
}


double VRecords[VRECORDS] = {20.0, 40.0, 60.0, 80.0, 100.0, 120.0, 150.0, 100.0*MILE/1000.0, 200.0, 250.0, 300.0, 200.0*MILE/1000.0};
double SRecords[SRECORDS] = {10.0, 100.0, 0.125*MILE, 0.25*MILE, 500.0, 1000.0, MILE};

void
carCountTimeline(car_t *car)
{
	double t=0.0, v=0.0, s=0.0, a, F;
	int gear = 0;
	int i;
	int vrec = 0, srec=0;

	for (i=0; i<VRECORDS; i++) car->vtimes[i] = 0.0;
	for (i=0; i<SRECORDS; i++) car->stimes[i] = 0.0;
	
	for (i=0; i<car->seconds*SECSAMPL; i++)
	{
		t = (double)i/(double)SECSAMPL;
		F = carForce(car, v, &gear); // a table? or sth else..
		a = F/car->weight;
		v += a*QTIME;
		s += v*QTIME;

		car->accel[i] = a;
		car->speed[i] = v;
		car->road[i] = s;

		if (vrec<VRECORDS && v >= KMH2MS(VRecords[vrec]))
			car->vtimes[vrec++] = t;
		if (srec<SRECORDS && s >= SRecords[srec])
			car->stimes[srec++] = t;
	}
}


