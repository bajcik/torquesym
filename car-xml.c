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
#include <stdlib.h>
#include "car.h"
#include "car-xml.h"


gboolean
carWrite(car_t *car, char *fname, GError **err)
{
	FILE *f = fopen(fname, "w");
	int i;
	g_return_val_if_fail (err == NULL || *err == NULL, FALSE);
	
	if (!f)
	{
		g_set_error(err, g_quark_from_string("carWrite"), 0, "Can't open file");
		return FALSE;
	}
	
	fprintf(f, "<?xml version=\"1.0\"?>\n<!DOCTYPE car SYSTEM \"http://kolos.math.uni.lodz.pl/~bajcik/car.dtd\">\n<car>\n");
	
	if (car->parts & CAR_PART_ENGINE)
	{
		fprintf(f, "<engine minrpm=\"%d\" maxrpm=\"%d\">\n", car->minrpm, car->maxrpm);
		for (i=car->minrpm; i<=car->maxrpm; i++)
			fprintf(f, " <torque value=\"%f\" />", car->torque[i]);
		fprintf(f, "\n</engine>\n");
	}
	
	if (car->parts & CAR_PART_GEARBOX)
	{
		fprintf(f, "<gearbox>\n");
		for (i=0; i<car->gears; i++)
			fprintf(f, " <gear ratio=\"%f\"/>\n", car->gear[i]);
		fprintf(f, "</gearbox>\n");
	}
	
	if (car->parts & CAR_PART_DIFF)
		fprintf(f, "<differential ratio=\"%f\" />\n", car->differential);
	
	if (car->parts & CAR_PART_TIRE)
		fprintf(f, "<tire width=\"%f\" ratio=\"%f\" rim=\"%f\"/>\n",
			car->tire_width, car->tire_ratio, car->tire_rim);
	
	if (car->parts & CAR_PART_WEIGHT)
		fprintf(f, "<weight value=\"%f\" />\n", car->weight);
	
	fprintf(f, "</car>\n");
	fclose(f);
	return TRUE;
}



typedef struct
{
	car_t *car;
	int level; /* 1-car, 2-engine, 3-gearbox */
	int nextrpm;
} xmlcar_t;


double
xmlcar_get_attribd(const gchar **attribute_names, const gchar **attribute_values, char *attr, GError **err)
{
	for (; *attribute_names; attribute_names++, attribute_values++)
		if (strcmp(*attribute_names, attr) == 0)
		{
			return strtod(*attribute_values, NULL);
		}
	g_set_error(err, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, "bad file structure (no attribute %s)", attr);
	return 0.0;
}

#define ATTRD(attr) xmlcar_get_attribd(attribute_names, attribute_values, attr, error)

#define ELEMENT(elem) (strcmp(element_name, elem) == 0)
 /* Called for open tags <foo bar="baz"> */
void xmlcar_start        (GMarkupParseContext *context,
                          const gchar         *element_name,
                          const gchar        **attribute_names,
                          const gchar        **attribute_values,
                          gpointer             user_data,
                          GError             **error)
{
	xmlcar_t *xc = (xmlcar_t *)user_data;
	car_t *car = xc->car;
	
	if (ELEMENT("car") && xc->level == 0)
		xc->level = 1;
	else if (ELEMENT("engine") && xc->level == 1)
	{
		const char **at, **av;
		gboolean minok=FALSE, maxok=FALSE;

		for (at=attribute_names, av=attribute_values; *at; at++, av++)
			if (strcmp(*at, "minrpm") == 0)
			{
				xc->car->minrpm = atoi(*av);
				xc->nextrpm = xc->car->minrpm;
				minok = TRUE;
			}
			else if (strcmp(*at, "maxrpm") == 0)
			{
				xc->car->maxrpm = atoi(*av);
				maxok = TRUE;
			}
		if (!minok || !maxok || car->minrpm < 0 || car->minrpm>=car->maxrpm || car->maxrpm >= MAXRPM)
			g_set_error(error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, "bad file structure (<car>: minrpm / maxrpm)");
			
		car->parts |= CAR_PART_ENGINE;
		xc->level = 2;
	}
	else if (ELEMENT("torque") && xc->level == 2)
		car->torque[xc->nextrpm++] = ATTRD("value");
	else if (ELEMENT("gearbox") && xc->level == 1)
	{
		xc->car->gears = 0;
		car->parts |= CAR_PART_GEARBOX;
		xc->level = 3;
	}
	else if (ELEMENT("gear") && xc->level == 3)
	{
		if (xc->car->gears == MAXGEARS)
		{
			g_set_error(error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, "bad file structure (max gears count is %d)", MAXGEARS);
			return;
		}
			
		car->gear[car->gears++] = ATTRD("ratio");
	}
	else if (ELEMENT("differential") && xc->level == 1)
	{
		car->differential = ATTRD("ratio");
		car->parts |= CAR_PART_DIFF;
	}
	else if (ELEMENT("tire") && xc->level == 1)
	{
		car->tire_width = ATTRD("width");
		car->tire_ratio = ATTRD("ratio");
		car->tire_rim = ATTRD("rim");
		car->parts |= CAR_PART_TIRE;
	}
	else if (ELEMENT("weight") && xc->level == 1)
	{
		car->weight = ATTRD("value");
		car->parts |= CAR_PART_WEIGHT;
	}
	else
		g_set_error(error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, "bad file structure (unsupported tag %s)", element_name);
}

  /* Called for close tags </foo> */
void xmlcar_end          (GMarkupParseContext *context,
                          const gchar         *element_name,
                          gpointer             user_data,
                          GError             **error)
{
	xmlcar_t *xc = (xmlcar_t *)user_data;
	
	if (ELEMENT("car") && xc->level == 1)
		xc->level = 0;
	else if (ELEMENT("engine") && xc->level == 2)
		xc->level = 1;
	else if (ELEMENT("gearbox") && xc->level == 3)
		xc->level = 1;
	/*else if (ELEMENT("torque") || ELEMENT("GEAR") || ELEMENT("differential") || ELEMENT())
	else
		g_set_error(error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, "bad file structure (bad closing tag %s)", element_name);*/
	
}


  /* Called on error, including one set by other
   * methods in the vtable. The GError should not be freed.
   */
void xmlcar_error        (GMarkupParseContext *context,
                          GError              *error,
                          gpointer             user_data)
{
	/*xmlcar_t *xc = (xmlcar_t *)user_data;*/
	printf("ERROR %s\n", error->message);
}


car_t *
carRead(char *file, GError **err)
{
	char *content;
	int flen;
	GMarkupParseContext *context;
	GMarkupParser parser;
	xmlcar_t *xc = g_new0(xmlcar_t,1);
	car_t *r;
	xc->car = carNew();
	g_return_val_if_fail (err == NULL || *err == NULL, FALSE);
	
	parser.start_element = xmlcar_start;
	parser.end_element = xmlcar_end;
	parser.text = NULL;
	parser.passthrough = NULL;
	parser.error = xmlcar_error;
	
	if (!g_file_get_contents(file, &content, &flen, err))
		goto err;
	
	context = g_markup_parse_context_new(&parser, 0, xc, NULL);
	
	if (!g_markup_parse_context_parse(context, content, flen, err))
		goto err;

	if (!g_markup_parse_context_end_parse(context, err))
		goto err;

	g_free(content);
	r = xc->car;
	g_free(xc);
	

	if (carCheck(r))
	{
		carCountHP(r);
		carCountWheelTorques(r);
		carCountTimeline(r);
	}
	
	return r;

err:
	g_free(content);
	carFree(xc->car);
	g_free(xc);
	return NULL;
}









