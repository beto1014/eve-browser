#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>



static GtkWidget* main_window;
static GtkWidget* uri_entry;
static GtkStatusbar* main_statusbar;
static GtkWidget* toolbar;
static GtkToolItem* itemUrl;
static GtkScrolledWindow* scrolled_window;
static WebKitWebView* web_view;
static gchar* main_title;
static gdouble load_progress;
static guint status_context_id;
static GtkWidget* window ;
static GtkWidget* vbox;

static void setTransparent(GtkWidget *widget);

///////////////////////////


void printJSStringRef(JSStringRef string)
{
    unsigned int len = JSStringGetLength(string);
    char * buffer = (char * )malloc(len+1);
    JSStringGetUTF8CString(string, buffer, len+1);
    printf("%s", buffer);
    free(buffer);
}

void printJSObjectRef(JSContextRef ctx, JSObjectRef argument)
{

    JSPropertyNameArrayRef names = JSObjectCopyPropertyNames(ctx, argument);
    unsigned int propertySize = JSPropertyNameArrayGetCount(names);
    printf("%s > (propertySize=%d)\n", __func__, propertySize);
    for(unsigned int i = 0; i < propertySize; i++)
    {
        JSStringRef name = JSPropertyNameArrayGetNameAtIndex(names, i);
        printf("[%2d] ", i); printJSStringRef(name); printf("\n");
    }
    printf("%s < \n", __func__);
}

void printJSValueRef(JSContextRef ctx, JSValueRef argument, JSValueRef *exception)
{
    printf("%s > \n", __func__);
    JSType type = JSValueGetType(ctx, argument);
    switch(type)
    {
    case kJSTypeUndefined: printf("kJSTypeUndefined\n"); break;
    case kJSTypeNull: printf("kJSTypeNull\n"); break;
    case kJSTypeBoolean: 
        printf("kJSTypeBoolean\n"); 
        printf("%s\n", JSValueToBoolean(ctx, argument)?"True":"False"); 
        break;
    case kJSTypeNumber: 
        printf("kJSTypeNumber\n"); 
        printf("%f\n", JSValueToNumber(ctx, argument, exception)); 
        break;

    case kJSTypeString: 
        printf("kJSTypeString\n"); 
        JSStringRef s;
        s = JSValueToStringCopy(ctx, argument, exception);
        printJSStringRef(s); 
        JSStringRelease(s);
        break;

    case kJSTypeObject: 
        printf("kJSTypeObject\n"); 
        JSObjectRef o = JSValueToObject(ctx, argument, exception);
        printJSObjectRef(ctx, o); 
        break;

    }
    printf("%s < \n", __func__);
}



///////////////////////////

// HTMLObjectElement - Bindings
// Visibility setting is a workaround to get video object invisible 
char s_o_bindToCurrentChannel[] = 
    "HTMLObjectElement.prototype.bindToCurrentChannel = function() { \
    c_o_bindToCurrentChannel(); \
    document.getElementById(\"video\").style.visibility = \"hidden\"; \
}";

JSValueRef
c_o_bindToCurrentChannel (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    /*printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    printJSObjectRef(ctx, function);

    printJSObjectRef(ctx, thisObject);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i]);
    }*/

    return NULL;
}

char s_o_getChannelConfig[] = 
"HTMLObjectElement.prototype.getChannelConfig = function() {\
    c_o_getChannelConfig(); \
	return new Channel();\
}";

JSValueRef
c_o_getChannelConfig (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED\n", __func__);
    return NULL;
}

char s_o_play[] = 
"HTMLObjectElement.prototype.play = function(speed) {\
    c_o_play(speed, this.data); \
}";

