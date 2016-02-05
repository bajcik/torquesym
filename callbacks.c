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

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "main.h"

#include "car-xml.h"
#include "wnd-imggraph.h"


/* ---------------------------------------------------------------------- */


gboolean updatingUI=FALSE;

void
UI2Car()
{
	int i;
	
	if (updatingUI)
		return;

	car->weight = gtk_spin_button_get_value(w->spinWeight);
	
	car->gears = gtk_range_get_value(GTK_RANGE(w->scaleGears));
	
	for (i=0; i<car->gears; i++)
		car->gear[i] = gtk_spin_button_get_value(w->spinGear[i]);

	car->differential = gtk_spin_button_get_value(w->spinDiffer);
	car->tire_width = gtk_spin_button_get_value(w->spinTireWidth);
	car->tire_ratio = gtk_spin_button_get_value(w->spinTireRatio);
	car->tire_rim = gtk_spin_button_get_value(w->spinRim);
	car->rwheel = WHEEL2R(car);
	
	gtk_statusbar_pop(w->sbMain, 1);
	if (carCheck(car))
	{
		carCountHP(car);
		carCountWheelTorques(car);
		carCountTimeline(car);
	}
	else
	{
		gtk_statusbar_push(w->sbMain, 1, car->checkerror->str);
	}
	
}

void
Car2UI(gboolean withinput)
{
	int i;
	char str0[50], str1[50];

	if (updatingUI)
		return;
	
	updatingUI = TRUE;

	if (withinput)
	{
		gtk_spin_button_set_value(w->spinWeight, car->weight);
		gtk_spin_button_set_value(w->spinMinRpm, car->minrpm);
		gtk_spin_button_set_value(w->spinMaxRpm, car->maxrpm);
		gtk_spin_button_set_value(w->spinTireRatio, car->tire_ratio);
		gtk_spin_button_set_value(w->spinTireWidth, car->tire_width);
		gtk_spin_button_set_value(w->spinRim, car->tire_rim);
		gtk_spin_button_set_value(w->spinDiffer, car->differential);
		gtk_range_set_value(GTK_RANGE(w->scaleGears), car->gears);
		gtk_spin_button_set_value(w->spinTSeconds, car->seconds);
	}
	
	for (i=0; i<MAXGEARS; i++)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(w->spinGear[i]), i<car->gears);
		if (withinput)
			gtk_spin_button_set_value(w->spinGear[i], car->gear[i]);
		if (i<car->gears)
		{
			snprintf(str0, 50, "%.1f", MS2KMH(carV(car, i, car->minrpm)));
			snprintf(str1, 50, "%.1f", MS2KMH(carV(car, i, car->maxrpm)));
		}
		else
			str0[0] = str1[0] = '\0';
		
		gtk_label_set_text(w->lblVMin[i], str0);
		gtk_label_set_text(w->lblVMax[i], str1);
	}
	
	snprintf(str0, 50, "%.1f@%d", car->hp[car->maxhpat], car->maxhpat);
	gtk_label_set_text(w->lblMaxHP, str0);
	snprintf(str0, 50, "%.1f@%d", car->torque[car->maxtorqueat], car->maxtorqueat);
	gtk_label_set_text(w->lblMaxTorque, str0);

	snprintf(str0, 50, "<i>%.0f</i>", car->rwheel*2.0*1000.0);
	gtk_label_set_markup(w->lblWheelHeight, str0);
	snprintf(str0, 50, "<i>%.0f</i>", car->rwheel*2.0*M_PI*1000.0);
	gtk_label_set_markup(w->lblWheelPerimeter, str0);
	
	for (i=0; i<VRECORDS; i++)
	{
		if (car->vtimes[i] > 0.0)
			snprintf(str0, 50, "<tt>%.02f</tt>", car->vtimes[i]);
		else
			snprintf(str0, 50, "-");
		gtk_label_set_markup(w->lblTV[i], str0);
	}
	for (i=0; i<SRECORDS; i++)
	{
		if (car->stimes[i] > 0.0)
			snprintf(str0, 50, "<tt>%.02f</tt>", car->stimes[i]);
		else
			snprintf(str0, 50, "-");
		gtk_label_set_markup(w->lblTS[i], str0);
	}
	
	gtk_widget_queue_draw(w->daTorque);
	gtk_widget_queue_draw(w->daWheelForce);
	gtk_widget_queue_draw(w->daTime);
	
	updatingUI = FALSE;
}

/* ---------------------------------------------------------------------- */

/* width - pixels
 * ret - table of return values
 * returns number of lines */
