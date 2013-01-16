#ifndef RELAXPLUGIN_H
#define RELAXPLUGIN_H

#include <glib-object.h>

/**
  * standard glib object declaration
  */
G_BEGIN_DECLS

#define RELAX_PLUGIN_TYPE (relax_plugin_get_type())
#define RELAX_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_CAST((o), RELAX_PLUGIN_TYPE, RelaxPlugin))
#define IS_RELAX_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), RELAX_PLUGIN_TYPE))

typedef struct _RelaxPlugin     RelaxPlugin;
typedef struct _RelaxPluginClass     RelaxPluginClass;

struct _RelaxPlugin {
    GObject parent_slot;
};

struct _RelaxPluginClass {
    GObjectClass parent_slot;
};

/**
  * @abstract relax_plugin_get_type() returns the gobject type
  *
  * @returns GType of the GObject. used by nautilus
  */
GType relax_plugin_get_type(void);

/**
  * @abstract relax_plugin_register_type() registers the module
  * called by Nautilus
  *
  * @param module: GTypeModule object to be registered
  */
void relax_plugin_register_type(GTypeModule *module);

G_END_DECLS

#endif // RELAXPLUGIN_H
