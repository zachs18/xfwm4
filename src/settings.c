/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; You may only use version 2 of the License,
	you have no option to use any other version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

        oroborus - (c) 2001 Ken Lynch
        xfwm4    - (c) 2002 Olivier Fourdan

 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <pango/pango.h>
#include <gtk/gtk.h>
#include "main.h"
#include "parserc.h"
#include "client.h"
#include "gtk_style.h"
#include "gtktoxevent.h"
#include "workspaces.h"
#include "debug.h"

MyKey keys[KEY_COUNT];
MyColor title_colors[2];
char button_layout[8];
int title_alignment;
int full_width_title;
int button_spacing;
int button_offset;
int title_vertical_offset;
int double_click_action;
int box_move;
int box_resize;
int click_to_focus;
int focus_new;
int raise_on_focus;
int raise_delay;
int snap_to_border;
int snap_width;
GC box_gc;
MyPixmap sides[3][2];
MyPixmap corners[4][2];
MyPixmap buttons[BUTTON_COUNT][3];
MyPixmap title[5][2];

void loadSettings()
{
    Settings rc[] = {
        {"active_text_color", NULL, FALSE},
        {"inactive_text_color", NULL, FALSE},
        {"active_border_color", NULL, FALSE},
        {"inactive_border_color", NULL, FALSE},
        {"active_color_1", NULL, FALSE},
        {"active_shadow_1", NULL, FALSE},
        {"inactive_shadow_1", NULL, FALSE},
        {"active_hilight_1", NULL, FALSE},
        {"inactive_hilight_1", NULL, FALSE},
        {"active_color_2", NULL, FALSE},
        {"inactive_color_2", NULL, FALSE},
        {"active_shadow_2", NULL, FALSE},
        {"inactive_shadow_2", NULL, FALSE},
        {"active_hilight_2", NULL, FALSE},
        {"inactive_hilight_2", NULL, FALSE},
        {"theme", NULL, TRUE},
        {"title_alignment", NULL, TRUE},
        {"full_width_title", NULL, TRUE},
        {"button_layout", NULL, TRUE},
        {"button_spacing", NULL, TRUE},
        {"title_vertical_offset", NULL, TRUE},
        {"button_offset", NULL, TRUE},
        {"double_click_action", NULL, TRUE},
        {"box_move", NULL, TRUE},
        {"box_resize", NULL, TRUE},
        {"click_to_focus", NULL, TRUE},
        {"focus_new", NULL, TRUE},
        {"raise_on_focus", NULL, TRUE},
        {"raise_delay", NULL, TRUE},
        {"snap_to_border", NULL, TRUE},
        {"snap_width", NULL, TRUE},
        {"workspace_count", NULL, TRUE},
        {"wrap_workspaces", NULL, TRUE},
        {"close_window_key", NULL, TRUE},
        {"hide_window_key", NULL, TRUE},
        {"maximize_window_key", NULL, TRUE},
        {"maximize_vert_key", NULL, TRUE},
        {"maximize_horiz_key", NULL, TRUE},
        {"shade_window_key", NULL, TRUE},
        {"lower_window_layer_key", NULL, TRUE},
        {"raise_window_layer_key", NULL, TRUE},
        {"cycle_windows_key", NULL, TRUE},
        {"move_window_up_key", NULL, TRUE},
        {"move_window_down_key", NULL, TRUE},
        {"move_window_left_key", NULL, TRUE},
        {"move_window_right_key", NULL, TRUE},
        {"resize_window_up_key", NULL, TRUE},
        {"resize_window_down_key", NULL, TRUE},
        {"resize_window_left_key", NULL, TRUE},
        {"resize_window_right_key", NULL, TRUE},
        {"next_workspace_key", NULL, TRUE},
        {"prev_workspace_key", NULL, TRUE},
        {"add_workspace_key", NULL, TRUE},
        {"del_workspace_key", NULL, TRUE},
        {"stick_window_key", NULL, TRUE},
        {"workspace_1_key", NULL, TRUE},
        {"workspace_2_key", NULL, TRUE},
        {"workspace_3_key", NULL, TRUE},
        {"workspace_4_key", NULL, TRUE},
        {"workspace_5_key", NULL, TRUE},
        {"workspace_6_key", NULL, TRUE},
        {"workspace_7_key", NULL, TRUE},
        {"workspace_8_key", NULL, TRUE},
        {"workspace_9_key", NULL, TRUE},
        {"move_window_next_workspace_key", NULL, TRUE},
        {"move_window_prev_workspace_key", NULL, TRUE},
        {"move_window_workspace_1_key", NULL, TRUE},
        {"move_window_workspace_2_key", NULL, TRUE},
        {"move_window_workspace_3_key", NULL, TRUE},
        {"move_window_workspace_4_key", NULL, TRUE},
        {"move_window_workspace_5_key", NULL, TRUE},
        {"move_window_workspace_6_key", NULL, TRUE},
        {"move_window_workspace_7_key", NULL, TRUE},
        {"move_window_workspace_8_key", NULL, TRUE},
        {"move_window_workspace_9_key", NULL, TRUE},
        {NULL, NULL, FALSE}
    };
    char *theme;
    XpmColorSymbol colsym[16];
    GtkWidget *widget;

    DBG("entering settingsLoad\n");

    widget = getDefaultGtkWidget();

    rc[0].value = get_style(widget, "text", "selected");
    rc[1].value = get_style(widget, "text", "normal");
    rc[2].value = get_style(widget, "fg", "active");
    rc[3].value = get_style(widget, "fg", "normal");
    rc[4].value = get_style(widget, "bg", "selected");
    rc[5].value = get_style(widget, "light", "selected");
    rc[6].value = get_style(widget, "dark", "selected");
    rc[7].value = get_style(widget, "bg", "normal");
    rc[8].value = get_style(widget, "light", "normal");
    rc[9].value = get_style(widget, "dark", "normal");
    rc[10].value = get_style(widget, "bg", "active");
    rc[11].value = get_style(widget, "light", "active");
    rc[12].value = get_style(widget, "dark", "active");
    rc[13].value = get_style(widget, "bg", "normal");
    rc[14].value = get_style(widget, "light", "normal");
    rc[15].value = get_style(widget, "dark", "normal");

    if(!parseRc("defaults", DATADIR, rc))
    {
        fprintf(stderr, "%s: Missing defaults file\n", progname);
        exit(1);
    }
    if(!checkRc(rc))
    {
        fprintf(stderr, "%s: Missing values in defaults file\n", progname);
        exit(1);
    }

    parseRc(".xfwm4rc", getenv("HOME"), rc);
    theme = getValue("theme", rc);
    parseRc("themerc", theme, rc);

    colsym[0].name = "active_text_color";
    colsym[0].value = rc[0].value;

    colsym[1].name = "inactive_text_color";
    colsym[1].value = rc[1].value;

    colsym[2].name = "active_border_color";
    colsym[2].value = rc[2].value;

    colsym[3].name = "inactive_border_color";
    colsym[3].value = rc[3].value;

    colsym[4].name = "active_color_1";
    colsym[4].value = rc[4].value;

    colsym[5].name = "active_hilight_1";
    colsym[5].value = rc[5].value;

    colsym[6].name = "active_shadow_1";
    colsym[6].value = rc[6].value;

    colsym[7].name = "active_color_2";
    colsym[7].value = rc[7].value;

    colsym[8].name = "active_hilight_2";
    colsym[8].value = rc[8].value;

    colsym[9].name = "active_shadow_2";
    colsym[9].value = rc[9].value;

    colsym[10].name = "inactive_color_1";
    colsym[10].value = rc[10].value;

    colsym[11].name = "inactive_hilight_1";
    colsym[11].value = rc[11].value;

    colsym[12].name = "inactive_shadow_1";
    colsym[12].value = rc[12].value;

    colsym[13].name = "inactive_color_2";
    colsym[13].value = rc[13].value;

    colsym[14].name = "inactive_hilight_2";
    colsym[14].value = rc[14].value;

    colsym[15].name = "inactive_shadow_2";
    colsym[15].value = rc[15].value;

    if(title_colors[ACTIVE].allocated)
    {
        gdk_colormap_free_colors(gdk_colormap_get_system(), &title_colors[ACTIVE].col, 1);
        title_colors[ACTIVE].allocated = FALSE;
    }
    if(gdk_color_parse(rc[0].value, &title_colors[ACTIVE].col))
    {
        if(gdk_colormap_alloc_color(gdk_colormap_get_system(), &title_colors[ACTIVE].col, FALSE, FALSE))
        {
            title_colors[ACTIVE].allocated = TRUE;
            if(title_colors[ACTIVE].gc)
            {
                g_object_unref(G_OBJECT(title_colors[ACTIVE].gc));
            }
            title_colors[ACTIVE].gc = gdk_gc_new(getDefaultGdkWindow());
            gdk_gc_copy(title_colors[ACTIVE].gc, get_style_gc(widget, "text", "selected"));
            gdk_gc_set_foreground(title_colors[ACTIVE].gc, &title_colors[ACTIVE].col);
        }
        else
        {
            gdk_beep();
            g_message("Cannot allocate active color %s\n", rc[0].value);
        }
    }
    else
    {
        gdk_beep();
        g_message("Cannot parse active color %s\n", rc[0].value);
    }

    if(title_colors[INACTIVE].allocated)
    {
        gdk_colormap_free_colors(gdk_colormap_get_system(), &title_colors[INACTIVE].col, 1);
        title_colors[INACTIVE].allocated = FALSE;
    }
    if(gdk_color_parse(rc[1].value, &title_colors[INACTIVE].col))
    {
        if(gdk_colormap_alloc_color(gdk_colormap_get_system(), &title_colors[INACTIVE].col, FALSE, FALSE))
        {
            title_colors[INACTIVE].allocated = TRUE;
            if(title_colors[INACTIVE].gc)
            {
                g_object_unref(G_OBJECT(title_colors[INACTIVE].gc));
            }
            title_colors[INACTIVE].gc = gdk_gc_new(getDefaultGdkWindow());
            gdk_gc_copy(title_colors[INACTIVE].gc, get_style_gc(widget, "text", "normal"));
            gdk_gc_set_foreground(title_colors[INACTIVE].gc, &title_colors[INACTIVE].col);
        }
        else
        {
            gdk_beep();
            g_message("Cannot allocate inactive color %s\n", rc[1].value);
        }
    }
    else
    {
        gdk_beep();
        g_message("Cannot parse inactive color %s\n", rc[1].value);
    }

    loadPixmap(dpy, &sides[SIDE_LEFT][ACTIVE], theme, "left-active.xpm", colsym, 16);
    loadPixmap(dpy, &sides[SIDE_LEFT][INACTIVE], theme, "left-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &sides[SIDE_RIGHT][ACTIVE], theme, "right-active.xpm", colsym, 16);
    loadPixmap(dpy, &sides[SIDE_RIGHT][INACTIVE], theme, "right-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &sides[SIDE_BOTTOM][ACTIVE], theme, "bottom-active.xpm", colsym, 16);
    loadPixmap(dpy, &sides[SIDE_BOTTOM][INACTIVE], theme, "bottom-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_TOP_LEFT][ACTIVE], theme, "top-left-active.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_TOP_LEFT][INACTIVE], theme, "top-left-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_TOP_RIGHT][ACTIVE], theme, "top-right-active.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_TOP_RIGHT][INACTIVE], theme, "top-right-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_BOTTOM_LEFT][ACTIVE], theme, "bottom-left-active.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_BOTTOM_LEFT][INACTIVE], theme, "bottom-left-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_BOTTOM_RIGHT][ACTIVE], theme, "bottom-right-active.xpm", colsym, 16);
    loadPixmap(dpy, &corners[CORNER_BOTTOM_RIGHT][INACTIVE], theme, "bottom-right-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[HIDE_BUTTON][ACTIVE], theme, "hide-active.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[HIDE_BUTTON][INACTIVE], theme, "hide-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[HIDE_BUTTON][PRESSED], theme, "hide-pressed.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[CLOSE_BUTTON][ACTIVE], theme, "close-active.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[CLOSE_BUTTON][INACTIVE], theme, "close-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[CLOSE_BUTTON][PRESSED], theme, "close-pressed.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[MAXIMIZE_BUTTON][ACTIVE], theme, "maximize-active.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[MAXIMIZE_BUTTON][INACTIVE], theme, "maximize-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[MAXIMIZE_BUTTON][PRESSED], theme, "maximize-pressed.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[SHADE_BUTTON][ACTIVE], theme, "shade-active.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[SHADE_BUTTON][INACTIVE], theme, "shade-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[SHADE_BUTTON][PRESSED], theme, "shade-pressed.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[STICK_BUTTON][ACTIVE], theme, "stick-active.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[STICK_BUTTON][INACTIVE], theme, "stick-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[STICK_BUTTON][PRESSED], theme, "stick-pressed.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[MENU_BUTTON][ACTIVE], theme, "menu-active.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[MENU_BUTTON][INACTIVE], theme, "menu-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &buttons[MENU_BUTTON][PRESSED], theme, "menu-pressed.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_1][ACTIVE], theme, "title-1-active.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_1][INACTIVE], theme, "title-1-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_2][ACTIVE], theme, "title-2-active.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_2][INACTIVE], theme, "title-2-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_3][ACTIVE], theme, "title-3-active.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_3][INACTIVE], theme, "title-3-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_4][ACTIVE], theme, "title-4-active.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_4][INACTIVE], theme, "title-4-inactive.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_5][ACTIVE], theme, "title-5-active.xpm", colsym, 16);
    loadPixmap(dpy, &title[TITLE_5][INACTIVE], theme, "title-5-inactive.xpm", colsym, 16);

    if(!g_ascii_strcasecmp("left", getValue("title_alignment", rc)))
    {
        title_alignment = ALIGN_LEFT;
    }
    else if(!g_ascii_strcasecmp("right", getValue("title_alignment", rc)))
    {
        title_alignment = ALIGN_RIGHT;
    }
    else
    {
        title_alignment = ALIGN_CENTER;
    }
    full_width_title = !g_ascii_strcasecmp("true", getValue("full_width_title", rc));

    strncpy(button_layout, getValue("button_layout", rc), 7);
    button_spacing = atoi(getValue("button_spacing", rc));
    button_offset = atoi(getValue("button_offset", rc));
    title_vertical_offset = atoi(getValue("title_vertical_offset", rc));

    box_gc = createGC(dpy, cmap, "#FFFFFF", GXxor, NULL, True);
    box_resize = !g_ascii_strcasecmp("true", getValue("box_resize", rc));
    box_move = !g_ascii_strcasecmp("true", getValue("box_move", rc));

    click_to_focus = !g_ascii_strcasecmp("true", getValue("click_to_focus", rc));
    focus_new = !g_ascii_strcasecmp("true", getValue("focus_new", rc));
    raise_on_focus = !g_ascii_strcasecmp("true", getValue("raise_on_focus", rc));
    raise_delay = abs(atoi(getValue("raise_delay", rc)));

    snap_to_border = !g_ascii_strcasecmp("true", getValue("snap_to_border", rc));
    snap_width = abs(atoi(getValue("snap_width", rc)));

    if(!g_ascii_strcasecmp("shade", getValue("double_click_action", rc)))
    {
        double_click_action = ACTION_SHADE;
    }
    else if(!g_ascii_strcasecmp("hide", getValue("double_click_action", rc)))
    {
        double_click_action = ACTION_HIDE;
    }
    else if(!g_ascii_strcasecmp("maximize", getValue("double_click_action", rc)))
    {
        double_click_action = ACTION_MAXIMIZE;
    }
    else
    {
        double_click_action = ACTION_NONE;
    }

    if(workspace_count < 0)
    {
        unsigned long data[1];
        workspace_count = abs(atoi(getValue("workspace_count", rc)));
        setGnomeHint(dpy, root, win_workspace_count, workspace_count);
        data[0] = workspace_count;
        XChangeProperty(dpy, root, net_number_of_desktops, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
    }
    wrap_workspaces = !g_ascii_strcasecmp("true", getValue("wrap_workspaces", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_UP], getValue("move_window_up_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_DOWN], getValue("move_window_down_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_LEFT], getValue("move_window_left_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_RIGHT], getValue("move_window_right_key", rc));
    parseKeyString(dpy, &keys[KEY_RESIZE_UP], getValue("resize_window_up_key", rc));
    parseKeyString(dpy, &keys[KEY_RESIZE_DOWN], getValue("resize_window_down_key", rc));
    parseKeyString(dpy, &keys[KEY_RESIZE_LEFT], getValue("resize_window_left_key", rc));
    parseKeyString(dpy, &keys[KEY_RESIZE_RIGHT], getValue("resize_window_right_key", rc));
    parseKeyString(dpy, &keys[KEY_CYCLE_WINDOWS], getValue("cycle_windows_key", rc));
    parseKeyString(dpy, &keys[KEY_CLOSE_WINDOW], getValue("close_window_key", rc));
    parseKeyString(dpy, &keys[KEY_HIDE_WINDOW], getValue("hide_window_key", rc));
    parseKeyString(dpy, &keys[KEY_MAXIMIZE_WINDOW], getValue("maximize_window_key", rc));
    parseKeyString(dpy, &keys[KEY_MAXIMIZE_VERT], getValue("maximize_vert_key", rc));
    parseKeyString(dpy, &keys[KEY_MAXIMIZE_HORIZ], getValue("maximize_horiz_key", rc));
    parseKeyString(dpy, &keys[KEY_SHADE_WINDOW], getValue("shade_window_key", rc));
    parseKeyString(dpy, &keys[KEY_LOWER_WINDOW_LAYER], getValue("lower_window_layer_key", rc));
    parseKeyString(dpy, &keys[KEY_RAISE_WINDOW_LAYER], getValue("raise_window_layer_key", rc));
    parseKeyString(dpy, &keys[KEY_NEXT_WORKSPACE], getValue("next_workspace_key", rc));
    parseKeyString(dpy, &keys[KEY_PREV_WORKSPACE], getValue("prev_workspace_key", rc));
    parseKeyString(dpy, &keys[KEY_ADD_WORKSPACE], getValue("add_workspace_key", rc));
    parseKeyString(dpy, &keys[KEY_DEL_WORKSPACE], getValue("del_workspace_key", rc));
    parseKeyString(dpy, &keys[KEY_STICK_WINDOW], getValue("stick_window_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_1], getValue("workspace_1_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_2], getValue("workspace_2_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_3], getValue("workspace_3_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_4], getValue("workspace_4_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_5], getValue("workspace_5_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_6], getValue("workspace_6_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_7], getValue("workspace_7_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_8], getValue("workspace_8_key", rc));
    parseKeyString(dpy, &keys[KEY_WORKSPACE_9], getValue("workspace_9_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_NEXT_WORKSPACE], getValue("move_window_next_workspace_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_PREV_WORKSPACE], getValue("move_window_prev_workspace_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_1], getValue("move_window_workspace_1_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_2], getValue("move_window_workspace_2_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_3], getValue("move_window_workspace_3_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_4], getValue("move_window_workspace_4_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_5], getValue("move_window_workspace_5_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_6], getValue("move_window_workspace_6_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_7], getValue("move_window_workspace_7_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_8], getValue("move_window_workspace_8_key", rc));
    parseKeyString(dpy, &keys[KEY_MOVE_WORKSPACE_9], getValue("move_window_workspace_9_key", rc));
    ungrabKeys(dpy, gnome_win);
    grabKey(dpy, &keys[KEY_CYCLE_WINDOWS], gnome_win);
    grabKey(dpy, &keys[KEY_NEXT_WORKSPACE], gnome_win);
    grabKey(dpy, &keys[KEY_PREV_WORKSPACE], gnome_win);
    grabKey(dpy, &keys[KEY_ADD_WORKSPACE], gnome_win);
    grabKey(dpy, &keys[KEY_NEXT_WORKSPACE], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_1], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_2], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_3], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_4], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_5], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_6], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_7], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_8], gnome_win);
    grabKey(dpy, &keys[KEY_WORKSPACE_9], gnome_win);
    freeRc(rc);
}

