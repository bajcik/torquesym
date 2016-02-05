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
#include <stdio.h>
#include <math.h>
#include <glib.h>
#include <gdk/gdk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "main.h"

#include "wnd-imggraph.h"
#include "imgtorque.h"


void change_mode(wnd_imggraph_t *w, int mode);


/* --------------------------------------- exported functions */

/**
 * wnd_imgtorqueNewCreate:
 * @char *fname, car_t *car
 *
 *
 * Returns: 
 */
gboolean
wnd_imgtorqueNewCreate(char *fname, car_t *car, GError **err)
{
	wnd_imggraph_t *w;
	GdkPixbuf *pbuf = gdk_pixbuf_new_from_file(fname, err);
	int width,height;
	
	if (!pbuf)
		return FALSE;
	
	w = g_new0(wnd_imggraph_t,1);
	w->car = car;
	w->wnd = create_wndImgGraph ();
	g_object_set_data (G_OBJECT (w->wnd), "wnd_imggraph_t", w);
	w->draImgGraph = lookup_widget(w->wnd, "draImgGraph");
	w->draZoom = lookup_widget(w->wnd, "draZoom");
	w->tglPath = lookup_widget(w->wnd, "tglPath");
	w->tglCalibr1 = lookup_widget(w->wnd, "tglCalibr1");
	w->tglCalibr2 = lookup_widget(w->wnd, "tglCalibr2");
	w->tglCalibr3 = lookup_widget(w->wnd, "tglCalibr3");
	w->spnCalibrRpm = lookup_widget(w->wnd, "spnCalibrRpm");
	w->spnCalibrValue = lookup_widget(w->wnd, "spnCalibrValue");
	w->pixbuf = pbuf;
	w->pixels = gdk_pixbuf_get_pixels (pbuf);
	width  = gdk_pixbuf_get_width(pbuf);
	height = gdk_pixbuf_get_height(pbuf);
	w->it = imgtorqueNew(width,height);
	gtk_widget_set_size_request(w->draImgGraph, width, height);
	change_mode(w, 0);
	gtk_widget_show(w->wnd);

	return TRUE;
}



/**
 * wnd_imgtorqueFree:
 * @wnd_imggraph_t *wnd
 *
 *
 * Returns: 
 */
void               
wnd_imgtorqueFree(wnd_imggraph_t *w)
{
	gdk_pixbuf_unref(w->pixbuf);
	g_object_unref(w->wnd); // TODO
	g_free(w);
}




/* -------------------------------- callbacks */
#define LOOKUP_W(widget) (wnd_imggraph_t *)lookup_widget(GTK_WIDGET(widget),"wnd_imggraph_t")

gboolean
on_draImgGraph_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	int i;
	wnd_imggraph_t *w = LOOKUP_W(widget);

	gdk_pixbuf_render_to_drawable(w->pixbuf,
		w->draImgGraph->window,
		w->draImgGraph->style->black_gc,
		event->area.x, event->area.y, /* src x,y */
		event->area.x, event->area.y, /* dest x,y */
		MIN(event->area.width, w->it->w),
		MIN(event->area.height, w->it->h),
		GDK_RGB_DITHER_NONE,
		0, 0);
	for (i=0; i<3; i++)
		if (w->it->cp[i].set)
			gdk_draw_rectangle(widget->window, widget->style->fg_gc[GTK_STATE_PRELIGHT],
			w->mode-1 == i,
			w->it->cp[i].x-1,
			w->it->cp[i].y-1,
			3,3);
  return TRUE;
}


gboolean
on_draImgGraph_configure_event         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{

  return FALSE;
}

