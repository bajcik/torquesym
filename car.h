#ifndef _TORQUE_H_
#define _TORQUE_H_

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

#define CAR_PART_ENGINE (1<<0)
#define CAR_PART_GEARBOX (1<<1)
#define CAR_PART_DIFF (1<<2)
#define CAR_PART_TIRE (1<<3)
#define CAR_PART_WEIGHT (1<<4)
#define CAR_PART_CAR 0xffff

#define SAMPLES_PER_RPM 1 /* TODO: values other than 1 */
#define MAXRPM 23000
#define MAXGEARS 7

#define KMHN   5
#define SECSAMPL 100       /* samples per second IN TABLE */
#define QTIME (1.0/(double)SECSAMPL)

#define VRECORDS 12
#define SRECORDS 7

typedef struct
{
	int        parts;
	char      *name;
	gboolean   check;   /* parameters valid (see carCheck) */
	GString   *checkerror;
	int        minrpm;
	int        maxrpm;
	int        ts;      /* (maxrpm-minrpm+1)*SAMPLES_PER_RPM */
	double     torque[MAXRPM];  /* torque on each rpm */
	double     hp[MAXRPM];      /* horse power on each rpm */
	int        maxtorqueat;
	int        maxhpat;
	int        gears;
	double     gear[MAXGEARS];
	double     wheeltorq[MAXGEARS][MAXRPM];
	double     differential;
	
	double     tire_width;   /* in mm */
	double     tire_ratio;   /* in % */
	double     tire_rim;     /* in inches */
	double     rwheel;  /* in meters */
	
	double     weight;
	double     vmax;    /* in m/s */
	double     vmaxkmh; /* in km/h */
	
	//double     force[MAXRPM*KMHN]; ??
	int        seconds;
	double    *accel;  /* totalseconds * SECSAMPL */
	double    *speed;  /* totalseconds * SECSAMPL */
	double    *road;   /* totalseconds * SECSAMPL */
	double     vtimes[VRECORDS];
	double     stimes[SRECORDS];
} car_t;

#define MS2KMH(ms)  ((double)(ms) * 3.6)
#define KMH2MS(kmh) ((double)(kmh) * (1.0/3.6))
#define INCH 0.0254
#define WHEEL2R(car) ((car)->tire_rim * INCH / 2 + (car)->tire_ratio/100.0 * (car)->tire_width*0.001)
#define RPM2RADS(rpm) ((double)(rpm) * 2*M_PI / 60.0)
#define RADS2RPM(rad) ((double)(rad) * 60 / (2*M_PI))
#define WATT2HP(w)    ((double)(w) * 0.00136)
#define HP2WATT(hp)   ((double)(hp) / 0.00136)
#define MILE 1609.344

car_t       *carNew();
void         carFree(car_t *car);
double       carV(car_t *car, int gear, int rpm);
double       carWheelForce(car_t *car, int gear, int rpm);
void         carRPMRange(car_t *car, int minrpm, int maxrpm, gboolean mksin);
void         carSeconds(car_t *car, int seconds);
void         carCountHP(car_t *car);
void         carCountWheelTorques(car_t *car);
//void         carCountForce(car_t *car);
//double       carForce(car_t *car, double ms);
void         carCountTimeline(car_t *car);

gboolean     carCheck(car_t *car);

#endif