void unloadSettings()
{
    int i;
    DBG("entering unloadSettings\n");
    for(i = 0; i < 3; i++)
    {
        freePixmap(dpy, &sides[i][ACTIVE]);
        freePixmap(dpy, &sides[i][INACTIVE]);
    }
    for(i = 0; i < BUTTON_COUNT; i++)
    {
        freePixmap(dpy, &corners[i][ACTIVE]);
        freePixmap(dpy, &corners[i][INACTIVE]);
        freePixmap(dpy, &buttons[i][ACTIVE]);
        freePixmap(dpy, &buttons[i][INACTIVE]);
        freePixmap(dpy, &buttons[i][PRESSED]);
    }
    for(i = 0; i < 5; i++)
    {
        freePixmap(dpy, &title[i][ACTIVE]);
        freePixmap(dpy, &title[i][INACTIVE]);
    }
    XFreeGC(dpy, box_gc);
}

void reloadSettings()
{
    Client *c;
    int i;
    XWindowChanges wc;
    DBG("entering reloadSettings\n");
    for(c = clients, i = 0; i < client_count; c = c->next, i++)
    {
        clientGravitate(c, REMOVE);
        clientUngrabKeys(c);
    }
    unloadSettings();
    loadSettings();
    for(c = clients, i = 0; i < client_count; c = c->next, i++)
    {
        clientGravitate(c, APPLY);
        wc.x = c->x;
        wc.y = c->y;
        wc.width = c->width;
        wc.height = c->height;
        clientConfigure(c, &wc, CWX | CWY | CWWidth | CWHeight);
        clientGrabKeys(c);
    }
}

void initSettings(void)
{
    title_colors[ACTIVE].gc = NULL;
    title_colors[ACTIVE].allocated = FALSE;
    title_colors[INACTIVE].gc = NULL;
    title_colors[INACTIVE].allocated = FALSE;
}