#define ZOOM 6
#define pixR(x,y) w->pixels[(y)*rowstride + (x)*3+0]
#define pixG(x,y) w->pixels[(y)*rowstride + (x)*3+1]
#define pixB(x,y) w->pixels[(y)*rowstride + (x)*3+2]
gboolean
on_draZoom_expose_event                (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	wnd_imggraph_t *w = LOOKUP_W(widget);
	GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(widget->window));
	int rowstride = gdk_pixbuf_get_rowstride(w->pixbuf);
	int swidth, sheight;
	int sx_s, sx_e, sy_s, sy_e;
	int sx, sy;

	swidth = (w->zoomw+ZOOM-1) / ZOOM;
	sheight = (w->zoomh+ZOOM-1) / ZOOM;
	sx_s = w->mousex-swidth/2; sx_e = sx_s + swidth;
	sy_s = w->mousey-sheight/2; sy_e = sy_s + sheight;

	// FIXME niedokładne, nieeleganckie
	for (sx=MAX(sx_s,0); sx<MIN(sx_e,w->it->w); sx++)
		for (sy=MAX(sy_s,0); sy<MIN(sy_e,w->it->h); sy++)
		{
			GdkColor color;
			
			color.red = pixR(sx,sy)*255;
			color.green = pixG(sx,sy)*255;
			color.blue = pixB(sx,sy)*255;
			gdk_gc_set_rgb_fg_color(gc, &color);

			gdk_draw_rectangle(GDK_DRAWABLE(widget->window), gc,
				TRUE,
				(sx-sx_s)*ZOOM,
				(sy-sy_s)*ZOOM,
				ZOOM, ZOOM);
		}

	// FIXME niedokładne, nieeleganckie
	gdk_draw_rectangle(GDK_DRAWABLE(widget->window), widget->style->black_gc,
		FALSE,
		(w->zoomw-ZOOM)/2-2,
		(w->zoomh-ZOOM)/2-2,
		ZOOM+2, ZOOM+2);
	
	gdk_gc_unref(gc);
	return TRUE;
}



gboolean
on_draZoom_configure_event             (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	wnd_imggraph_t *w = LOOKUP_W(widget);
	w->zoomw = event->width;
	w->zoomh = event->height;
	return TRUE;
}


void
on_btnCopyCar_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btnFillCurve_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}




gboolean
on_draImgGraph_button_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	wnd_imggraph_t *w = LOOKUP_W(widget);

	if (event->x != w->mousex || event->y != w->mousey)
	{
		w->mousex = event->x;
		w->mousey = event->y;
		gtk_widget_queue_draw(w->draZoom);
	}
	
	if (event->type == GDK_BUTTON_PRESS && w->mode>=1 && w->mode<=3)
	{
		w->it->cp[w->mode-1].x = event->x;
		w->it->cp[w->mode-1].y = event->y;
		w->it->cp[w->mode-1].set = TRUE;
		gtk_widget_queue_draw_area(widget, event->x-1, event->y-1, 3, 3);
	}
	
	return TRUE;
}


void
change_mode(wnd_imggraph_t *w, int mode)
{
	int i;

	if (w->updating_ui)
		return; /* signal from another change_mode() */
	
	w->updating_ui = TRUE;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->tglPath), mode==0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->tglCalibr1), mode==1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->tglCalibr2), mode==2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->tglCalibr3), mode==3);

	if (mode>=1 && mode<=3)
	{
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(w->spnCalibrRpm), w->it->cp[mode-1].rpm);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(w->spnCalibrValue), w->it->cp[mode-1].value);
	}
	
	w->updating_ui = FALSE;

	gtk_widget_set_sensitive(w->spnCalibrRpm, mode>0);
	gtk_widget_set_sensitive(w->spnCalibrValue, mode>0);
	for (i=0; i<3; i++)
		gtk_widget_queue_draw_area(w->draImgGraph, w->it->cp[i].x-1, w->it->cp[i].y-1, 3, 3);

	w->mode = mode;
}


void
on_tglPath_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	change_mode(LOOKUP_W(togglebutton), 0);
}


void
on_tglCalibr1_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	change_mode(LOOKUP_W(togglebutton), 1);
}


void
on_tglCalibr2_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	change_mode(LOOKUP_W(togglebutton), 2);
}


void
on_tglCalibr3_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	change_mode(LOOKUP_W(togglebutton), 3);
}



void
on_spnCalibrRpm_value_changed          (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	wnd_imggraph_t *w = LOOKUP_W(spinbutton);
	if (!w->updating_ui)
		w->it->cp[w->mode-1].rpm = gtk_spin_button_get_value_as_int(spinbutton);
}


void
on_spnCalibrValue_value_changed        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	wnd_imggraph_t *w = LOOKUP_W(spinbutton);
	if (!w->updating_ui)
		w->it->cp[w->mode-1].value = gtk_spin_button_get_value(spinbutton);
}