JSValueRef
c_o_play (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_stop[] = 
"HTMLObjectElement.prototype.stop = function() {\
    c_o_stop(); \
}";

JSValueRef
c_o_stop (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_release[] = 
"HTMLObjectElement.prototype.release = function() {\
    c_o_release(); \
}";

JSValueRef
c_o_release (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_setFullScreen[] = 
"HTMLObjectElement.prototype.setFullScreen = function(show) {\
    c_o_setFullScreen(show); \
}";

JSValueRef
c_o_setFullScreen (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_seek[] = 
"HTMLObjectElement.prototype.seek = function(millis) {\
    c_o_seek(millis); \
}";

JSValueRef
c_o_seek (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

///////////////////////////

// Registering single funxtions
void register_javascript_function(const char *name, JSObjectCallAsFunctionCallback callback)
{
    WebKitWebFrame *frame = webkit_web_view_get_main_frame(WEBKIT_WEB_VIEW(web_view));
    JSContextRef ctx = webkit_web_frame_get_global_context(frame);
    JSObjectRef global = JSContextGetGlobalObject(ctx);
    JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, NULL, callback);
    JSStringRef jsname = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(ctx, global, jsname, func,0, NULL);
    JSStringRelease(jsname);
}

static void window_object_cleared_cb(   WebKitWebView *frame,
                                        gpointer context,
                                        gpointer arg3,
                                        gpointer user_data)
{
    //JSGlobalContextRef jsContext = webkit_web_frame_get_globale_context(frame);
    //adJSClasses(jsContext)
    printf("window_object_cleared_cb\n");

    webkit_web_view_execute_script(web_view, s_o_bindToCurrentChannel);
    register_javascript_function("c_o_bindToCurrentChannel", c_o_bindToCurrentChannel);

    webkit_web_view_execute_script(web_view, s_o_play);
    register_javascript_function("c_o_play", c_o_play);

    webkit_web_view_execute_script(web_view, s_o_stop);
    register_javascript_function("c_o_stop", c_o_stop);

    webkit_web_view_execute_script(web_view, s_o_release);
    register_javascript_function("c_o_release", c_o_release);

    webkit_web_view_execute_script(web_view, s_o_setFullScreen);
    register_javascript_function("c_o_setFullScreen", c_o_setFullScreen);

    webkit_web_view_execute_script(web_view, s_o_seek);
    register_javascript_function("c_o_seek", c_o_seek);
}

///////////////////////7

static void
activate_uri_entry_cb (GtkWidget* entry, gpointer data)
{
    const gchar* uri = gtk_entry_get_text (GTK_ENTRY (entry));
    g_assert (uri);
    webkit_web_view_load_uri (web_view, uri);
}

static void
update_title (GtkWindow* window)
{
    GString* string = g_string_new (main_title);
    g_string_append (string, " - WebKit Launcher");
    if (load_progress < 100)
        g_string_append_printf (string, " (%f%%)", load_progress);
    gchar* title = g_string_free (string, FALSE);
    gtk_window_set_title (window, title);
    g_free (title);
}

static void
link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data)
{
    /* underflow is allowed */
    gtk_statusbar_pop (main_statusbar, status_context_id);
    if (link)
        gtk_statusbar_push (main_statusbar, status_context_id, link);
}

static void
notify_title_cb (WebKitWebView* web_view, GParamSpec* pspec, gpointer data)
{
    if (main_title)
        g_free (main_title);
    main_title = g_strdup (webkit_web_view_get_title(web_view));
    update_title (GTK_WINDOW (main_window));
}

static void
notify_load_status_cb (WebKitWebView* web_view, GParamSpec* pspec, gpointer data)
{
    if (webkit_web_view_get_load_status (web_view) == WEBKIT_LOAD_COMMITTED) {
        WebKitWebFrame* frame = webkit_web_view_get_main_frame (web_view);
        const gchar* uri = webkit_web_frame_get_uri (frame);
        if (uri)
            gtk_entry_set_text (GTK_ENTRY (uri_entry), uri);
    }
}

static void
notify_progress_cb (WebKitWebView* web_view, GParamSpec* pspec, gpointer data)
{
    load_progress = webkit_web_view_get_progress (web_view) * 100;
    update_title (GTK_WINDOW (main_window));
}

static void
destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}


static gboolean
focus_out_cb (GtkWidget* widget, GdkEvent * event, gpointer data)
{
    printf("%s > \n", __func__);
    gtk_widget_grab_focus(widget);
    return false;
}


static void
document_load_finished_cb (GtkWidget* widget, WebKitWebFrame * arg1, gpointer data)
{
//char scriptBackgroundColor[] = "document.styleSheets[0].insertRule(\'a{backgroundColor=\"#FF0000\";}\',0);";

/*char scriptBackgroundColor[] = "var cssTags = document.getElementsByTagName(\"style\"); \
var cssTag = cssTags[0]; \
alert(cssTag.innerText); \
                      cssTag.innerText += \'a { backgroundColor = \"#FF0000\"; }\'; \
alert(cssTag.innerText);";*/

/*char scriptBackgroundColor[] = "var cssTag = document.createElement(\"style\"); \
                      cssTag.type = \"text/css\"; \
                      cssTag.innerHtml = \'a { backgroundColor = \"#FF0000\"; }\'; \
                      document.getElementsByTagName(\"head\")[0].appendChild(cssTag);";*/
/*
                      cssTag.getElementsByTagName.innerHtml = \'a { backgroundColor = \"#FF0000\" }\'; \

char scriptBackgroundColor[] = "var a = document.getElementsByTagName(\"a\"); \
                for(var i=0;i<a.length;i++){ \
                     a[i].style.backgroundColor = \"#FFA4A4\"; \
                     a[i].style.border = \"#FF0000 solid medium\"; \
                     a[i].style.borderRadius = \"15px\"; \
                 }";*/
//webkit_web_view_execute_script(web_view, scriptBackgroundColor);


/*char scriptHover[] = "var cssTag = document.createElement(\"style\"); \
                      cssTag.setAttribute(\"type\",\"text/css\"); \
                      cssTag.innerHtml = \'a:focus { backgroundColor = \"#00FF00\" }\'; \
                      document.body.appendChild(cssTag);";*/

/*char scriptHover[] = "var a = document.getElementsByTagName(\"*\"); \
                for(var i=0;i<a.length;i++){ \
					 a[i].onfocus = function() { \
                                                this.style.backgroundColor= \"#87FF87\"; \
                                                this.style.border = \"#00FF00 solid medium\"; \
                                                this.style.borderRadius = \"15px\"; \
                     }; \
					 a[i].onblur = function() { \
                                                this.style.backgroundColor= \"#FFA4A4\"; \
                                                this.style.border = \"#FF0000 solid medium\"; \
                                                this.style.borderRadius = \"15px\"; \
                     }; \
                 }";

webkit_web_view_execute_script(web_view, scriptHover);*/

//char scriptError[] = "alert(hbbtvlib_lastError);";
//webkit_web_view_execute_script(web_view, scriptError);
}


void goBack()
{
    webkit_web_view_go_back(web_view);
}

void gtk_widget_set_can_focus(GtkWidget* wid, gboolean can)
{
    if(can)
        GTK_WIDGET_SET_FLAGS(wid, GTK_CAN_FOCUS);
    else
        GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS);
}

static GtkScrolledWindow*
create_browser ()
{
    scrolled_window = (GtkScrolledWindow*)gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
    webkit_web_view_set_transparent(web_view, true);
    gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (web_view));
    //setTransparent(GTK_WIDGET (scrolled_window));
    //setTransparent(GTK_WIDGET (web_view));
    webkit_web_view_set_full_content_zoom(web_view, true);

    g_signal_connect (web_view, "notify::title", G_CALLBACK (notify_title_cb), web_view);
    g_signal_connect (web_view, "notify::load-status", G_CALLBACK (notify_load_status_cb), web_view);
    g_signal_connect (web_view, "notify::progress", G_CALLBACK (notify_progress_cb), web_view);
    g_signal_connect (web_view, "hovering-over-link", G_CALLBACK (link_hover_cb), web_view);

    g_signal_connect (web_view, "focus-out-event", G_CALLBACK (focus_out_cb), web_view);

    g_signal_connect (web_view, "document-load-finished", G_CALLBACK (document_load_finished_cb), web_view);

    g_signal_connect (web_view, "window_object_cleared", G_CALLBACK (window_object_cleared_cb), web_view);

    return scrolled_window;
}

