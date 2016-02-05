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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

#include "main.h"
#include "car.h"

widgets_t *w;
car_t *car;
char *file_name = NULL;

void
LocateWidgets(widgets_t *w)
{
	int i;
	GString *str = g_string_new("");
	w->btnUpdate = lookup_widget(w->wndMain, "btnUpdate");
	for (i=0; i<MAXGEARS; i++)
	{
		g_string_printf(str, "spinGear%d", i+1);
		w->spinGear[i] = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, str->str));
		g_string_printf(str, "lblVMin%d", i+1);
		w->lblVMin[i] = GTK_LABEL(lookup_widget(w->wndMain, str->str));
		g_string_printf(str, "lblVMax%d", i+1);
		w->lblVMax[i] = GTK_LABEL(lookup_widget(w->wndMain, str->str));
	}
	w->spinWeight = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinWeight"));
	w->lblMaxTorque = GTK_LABEL(lookup_widget(w->wndMain, "lblMaxTorque"));
	w->lblMaxHP = GTK_LABEL(lookup_widget(w->wndMain, "lblMaxHP"));
	w->scaleGears = GTK_SCALE(lookup_widget(w->wndMain, "scaleGears"));
	w->spinMinRpm = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinMinRpm"));
	w->spinMaxRpm = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinMaxRpm"));
	w->spinRim = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinRim"));
	w->spinTireWidth = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinTireWidth"));
	w->spinTireRatio = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinTireRatio"));
	w->spinDiffer = GTK_SPIN_BUTTON(lookup_widget(w->wndMain, "spinDiffer"));
	w->lblWheelHeight = GTK_LABEL(lookup_widget(w->wndMain, "lblWheelHeight"));
	w->lblWheelPerimeter = GTK_LABEL(lookup_widget(w->wndMain, "lblWheelPerimeter"));
	w->sbMain = GTK_STATUSBAR(lookup_widget(w->wndMain, "sbMain"));

	w->daTorque = lookup_widget(w->wndTorque, "daTorque");
	w->spinMaxTorque = GTK_SPIN_BUTTON(lookup_widget(w->wndTorque, "spinMaxTorque"));
	w->spinMaxHP = GTK_SPIN_BUTTON(lookup_widget(w->wndTorque, "spinMaxHP"));
	
	w->daWheelForce = lookup_widget(w->wndWheelForce, "daWheelForce");
	w->spinMaxWForce = GTK_SPIN_BUTTON(lookup_widget(w->wndWheelForce, "spinMaxWForce"));
	w->spinMaxSpeed = GTK_SPIN_BUTTON(lookup_widget(w->wndWheelForce, "spinMaxSpeed"));
	
	w->daTime = lookup_widget(w->wndTime, "daTime");
	w->spinTAccel = GTK_SPIN_BUTTON(lookup_widget(w->wndTime, "spinTAccel"));
	w->spinTSpeed = GTK_SPIN_BUTTON(lookup_widget(w->wndTime, "spinTSpeed"));
	w->spinTRoad = GTK_SPIN_BUTTON(lookup_widget(w->wndTime, "spinTRoad"));
	w->spinTSeconds = GTK_SPIN_BUTTON(lookup_widget(w->wndTime, "spinTSeconds"));
	
	for (i=0; i<VRECORDS; i++)
	{
		g_string_printf(str, "lblTV%d", i);
		w->lblTV[i] = GTK_LABEL(lookup_widget(w->wndPerf, str->str));
	}
	for (i=0; i<SRECORDS; i++)
	{
		g_string_printf(str, "lblTS%d", i);
		w->lblTS[i] = GTK_LABEL(lookup_widget(w->wndPerf, str->str));
	}
	g_string_free(str, TRUE);
}


void Car2UI(gboolean);

int
main (int argc, char *argv[])
{

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  //gtk_set_locale ();
  gtk_init (&argc, &argv);

  //add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  w = g_new0(widgets_t, 1); 
  w->wndMain = create_wndMain ();
  w->wndTorque = create_wndTorque ();
  w->wndWheelForce = create_wndWheelForce ();
  w->wndTime = create_wndTime ();
  w->wndPerf = create_wndPerf ();
  LocateWidgets(w);
  
  car = carNew();
  Car2UI(TRUE);
  
  gtk_widget_show (w->wndMain);
  gtk_widget_show (w->wndPerf);
  gtk_widget_show (w->wndTorque);
  gtk_widget_show (w->wndWheelForce);
  gtk_widget_show (w->wndTime);

  gtk_main ();
  return 0;
}

