#include "entrance_client.h"

typedef struct Entrance_Int_Conf_Main_
{
   struct
     {
        Evas_Object *preview;
        const char *path;
        const char *group;
     } bg;
   Evas_Object *display_area;
   const char *theme;
   const char *elm_profile;
   Eina_Bool vkbd_enabled : 1;
   double scale;
   Eina_Bool update : 1;
} Entrance_Int_Conf_Main;

static void _entrance_conf_main_begin(void);
static void _entrance_conf_main_end(void);
static Eina_Bool _entrance_conf_bg_fill_cb(void *data, Elm_Object_Item *it);
static void _entrance_conf_bg_sel(void *data, Evas_Object *obj, void *event_info);
static void _entrance_conf_vkbd_changed(void *data, Evas_Object *obj, void *event);
static void _entrance_conf_scale_changed(void *data, Evas_Object *obj, void *event);
static Evas_Object *_entrance_conf_main_build(Evas_Object *obj);
static Eina_Bool _entrance_conf_main_check(void);
static void _entrance_conf_main_apply(void);

static Entrance_Int_Conf_Main *_entrance_int_conf_main = NULL;


static void
_entrance_conf_main_begin(void)
{
   _entrance_int_conf_main = calloc(1, sizeof(Entrance_Int_Conf_Main));
   _entrance_int_conf_main->theme = entrance_gui_theme_name_get();
   entrance_gui_background_get(&(_entrance_int_conf_main->bg.path),
                               &(_entrance_int_conf_main->bg.group));
   _entrance_int_conf_main->vkbd_enabled = entrance_gui_vkbd_enabled_get();
   _entrance_int_conf_main->scale = elm_config_scale_get();
   _entrance_int_conf_main->elm_profile = elm_config_profile_get();

}

static void
_entrance_conf_main_end(void)
{
   free(_entrance_int_conf_main);
}

static Eina_Bool
_entrance_conf_bg_fill_cb(void *data, Elm_Object_Item *it)
{
   Entrance_Conf_Background *cbg;
   const char *bg_path, *bg_group;
   cbg = data;

   entrance_gui_background_get(&bg_path, &bg_group);
   if (((cbg->path) && (bg_path)
         && (!strcmp(cbg->path, bg_path))) ||
       ((!cbg->path) && (!bg_path)))
     {
        if  (((cbg->group) && (bg_group)
              && (!strcmp(cbg->group, bg_group))) ||
            ((!cbg->group) && (!bg_group)))
          {
             elm_gengrid_item_selected_set(it, EINA_TRUE);
             return EINA_TRUE;
          }
     }
   return EINA_FALSE;
}

static void
_entrance_conf_bg_sel(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Entrance_Conf_Background *cbg;
   cbg = elm_object_item_data_get(event_info);
   _entrance_int_conf_main->bg.path = cbg->path;
   _entrance_int_conf_main->bg.group = cbg->group;
   entrance_conf_changed();
}

/* vkbd */
static void
_entrance_conf_vkbd_changed(void *data EINA_UNUSED, Evas_Object *obj, void *event EINA_UNUSED)
{
   Elm_Actionslider_Pos p;

   PT("User change vkbd state\n");

   p = elm_actionslider_indicator_pos_get(obj);
   _entrance_int_conf_main->vkbd_enabled = !!(p == ELM_ACTIONSLIDER_RIGHT);
   entrance_conf_changed();
}

/* scale */
static void
_entrance_conf_scale_changed(void *data EINA_UNUSED, Evas_Object *obj, void *event EINA_UNUSED)
{
   _entrance_int_conf_main->scale = elm_spinner_value_get(obj);
   entrance_conf_changed();
}

static void
_entrance_conf_toolbar_click(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *o = data, *old;

   Eina_List *childs = elm_box_children_get(_entrance_int_conf_main->display_area);
   old = eina_list_data_get(childs);
   elm_box_unpack(_entrance_int_conf_main->display_area, old);

   evas_object_hide(old);

   elm_box_pack_end(_entrance_int_conf_main->display_area, o);
   evas_object_show(o);
}

