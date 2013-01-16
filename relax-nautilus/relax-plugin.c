#include "relax-plugin.h"

#include <glib.h>
/* Nautilus extension headers */
#include <libnautilus-extension/nautilus-extension-types.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-info-provider.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-property-page-provider.h>

#include <gtk/gtk.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static GObjectClass *parent_class;

static void add_to_relax_callback(NautilusMenuItem* item,
                                  gpointer user_data)
{
    gchar* folders;
    folders = (gchar*)g_object_get_data(G_OBJECT(item), "folder");

    g_print("found this folder %s\n", folders);
    char *home = "/home/";
    char* username = getenv("USER");
    char* filename = "/.relax-comm";

    char path[strlen(home)+strlen(username)+strlen(filename)+1];

    strcpy(path, home);
    strcat(path, username);
    strcat(path, filename);

    int fd = open(path, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                  S_IROTH | S_IWOTH);


    if(fd != -1){
        write(fd, (char*)folders, strlen(folders));
    }

    close(fd);
}

static void refresh_relax_callback(NautilusMenuItem* item,
                                 gpointer user_data)
{
    g_print("now refreshing...");
}

static GList* relax_plugin_get_background_items(NautilusMenuProvider* provider,
                                              GtkWidget *window,
                                              NautilusFileInfo* current_folder)
{
    NautilusMenu *sub_menu;

    NautilusMenuItem *item;
    NautilusMenuItem *add_item;
    NautilusMenuItem *refresh_item;

    GList* ret_val = NULL;
    GFile* a_file;

    add_item = nautilus_menu_item_new("RelaxPlugin::add",
                           "add folder to relax",
                           "adds the current folder as a folder to be monitored on relax",
                           NULL);

    g_signal_connect (add_item, "activate", add_to_relax_callback, provider);

    a_file = nautilus_file_info_get_location(current_folder);
    g_object_set_data(G_OBJECT(add_item), "folder", (gpointer)g_file_get_path(a_file));

    refresh_item = nautilus_menu_item_new("RelaxPlugin::refresh",
                                          "refresh relax",
                                          "refreshes Relax! app",
                                          NULL);
    g_signal_connect (refresh_item, "activate", refresh_relax_callback, provider);

    sub_menu = nautilus_menu_new();
    nautilus_menu_append_item(sub_menu, add_item);
    nautilus_menu_append_item(sub_menu, refresh_item);

    item = nautilus_menu_item_new("Relax::Plugin",
                                  "relax-file-manager",
                                  "show operations to be done on relax",
                                  NULL);

    nautilus_menu_item_set_submenu(item, sub_menu);

    ret_val = g_list_append(ret_val, item);

    return ret_val;
}

static void relax_plugin_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
    iface->get_background_items = relax_plugin_get_background_items;
}

static void relax_plugin_instance_init (RelaxPlugin* obj)
{
    /*?*/
}

static void relax_plugin_class_init (RelaxPluginClass* class_obj)
{
    parent_class = g_type_class_peek_parent(class_obj);
}

static GType plugin_type = 0;

GType relax_plugin_get_type()
{
    return plugin_type;
}

void relax_plugin_register_type(GTypeModule *module){
    static const GTypeInfo info = {
        sizeof(RelaxPluginClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) relax_plugin_class_init,
        NULL,
        NULL,
        sizeof(RelaxPlugin),
        0,
        (GInstanceInitFunc) relax_plugin_class_init,
    };

    static const GInterfaceInfo menu_provider_iface_info = {
        (GInterfaceInitFunc) relax_plugin_menu_provider_iface_init,
        NULL,
        NULL
    };

    plugin_type = g_type_module_register_type(module, G_TYPE_OBJECT,
                                              "RelaxPlugin", &info, 0);

    g_type_module_add_interface(module,
                                plugin_type,
                                NAUTILUS_TYPE_MENU_PROVIDER,
                                &menu_provider_iface_info);
}


