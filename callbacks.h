#include <gtk/gtk.h>


void
on_nowy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_otw__rz1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_zapisz1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_zapisz_jako1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_zako__cz1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_wytnij1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_skopiuj1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_wklej1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_usu__1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_informacje_o1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_spinRPM_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_btnUpdate_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_spinGear_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scaleGears_value_changed            (GtkRange        *range,
                                        gpointer         user_data);

gboolean
on_daTorque_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_daTorque_no_expose_event            (GtkWidget       *widget,
                                        GdkEventNoExpose *event,
                                        gpointer         user_data);

void
on_wndMain_show                        (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_wndTorque_show                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_spinWheel_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinDiffer_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_daTorque_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_daWheelForce_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_daWheelForce_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_daTorque_button_press_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_daTorque_button_release_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_daTorque_motion_notify_event        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_daTorque_button_event               (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_spinWheel_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinMaxTHP_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinMaxWForce_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinWForce_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_lblMaxSpeed_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinWForce_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_daTime_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_daTime_expose_event                 (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_spinTime_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinTSeconds_changed                (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinTime_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spinWeight_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_graph_image_window1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_draZoom_expose_event                (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_draImggraph_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_draZoom_configure_event             (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

void
on_btnOpen_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnClose_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnCopyCar_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnFillCurve_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnPoint1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnPoint2_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnPoint3_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_tglMode_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

gboolean
on_draImgGraph_configure_event         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_draImgGraph_button_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_draImgGraph_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_tglPath_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_tglCalibr1_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_tglCalibr2_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_tglCalibr3_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_spnCalibrRpm_value_changed          (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_spnCalibrValue_value_changed        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);