static Evas_Object*
_entrance_conf_main_general(Evas_Object *obj)
{
   Evas_Object *t, *o;

   /* General */
   t = elm_table_add(obj);
   elm_table_padding_set(t, 0, 0);
   evas_object_size_hint_weight_set(t, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(t, EVAS_HINT_FILL, EVAS_HINT_FILL);

   /* Touch Screen */
   o = elm_label_add(obj);
   elm_object_text_set(o, "Use a virtual keyboard");
   evas_object_size_hint_weight_set(o, 1, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(t, o, 0, 0, 1, 1);
   evas_object_show(o);

   o = elm_actionslider_add(obj);
   elm_object_style_set(o, "bar");
   evas_object_size_hint_weight_set(o, 1, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_text_set(o, "left", "Disabled");
   elm_object_part_text_set(o, "right", "Enabled");

   elm_actionslider_magnet_pos_set(o, ELM_ACTIONSLIDER_LEFT);
   elm_actionslider_enabled_pos_set(o, ELM_ACTIONSLIDER_LEFT |
                                    ELM_ACTIONSLIDER_RIGHT);
   evas_object_smart_callback_add(o, "selected",
                                  _entrance_conf_vkbd_changed, obj);
   if (_entrance_int_conf_main->vkbd_enabled)
     elm_actionslider_indicator_pos_set(o, ELM_ACTIONSLIDER_RIGHT);
   else
     elm_actionslider_indicator_pos_set(o, ELM_ACTIONSLIDER_LEFT);
   elm_table_pack(t, o, 1, 0, 1, 1);
   evas_object_show(o);

   /* Elementary Profile */
   o = elm_label_add(obj);
   elm_object_text_set(o, "elementary profile - NOT WORKING");
   evas_object_size_hint_weight_set(o, 1, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(t, o, 0, 1, 1, 1);
   evas_object_show(o);

   o = elm_hoversel_add(obj);
   elm_object_text_set(o, _entrance_int_conf_main->elm_profile);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(t, o, 1, 1, 1, 1);
   evas_object_show(o);

   /* Scaling */
   o = elm_label_add(t);
   elm_object_text_set(o, "Scaling");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(t, o, 0, 2, 1, 1);
   evas_object_show(o);
   o = elm_spinner_add(t);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(o, "changed",
                                  _entrance_conf_scale_changed, NULL);
   elm_spinner_value_set(o, _entrance_int_conf_main->scale);
   elm_spinner_min_max_set(o, 0.25, 5.0);
   elm_spinner_step_set(o, 0.15);
   elm_spinner_label_format_set(o, "%.2f");

   elm_table_pack(t, o, 1, 2, 1, 1);
   evas_object_show(o);
   return t;
}

static Evas_Object*
_entrance_conf_main_background(Evas_Object *obj)
{
   Evas_Object *o, *bx;
   Eina_List *s_bg, *t_bg, *tmp = NULL, *node = NULL;

   o = bx = elm_box_add(obj);
   elm_box_horizontal_set(o, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);

   o = elm_label_add(obj);
   elm_object_text_set(o, "Background");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, o);
   evas_object_show(o);

   o = elm_gengrid_add(obj);
   elm_gengrid_item_size_set(o,
                             elm_config_scale_get() * 150,
                             elm_config_scale_get() * 150);
   elm_gengrid_group_item_size_set(o,
                                   elm_config_scale_get() * 31,
                                   elm_config_scale_get() * 31);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, o);
   evas_object_show(o);

   s_bg = entrance_gui_background_pool_get();
   t_bg = entrance_gui_theme_backgrounds();

#define LIST_FILL(list) \
   tmp = NULL; \
   IMG_LIST_FORK(list, tmp); \
   entrance_fill(o, entrance_conf_background_fill_get(),\
                 tmp, _entrance_conf_bg_fill_cb,\
                 _entrance_conf_bg_sel, o);

   LIST_FILL(s_bg);
   LIST_FILL(t_bg);

#undef LIST_FILL

   return bx;
}

static Evas_Object*
_entrance_conf_main_themesel(Evas_Object *obj)
{
   Evas_Object *o;
   /* Theme selector */
   o = elm_label_add(obj);
   elm_object_text_set(o, "TODO Implement theme selector!");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   return o;
}

static Evas_Object*
_entrance_conf_main_graph_log(Evas_Object *obj)
{
   Evas_Object *o;
   /* Graphical Log */
   o = elm_label_add(obj);
   elm_object_text_set(o, "TODO Implement Graphical Log !");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   return o;
}

static Evas_Object *
_entrance_conf_main_build(Evas_Object *obj)
{
   Evas_Object *tb, *bx_over, *o, *bx;

   /*Main Frame*/
   o = bx_over = elm_box_add(obj);
   elm_box_horizontal_set(o, EINA_TRUE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(o);

   o = tb = elm_toolbar_add(obj);
   evas_object_size_hint_weight_set(o, 0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_toolbar_horizontal_set(o, EINA_FALSE);
   elm_toolbar_select_mode_set(o, ELM_OBJECT_SELECT_MODE_ALWAYS);
   elm_toolbar_shrink_mode_set(o, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_homogeneous_set(o, EINA_FALSE);
   elm_box_pack_end(bx_over, o);
   evas_object_show(o);

   o = bx = elm_box_add(obj);
   elm_box_horizontal_set(o, EINA_TRUE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx_over, o);
   evas_object_show(o);

   _entrance_int_conf_main->display_area = o;

   elm_toolbar_item_append(tb, NULL, "General",
       _entrance_conf_toolbar_click, _entrance_conf_main_general(obj));
   elm_toolbar_item_append(tb, NULL, "Background",
       _entrance_conf_toolbar_click, _entrance_conf_main_background(obj));
   elm_toolbar_item_append(tb, NULL, "Theme",
       _entrance_conf_toolbar_click, _entrance_conf_main_themesel(obj));
   elm_toolbar_item_append(tb, NULL, "Log",
       _entrance_conf_toolbar_click, _entrance_conf_main_graph_log(obj));

   return bx_over;
}

static Eina_Bool
_entrance_conf_main_check(void)
{
   const char *bg_path;
   const char *bg_group;

   entrance_gui_background_get(&bg_path, &bg_group);
   return !!((_entrance_int_conf_main->theme != entrance_gui_theme_name_get())
             || (_entrance_int_conf_main->bg.path != bg_path)
             || (_entrance_int_conf_main->bg.group != bg_group)
             || (_entrance_int_conf_main->scale != elm_config_scale_get())
             || (_entrance_int_conf_main->elm_profile != elm_config_profile_get())
             || (_entrance_int_conf_main->vkbd_enabled != entrance_gui_vkbd_enabled_get()));
}

static void
_entrance_conf_main_apply(void)
{
   Entrance_Conf_Gui_Event conf;

   conf.bg.path = _entrance_int_conf_main->bg.path;
   conf.bg.group = _entrance_int_conf_main->bg.group;
   conf.background_pool = NULL;
   conf.icon_pool = NULL;
   conf.vkbd_enabled = _entrance_int_conf_main->vkbd_enabled;

   if (_entrance_int_conf_main->scale != elm_config_scale_get())
     {
        elm_config_scale_set(_entrance_int_conf_main->scale);
        elm_config_all_flush();
        elm_config_save();
     }
   if (_entrance_int_conf_main->theme != entrance_gui_theme_name_get())
     {
        entrance_gui_theme_name_set(_entrance_int_conf_main->theme);
     }
   entrance_gui_conf_set(&conf);
   entrance_connect_conf_gui_send(&conf);
}


void
entrance_conf_main_init(void)
{
   PT("conf main init\n");
   entrance_conf_module_register("Main",
                                 _entrance_conf_main_begin,
                                 _entrance_conf_main_end,
                                 _entrance_conf_main_build,
                                 _entrance_conf_main_check,
                                 _entrance_conf_main_apply);
}

void
entrance_conf_main_shutdown(void)
{
   PT("conf main shutdown\n");
}

