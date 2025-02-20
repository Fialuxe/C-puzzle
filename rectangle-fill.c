/**
 * 20250220作成
 * gtkを用いたGUIプログラミング
 * 四角形が移動できるという機能のみ実装
 * やり残し：
 * 四角形が画面外に移動した時の修正動作(たまに元に戻らない)
 * ウィンドウサイズによる処理の対応(現状ウィンドウサイズが変わらない前提)
 * 四角形を二つ置いておいて、片方は動かない、もう片方は動く。動かない方に動く方を重ねたらクリア。パズルのようなゲームを作る。
*/

#include <gtk/gtk.h>
#define WIDTH_RECTANGLE 100
#define HEIGHT_RECTANGLE 100
#define WIDTH_WINDOW 400
#define HEIGHT_WINDOW 400
#define X_INITIAL 100
#define Y_INITIAL 100
static gint x_offset = X_INITIAL;
static gint y_offset = Y_INITIAL;
static gboolean is_dragging = FALSE;

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data){
    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0); //青色に設定
    cairo_rectangle(cr, x_offset, y_offset, WIDTH_RECTANGLE, HEIGHT_RECTANGLE); // 始点座標(x_offset, y_offset)として、幅、高さを設定した四角形を描写
    cairo_fill(cr);
    return FALSE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data){
    //ドラッグ開始
    gboolean    is_x_inside_rectangle,
                is_y_inside_rectangle;
    is_x_inside_rectangle = x_offset <= event->x && event->x <= x_offset + WIDTH_RECTANGLE;
    is_y_inside_rectangle = y_offset <= event->y && event->y <= y_offset + HEIGHT_RECTANGLE;
    if(is_x_inside_rectangle && is_y_inside_rectangle){
        is_dragging = TRUE;
        g_print("Drag started at %.1f, %.1f\n", event->x, event->y);
    }
    return FALSE;
}

static gboolean on_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer data){
    //ドラッグ中
    if(is_dragging){
        //四角形をマウスの中心に
        x_offset = event->x - (WIDTH_RECTANGLE / 2);
        y_offset  =event->y - (HEIGHT_RECTANGLE / 2);
        //再描写する
        gtk_widget_queue_draw(widget);
    }
    g_print("Mouse moved at %.1f, %.1f\n", event->x, event->y);
    return FALSE;
}

static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data){
    //ドラッグ終了
    if(is_dragging){
        is_dragging = FALSE;
        g_print("Drag ended at %.1f, %.1f\n", event->x, event->y);
        //ウィンドウの左上の座標を取得する
        gint window_x, window_y;
        gtk_window_get_position(GTK_WINDOW(gtk_widget_get_toplevel(widget)), &window_x, &window_y);
        g_print("Window placed on %4d,%4d", window_x, window_y);
        if( x_offset < window_x || window_x + WIDTH_WINDOW < x_offset|| 
            y_offset < window_y || window_y + HEIGHT_WINDOW < y_offset){
                x_offset = window_x;
                y_offset  =window_y;
                gtk_widget_queue_draw(widget);
            }
    }
    return FALSE;
}

int main(int argc, char *argv[]){
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Drag and Drop");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH_WINDOW, HEIGHT_WINDOW);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), NULL);
    g_signal_connect(drawing_area, "motion-notify-event", G_CALLBACK(on_motion_notify), NULL);
    g_signal_connect(drawing_area, "button-release-event", G_CALLBACK(on_button_release), NULL);

    gtk_widget_set_events(drawing_area, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;

}