static GtkWidget*
create_statusbar ()
{
    main_statusbar = GTK_STATUSBAR (gtk_statusbar_new ());
    gtk_widget_set_can_focus(GTK_WIDGET (main_statusbar), false);
    status_context_id = gtk_statusbar_get_context_id (main_statusbar, "Link Hover");
    
    return (GtkWidget*)main_statusbar;
}

static GtkWidget*
create_toolbar ()
{
    toolbar = gtk_toolbar_new ();
    gtk_widget_set_can_focus(GTK_WIDGET (toolbar), false);

#if GTK_CHECK_VERSION(2,15,0)
    gtk_orientable_set_orientation (GTK_ORIENTABLE (toolbar), GTK_ORIENTATION_HORIZONTAL);
#else
    gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar), GTK_ORIENTATION_HORIZONTAL);
#endif
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);


    /* The URL entry */
    itemUrl = gtk_tool_item_new ();
    gtk_widget_set_can_focus(GTK_WIDGET (itemUrl), false);
    gtk_tool_item_set_expand (itemUrl, TRUE);
    uri_entry = gtk_entry_new ();
    gtk_container_add (GTK_CONTAINER (itemUrl), uri_entry);
    g_signal_connect (G_OBJECT (uri_entry), "activate", G_CALLBACK (activate_uri_entry_cb), NULL);
    gtk_toolbar_insert (GTK_TOOLBAR (toolbar), itemUrl, -1);

    return toolbar;
}