void mkscale(int width, double min, double max, double *rdv, double *v0, double *v1)
{
	double minpix = 50.0; /* min. pixel distance */
	double range = max-min;
	double dv = 100.0;

	/* into 10x bigger */
	while (width/range*dv < minpix)
		dv *= 10.0;
	
	/* into 10x smaller */
	while (width/range*dv*0.1 > minpix)
		dv *= 0.1;
	
	/* can't *0.1 any more, mabye *0.2 or *0.5 instead? */
	if (width/range*dv*0.2 > minpix)
		dv *= 0.2;
	else if (width/range*dv*0.5 > minpix)
		dv *= 0.5;
	
	*rdv = dv;
	*v0 = ceil(min/dv)*dv;
	*v1 = floor(max/dv)*dv;
}

void
DrawHScale(GdkDrawable *wnd, GdkGC *gc, int x, int y, int w, int h, double min, double max)
{
	double d, dy, y0, y1;

	mkscale(h, min, max, &dy, &y0, &y1);
	
	for (d=dy; d<max; d+=dy)
	{
		int    iy = y+h-d/(max-min)*h;
		gdk_draw_line(wnd, gc, x, iy, x+w, iy);
	}
}

void
DrawVScale(GdkDrawable *wnd, GdkGC *gc, int x, int y, int w, int h, double min, double max)
{
	double d, dx, x0, x1;

	mkscale(w, min, max, &dx, &x0, &x1);
	
	for (d=x0; d<max; d+=dx)
	{
		int    ix = d/(max-min)*w+x;
		gdk_draw_line(wnd, gc, ix, y, ix, y+h);
	}
}

/* ---------------------------------------------------------------------- */

void
erdlg(char *error)
{
	GtkWidget *errdlg = gtk_message_dialog_new(
		GTK_WINDOW(w->wndMain),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_CLOSE,
		error);
	gtk_dialog_run(GTK_DIALOG(errdlg));
	gtk_widget_destroy(errdlg);
}

void
on_nowy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	carFree(car);
	car = carNew();
	g_free(file_name);
	file_name = NULL;
	Car2UI(TRUE);
}


void
on_otw__rz1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GError *err = NULL;
	GtkWidget *chd = gtk_file_chooser_dialog_new(
		_("Open a car file"),
		GTK_WINDOW(w->wndMain),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	if (gtk_dialog_run(GTK_DIALOG(chd)) == GTK_RESPONSE_ACCEPT)
	{
		char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chd));
		car_t *newcar = carRead(fname,&err);
		if (newcar)
		{
			carFree(car);
			car = newcar;
			Car2UI(TRUE);
		}
		else
		{
			GtkWidget *errdlg = gtk_message_dialog_new_with_markup(
				GTK_WINDOW(w->wndMain),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE,
				_("Failed to open <i>%s</i> car file: <b>%s</b>"), fname, err->message);
			gtk_dialog_run(GTK_DIALOG(errdlg));
			gtk_widget_destroy(errdlg);
			
		}
		
		g_free(fname);
		g_clear_error(&err);
	}
	gtk_widget_destroy(chd);
}


void
on_zapisz1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (file_name)
	{
		GError *err = NULL;
		if (!carWrite(car, file_name, &err))
		{
			erdlg(err->message);
			g_clear_error(&err);
		}
	}
	else
		on_zapisz_jako1_activate(menuitem, user_data);
}


void
on_zapisz_jako1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *chd = gtk_file_chooser_dialog_new(
		_("Save car to file"),
		GTK_WINDOW(w->wndMain),
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
		NULL);

	if (gtk_dialog_run(GTK_DIALOG(chd)) == GTK_RESPONSE_ACCEPT)
	{
		char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chd));
		GError *err = NULL;
		
		if (!carWrite(car, fname, &err))
		{
			erdlg(err->message);
			g_clear_error(&err);
		}
		g_free(file_name);
		file_name = fname;
	}
	gtk_widget_destroy(chd);
}


void
on_zako__cz1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_graph_image_window1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GError *err = NULL;
	GtkWidget *chd = gtk_file_chooser_dialog_new(
		_("Open an image file"),
		GTK_WINDOW(w->wndMain),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	if (gtk_dialog_run(GTK_DIALOG(chd)) == GTK_RESPONSE_ACCEPT)
	{
		char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chd));
		if (!wnd_imgtorqueNewCreate(fname, car, &err))
		{
			GtkWidget *errdlg = gtk_message_dialog_new_with_markup(
				GTK_WINDOW(w->wndMain),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE,
				_("Failed to open <i>%s</i>: %s"), fname, err->message);
			gtk_dialog_run(GTK_DIALOG(errdlg));
			gtk_widget_destroy(errdlg);
			
		}
		
		g_free(fname);
	}
	g_clear_error(&err);
	gtk_widget_destroy(chd);
}


