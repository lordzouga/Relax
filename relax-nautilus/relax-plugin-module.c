#include "relax-plugin.h"

#include <glib.h>
/* Nautilus extension headers */
#include <libnautilus-extension/nautilus-extension-types.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-info-provider.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-property-page-provider.h>

#include <gtk/gtk.h>

#include <string.h>
#include <time.h>
static GType provider_types[1];

void nautilus_module_initialize(GTypeModule *module){
    relax_plugin_register_type(module);
    provider_types[0] = relax_plugin_get_type();
}

void nautilus_module_shutdown(){
    /*?*/
}

void nautilus_module_list_types(const GType **types, int *num_types)
{
    *types = provider_types;
    *num_types = G_N_ELEMENTS (provider_types);
}


