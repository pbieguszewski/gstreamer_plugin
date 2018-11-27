#ifndef GST_SIMPLE_PLUGIN_SRC_H
#define GST_SIMPLE_PLUGIN_SRC_H

#include <gst/base/gstpushsrc.h>

G_BEGIN_DECLS

typedef struct _GstSimplePluginSrc GstSimplePluginSrc;
typedef struct  _GstSimplePluginSrcClass GstSimplePluginSrcClass;

#define GST_TYPE_SIMPLE_PLUGIN_SRC (gst_simple_plugin_src_get_type())
#define GST_SIMPLE_PLUGIN_SRC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_SIMPLE_PLUGIN_SRC, GstSimplePluginSrc))
#define GST_SIMPLE_PLUGIN_SRC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((obj), GST_TYPE_SIMPLE_PLUGIN_SRC, GstSimplePluginClass))
#define GST_IS_SIMPLE_PLUGIN_SRC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_SIMPLE_PLUGIN_SRC))
#define GST_IS_SIMPLE_PLUGIN_SRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_SIMPLE_PLUGIN_SRC))

typedef struct  _GstSimplePluginSrc {
  GstPushSrc element;
  GstPad* srcPad;
} GstSimplePluginSrc;

typedef struct _GstSimplePluginSrcClass
{
  GstPushSrcClass parent_class;
} GstSimplePluginSrcClass;

GType gst_simple_plugin_src_get_type(void);

G_END_DECLS

#endif // GST_SIMPLE_PLUGIN_SRC_H