void
on_wytnij1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_skopiuj1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_wklej1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_usu__1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_informacje_o1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *about = gtk_about_dialog_new();
	
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "0.1");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "Krzysztof Garus");
	//gtk_about_dialog_set_licence(GTK_ABOUT_DIALOG(about), "GNU GPL v2.0");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), _("Engine torque/power simulator"));
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "http://kolos.math.uni.lodz.pl/~bajcik/programy.html#torquesym");
	
	gtk_dialog_run(GTK_DIALOG(about));
	
	gtk_widget_destroy(about);
}


void
on_spinRPM_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (!updatingUI)
		gtk_widget_set_sensitive(w->btnUpdate, TRUE);
}


void
on_btnUpdate_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	int minrpm,maxrpm;
	
	minrpm = gtk_spin_button_get_value(w->spinMinRpm);
	maxrpm = gtk_spin_button_get_value(w->spinMaxRpm);
	
	if (maxrpm > minrpm)
	{
		carRPMRange(car, minrpm, maxrpm, FALSE);
		carCountHP(car);
		carCountWheelTorques(car);
		gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
		Car2UI(FALSE);
	}
}


void
on_spinWeight_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	UI2Car();
	Car2UI(FALSE);
}

void
on_spinGear_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
	UI2Car();
	Car2UI(FALSE);
}


void
on_scaleGears_value_changed            (GtkRange        *range,
                                        gpointer         user_data)
{
	UI2Car();
	Car2UI(FALSE);
}


void
on_spinWheel_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
	UI2Car();
	Car2UI(FALSE);

}


void
on_spinDiffer_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	UI2Car();
	Car2UI(FALSE);
}


/* --------------------------------------------------------- Window  Torque/HP */

int Tw, Th;
double MaxT = 200.0, MaxHP = 100.0;
#define MARGIN 8


void
on_spinMaxTHP_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	MaxT = gtk_spin_button_get_value(w->spinMaxTorque);
	MaxHP = gtk_spin_button_get_value(w->spinMaxHP);
	gtk_widget_queue_draw(w->daTorque);
}

gboolean
on_daTorque_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	Tw = event->width - 2*MARGIN;
	Th = event->height - 2*MARGIN;
	return TRUE;
}


gboolean
on_daTorque_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	int i;
	
	if (!carCheck(car))
		return FALSE;

	DrawHScale(widget->window, widget->style->light_gc[GTK_STATE_NORMAL],
		MARGIN, MARGIN, Tw, Th, 0, MaxT);
	DrawHScale(widget->window, widget->style->light_gc[GTK_STATE_SELECTED],
		MARGIN, MARGIN, Tw, Th, 0, MaxHP);
	DrawVScale(widget->window, widget->style->light_gc[GTK_STATE_NORMAL],
		MARGIN, MARGIN, Tw, Th, car->minrpm, car->maxrpm);
	
	gdk_draw_rectangle(widget->window, widget->style->fg_gc[GTK_STATE_PRELIGHT], FALSE,
		MARGIN,MARGIN,Tw,Th);

	for (i=car->minrpm; i<car->maxrpm; i++)
	{
		int x,yt,yh;
		x = MARGIN + Tw * (i-car->minrpm)/car->ts;
		yt = MARGIN + Th - Th * car->torque[i] / MaxT;
		yh = MARGIN + Th - Th * car->hp[i] / MaxHP;

		gdk_draw_point(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], x, yt);
		gdk_draw_point(widget->window, widget->style->fg_gc[GTK_STATE_SELECTED], x, yh);
	}
	
	//printf("expose\n");
	return TRUE;
}



gboolean drawing = FALSE;
int prevrpm = 0;
double prevtorque = 0.0;

gboolean
on_daTorque_button_event               (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	int rpm = car->minrpm + (event->x-MARGIN) * car->ts/Tw;
	double torque = (Th-event->y+MARGIN) * MaxT / Th;
	int i, step;
	
	switch (event->type)
	{
		case GDK_BUTTON_PRESS:
			if (rpm < car->minrpm || rpm > car->maxrpm)
				return FALSE;
			//g_print("P");
			drawing = TRUE;
			prevrpm = rpm;
			prevtorque = torque;
		break;
		
		case GDK_BUTTON_RELEASE:
			//g_print("R");
			carCountHP(car);
			carCountWheelTorques(car);
			carCountTimeline(car);
			Car2UI(FALSE);
			gtk_widget_queue_draw(widget);
			gtk_widget_queue_draw(w->daWheelForce);
			drawing = FALSE;
		break;
		
		case GDK_MOTION_NOTIFY:
			if (!drawing)
				return FALSE;
			rpm = CLAMP(rpm, car->minrpm, car->maxrpm);
			step = (rpm>prevrpm) ? 1:-1;
			for (i=prevrpm; i!=rpm; i+= step)
			{
				double q=(double)(i-prevrpm) / (double)(rpm-prevrpm);
				car->torque[i] = prevtorque*(1-q) + torque*q;
				//g_print("%e %e\n", q, prevtorque*q + torque*(1-q));
			}
			prevrpm = rpm;
			prevtorque = torque;
			//g_print("m");
		break;
		default: break;
	}
	return TRUE;
}