static void
toogleBackground (void)
{
    printf("%s > \n", __func__);
    static gboolean isTransparent = false;
    isTransparent = !isTransparent;
    webkit_web_view_set_transparent(web_view, isTransparent);
}

static gboolean isShown = true;

static void
toogleMode (void)
{
    printf("%s > \n", __func__);
    if(isShown)
    {
        gtk_widget_set_size_request(vbox, 1280, 720);
        gtk_widget_hide(GTK_WIDGET (main_statusbar));
        gtk_widget_hide(GTK_WIDGET (toolbar));
    }
    else
    {
        gtk_widget_set_size_request(vbox, 1080, 720);
        gtk_widget_show(GTK_WIDGET (main_statusbar));
        gtk_widget_show(GTK_WIDGET (toolbar));

        gtk_widget_grab_focus(GTK_WIDGET (itemUrl));
    }
    isShown = !isShown;
}

static gboolean gIsNumLock = false;
static void toogleNumLock()
{
    gIsNumLock = !gIsNumLock;
}

static gboolean gIsZoomLock = false;
static void toggleZoomLock()
{
    gIsZoomLock = !gIsZoomLock;
}

static void handleZoomLock(int value)
{
    if(value > 0)
        webkit_web_view_zoom_in(web_view);
    else if(value < 0)
        webkit_web_view_zoom_out(web_view);
    else
        webkit_web_view_set_zoom_level(web_view, 1.0f);
}

