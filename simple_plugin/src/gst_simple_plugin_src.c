#include "gst_simple_plugin_src.h"

G_DEFINE_TYPE(GstSimplePluginSrc, gst_simple_plugin_src, GST_TYPE_PUSH_SRC);


static void gst_simple_plugin_src_init(GstSimplePluginSrc* src)
{
  (void)src;
}

static void gst_simple_plugin_src_class_init(GstSimplePluginSrcClass* klass)
{
  (void)klass;
}

static gboolean plugin_init(GstPlugin* plugin)
{
  gboolean res = gst_element_register(plugin, "simplepluginsrc", GST_RANK_NONE, GST_TYPE_SIMPLE_PLUGIN_SRC);
  return  res;
}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    simple_plugin_src,
    "Simple plugin",
    plugin_init,
    GST_PACKAGE_VERSION,
    GST_PACKAGE_LICENSE,
    GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)