/* --------------------------------------------------------- Window Wheel force */
int WFw, WFh;
double MaxF = 150.0;
double MaxV = 250.0;

void
on_spinWForce_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{
	MaxF = gtk_spin_button_get_value(w->spinMaxWForce);
	MaxV = gtk_spin_button_get_value(w->spinMaxSpeed);
	gtk_widget_queue_draw(w->daWheelForce);
}

gboolean
on_daWheelForce_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	WFw = event->width - 2*MARGIN;
	WFh = event->height - 2*MARGIN;
	return TRUE;
}


gboolean
on_daWheelForce_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	int g,i;

	if (!carCheck(car))
		return FALSE;
	
	DrawHScale(widget->window, widget->style->light_gc[GTK_STATE_NORMAL],
		MARGIN, MARGIN, WFw, WFh, 0, MaxF);
	DrawVScale(widget->window, widget->style->light_gc[GTK_STATE_NORMAL],
		MARGIN, MARGIN, WFw, WFh, 0, MaxV);
	
	gdk_draw_rectangle(widget->window, widget->style->fg_gc[GTK_STATE_PRELIGHT], FALSE,
		MARGIN,MARGIN,WFw,WFh);
	for (g=0; g<car->gears; g++)
		for (i=car->minrpm; i<=car->maxrpm; i++)
		{
			double v = MS2KMH(carV(car, g, i));
			//double f = carWheelForce(car, g, i);
			double f = car->wheeltorq[g][i];
			int x = MARGIN + v/MaxV*WFw;
			int y = MARGIN+WFh - f/MaxF*WFh;
			gdk_draw_point(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], x, y);
		}
	
	return TRUE;
}


/* --------------------------------------------------------- Window Speed / Road */


int VSw, VSh;
double MaxTA = 8.0;
double MaxTV = 250.0;
double MaxTS = 1000.0;
//double MaxSec = 26.0;

gboolean
on_daTime_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	VSw = event->width - 2*MARGIN;
	VSh = event->height - 2*MARGIN;
	return TRUE;
}


gboolean
on_daTime_expose_event                 (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	int i;

	if (!carCheck(car) || VSh<3)
		return FALSE;
	
	DrawHScale(widget->window, widget->style->light_gc[GTK_STATE_NORMAL],
		MARGIN, MARGIN, VSw, VSh, 0, MaxTV);
	DrawHScale(widget->window, widget->style->light_gc[GTK_STATE_PRELIGHT],
		MARGIN, MARGIN, VSw, VSh, 0, MaxTS);
	DrawVScale(widget->window, widget->style->light_gc[GTK_STATE_NORMAL],
		MARGIN, MARGIN, VSw, VSh, 0, car->seconds);
	
	gdk_draw_rectangle(widget->window, widget->style->fg_gc[GTK_STATE_PRELIGHT], FALSE,
		MARGIN,MARGIN,VSw,VSh);
	
	for (i=0; i<car->seconds*SECSAMPL; i++)
	{
		int x,ya,yv,ys;
		x = MARGIN + VSw * i/(car->seconds*SECSAMPL);
		ya = MARGIN + VSh - VSh * car->accel[i] / MaxTA;
		yv = MARGIN + VSh - VSh * MS2KMH(car->speed[i]) / MaxTV;
		ys = MARGIN + VSh - VSh * car->road[i] / MaxTS;
		//printf("%d [%d,%d/%d] V=%f s=%f\n", i, x, yv, ys, MS2KMH(car->speed[i]), car->road[i]);

		gdk_draw_point(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], x, ya);
		gdk_draw_point(widget->window, widget->style->fg_gc[GTK_STATE_SELECTED], x, yv);
		gdk_draw_point(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], x, ys);
	}
	
	return TRUE;
}


void
on_spinTime_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
	MaxTA = gtk_spin_button_get_value(w->spinTAccel);
	MaxTV = gtk_spin_button_get_value(w->spinTSpeed);
	MaxTS = gtk_spin_button_get_value(w->spinTRoad);
	gtk_widget_queue_draw(w->daTime);
}


void
on_spinTSeconds_changed                (GtkEditable     *editable,
                                        gpointer         user_data)
{
	carSeconds(car, gtk_spin_button_get_value(w->spinTSeconds));
	carCountTimeline(car);
	//gtk_widget_queue_draw(w->daTime);
	Car2UI(FALSE);
}

void
on_wndMain_show                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
}


void
on_wndTorque_show                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
}