static gboolean
on_key_press (GtkWidget* widget, GdkEventKey *event, gpointer data)
{
    if (event->type == GDK_KEY_PRESS)
    {
        printf("KeyPress: %02X\n", event->keyval);
        printf("KeyState: %02X\n", event->state);
        printf("KeyString: %s\n", event->string);

        if(gIsZoomLock)
        {
            switch(event->keyval)
            {
            case GDK_Up:     handleZoomLock(+1); break;
            case GDK_Return: handleZoomLock(0); break;
            case GDK_Down:   handleZoomLock(-1); break;
            default: break;
            }
        }

        if (event->keyval == GDK_F1)
        {
            toogleMode();
        }
        else if (isShown && event->keyval == GDK_F2)
        {
            //toogleBackground();
            toggleZoomLock();
        }
        else if (isShown && event->keyval == GDK_Num_Lock)
        {
            toogleNumLock();
        }
        else if (isShown && event->keyval == GDK_F4)
        {
            gboolean rtv;
            event->keyval = GDK_Tab;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_F3)
        {
            gboolean rtv;
            event->keyval = GDK_Tab;
            event->state |= GDK_SHIFT_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval >= 0xFFB0 && event->keyval <= 0xFFB9)
        {
            gIsNumLock = true;
            //event->state |= GDK_MOD1_MASK;
            switch(event->keyval) {
            case GDK_KP_1: event->keyval = '.'; break;
            case GDK_KP_2: event->keyval = 'a'; break;
            case GDK_KP_3: event->keyval = 'd'; break;
            case GDK_KP_4: event->keyval = 'g'; break;
            case GDK_KP_5: event->keyval = 'j'; break;
            case GDK_KP_6: event->keyval = 'm'; break;
            case GDK_KP_7: event->keyval = 'p'; break;
            case GDK_KP_8: event->keyval = 't'; break;
            case GDK_KP_9: event->keyval = 'w'; break;
            case GDK_KP_0: event->keyval = '+'; break;
            default: break;
            }
        }
        else if (isShown && event->keyval == GDK_KP_Down )
        {
            gboolean rtv;
            event->keyval = '2';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Up )
        {
            gboolean rtv;
            event->keyval = '8';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Left )
        {
            gboolean rtv;
            event->keyval = '4';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Right )
        {
            gboolean rtv;
            event->keyval = '6';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Home )
        {
            gboolean rtv;
            event->keyval = '7';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Page_Up )
        {
            gboolean rtv;
            event->keyval = '9';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_End )
        {
            gboolean rtv;
            event->keyval = '1';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Page_Down )
        {
            gboolean rtv;
            event->keyval = '3';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Insert || event->keyval == 0xFFFFFF)
        {
            gboolean rtv;
            event->keyval = '0';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Begin )
        {
            gboolean rtv;
            event->keyval = '5';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (window), "key-press-event", event, &rtv);
            return true;
        }
        else if (event->keyval == GDK_BackSpace)
            goBack();
    }
    return false;
}


static gboolean expose_event(GtkWidget * widget, GdkEventExpose * event)
{
printf("%s\n", __func__);
cairo_t *cr;

cr = gdk_cairo_create(GDK_DRAWABLE(widget->window));
gdk_cairo_region(cr, event->region);
cairo_clip(cr);

cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1.0);
cairo_paint(cr);
cairo_destroy(cr);
return false;
}

static GtkWidget*
create_window ()
{
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 1280, 720);
    gtk_widget_set_name (window, "GtkLauncher");
    gtk_window_set_decorated(GTK_WINDOW(window), false);

    GdkScreen *screen = gtk_widget_get_screen(window);
    gtk_widget_set_colormap(window, gdk_screen_get_rgba_colormap(screen));
    gtk_widget_set_app_paintable(window, true);
    gtk_widget_realize(window);
    gdk_window_set_back_pixmap(window->window, NULL, false);

    g_signal_connect(window, "expose-event", G_CALLBACK(expose_event), window);

    g_signal_connect (window, "destroy", G_CALLBACK (destroy_cb), NULL);
    g_signal_connect (window, "key-press-event", G_CALLBACK (on_key_press), NULL);

    //g_signal_connect (window, "expose-event", G_CALLBACK (expose_event), NULL);
    //g_signal_connect (window, "screen-changed", G_CALLBACK (screen_changed), NULL);
    return window;
}


int
main (int argc, char* argv[])
{
    gtk_init (&argc, &argv);
    if (!g_thread_supported ())
        g_thread_init (NULL);


    GtkWidget* fixed = gtk_fixed_new();
    //screen_changed(fixed, NULL, NULL);
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), create_toolbar (), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (create_browser ()), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), create_statusbar (), FALSE, FALSE, 0);

    main_window = create_window ();

    gtk_fixed_put(GTK_FIXED(fixed), vbox, 0, 0);
    gtk_widget_set_size_request(vbox, 1280, 720);

    GtkWidget* statusLabel = gtk_label_new ("Status");
    gtk_fixed_put(GTK_FIXED(fixed), statusLabel, 1080, 0);
    gtk_widget_set_size_request(statusLabel, 200, 100);




    gtk_container_add (GTK_CONTAINER (main_window), fixed);

    gchar* uri = (gchar*) (argc > 1 ? argv[1] : "http://www.google.com/");
    webkit_web_view_load_uri (web_view, uri);

    gtk_widget_grab_focus (GTK_WIDGET (web_view));
    gtk_widget_show_all (main_window);

    toogleMode();

    gtk_main ();

    return 0;
}