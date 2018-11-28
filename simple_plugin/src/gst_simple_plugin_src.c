#include "gst_simple_plugin_src.h"

#include <gst/video/video.h>

#define DEFAULT_R_CHANNEL (guint)53
#define DEFAULT_G_CHANNEL (guint)50
#define DEFAULT_B_CHANNEL (guint)95
#define MIN_CHANNEL (guint)0
#define MAX_CHANNEL (guint)255

enum
{
  PROP_0 = 0,
  PROP_R_CHANNEL,
  PROP_G_CHANNEL,
  PROP_B_CHANNEL,
  PROP_SIZE
};

static void gst_simple_plugin_src_set_property(
  GObject* object, guint property_id, const GValue* value, GParamSpec* pspec);
static void gst_simple_plugin_src_get_property(
  GObject* object, guint property_id, GValue* value, GParamSpec* pspec);
static void gst_imple_plugin_src_dispose (GObject* object);
static gboolean gst_simple_plugin_src_start(GstBaseSrc* bsrc);
static gboolean gst_simple_plugin_src_stop (GstBaseSrc* src);
static GstFlowReturn gst_simple_plugin_src_fill(GstPushSrc* src, GstBuffer* buf);
static GstCaps* gst_simple_plugin_src_get_caps (GstBaseSrc* src, GstCaps* filter);
static gboolean gst_simple_plugin_src_set_caps (GstBaseSrc* src, GstCaps* caps);

static GstStaticPadTemplate gst_simple_plugin_src_template =
  GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE("{ RGB }"))
);

G_DEFINE_TYPE(GstSimplePluginSrc, gst_simple_plugin_src, GST_TYPE_PUSH_SRC);

static void* rawFrame = NULL;
static guint32 frameFill = 0;

static void gst_simple_plugin_src_init(GstSimplePluginSrc* src)
{
  gst_base_src_set_live (GST_BASE_SRC(src), TRUE);

  src->r = DEFAULT_R_CHANNEL;
  src->g = DEFAULT_G_CHANNEL;
  src->b = DEFAULT_B_CHANNEL;
}

static void gst_simple_plugin_src_class_init(GstSimplePluginSrcClass* klass)
{
  GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
  GstElementClass* gstelement_class = GST_ELEMENT_CLASS(klass);
  GstBaseSrcClass* gstbasesrc_class = GST_BASE_SRC_CLASS(klass);
  GstPushSrcClass* gstpushsrc_class = GST_PUSH_SRC_CLASS(klass);

  gobject_class->set_property = gst_simple_plugin_src_set_property;
  gobject_class->get_property = gst_simple_plugin_src_get_property;
  gobject_class->dispose = gst_imple_plugin_src_dispose;

  g_object_class_install_property(
    gobject_class,
    PROP_R_CHANNEL,
    g_param_spec_uint(
      "r_channel",
      "R channel",
      "R channel",
      MIN_CHANNEL,
      MAX_CHANNEL,
      DEFAULT_R_CHANNEL,
      G_PARAM_READWRITE
  ));

  g_object_class_install_property(
    gobject_class,
    PROP_G_CHANNEL,
    g_param_spec_uint(
      "g_channel",
      "G channel",
      "G channel",
      MIN_CHANNEL,
      MAX_CHANNEL,
      DEFAULT_G_CHANNEL,
      G_PARAM_READWRITE
  ));

  g_object_class_install_property(
    gobject_class,
    PROP_B_CHANNEL,
    g_param_spec_uint(
      "b_channel",
      "B channel",
      "B channel",
      MIN_CHANNEL,
      MAX_CHANNEL,
      DEFAULT_B_CHANNEL,
      G_PARAM_READWRITE
  ));

  gst_element_class_add_pad_template(
    gstelement_class,
    gst_static_pad_template_get(&gst_simple_plugin_src_template));

  gst_element_class_set_static_metadata(
    gstelement_class,
    "Simple Plugin Source",
    "Source/Video",
    "Simple plugin video source",
    "pbieguszewski"
  );

  gstbasesrc_class->start = gst_simple_plugin_src_start;
  gstbasesrc_class->stop = gst_simple_plugin_src_stop;
  gstbasesrc_class->get_caps = gst_simple_plugin_src_get_caps;
  gstbasesrc_class->set_caps = gst_simple_plugin_src_set_caps;

  gstpushsrc_class->fill = gst_simple_plugin_src_fill;
}

static void gst_simple_plugin_src_set_property(
  GObject* object, guint property_id, const GValue* value, GParamSpec* pspec)
{
  GstSimplePluginSrc* simplePluginSrc = GST_SIMPLE_PLUGIN_SRC (object);

  switch (property_id)
  {
  case PROP_R_CHANNEL:
    simplePluginSrc->r = g_value_get_uint(value);
    break;
  case PROP_G_CHANNEL:
    simplePluginSrc->g = g_value_get_uint(value);
    break;
  case PROP_B_CHANNEL:
    simplePluginSrc->b = g_value_get_uint(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void gst_simple_plugin_src_get_property(
  GObject* object, guint property_id, GValue* value, GParamSpec* pspec)
{
  GstSimplePluginSrc* simplePluginSrc = GST_SIMPLE_PLUGIN_SRC (object);

  switch (property_id)
  {
  case PROP_R_CHANNEL:
    g_value_set_uint (value, simplePluginSrc->r);
    break;
  case PROP_G_CHANNEL:
    g_value_set_uint (value, simplePluginSrc->g);
    break;
  case PROP_B_CHANNEL:
    g_value_set_uint (value, simplePluginSrc->b);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void gst_imple_plugin_src_dispose (GObject* object)
{
  (void)object;
  free(rawFrame);
  frameFill = 0;
}

static gboolean gst_simple_plugin_src_start(GstBaseSrc* src)
{
  (void)src;
  return  TRUE;
}

static gboolean gst_simple_plugin_src_stop (GstBaseSrc* src)
{
  (void)src;
  return  TRUE;
}

static GstFlowReturn gst_simple_plugin_src_fill(GstPushSrc* src, GstBuffer* buf)
{
  GstSimplePluginSrc* simplePluginSrc = GST_SIMPLE_PLUGIN_SRC(src);
  (void)simplePluginSrc;
  GstMapInfo mapInfo;
  if (!gst_buffer_map(buf, &mapInfo, GST_MAP_WRITE))
    return GST_FLOW_ERROR;

  memcpy (mapInfo.data, rawFrame, frameFill);

  gst_buffer_unmap (buf, &mapInfo);

  return GST_FLOW_OK;
}

static GstCaps* gst_simple_plugin_src_get_caps (GstBaseSrc* src, GstCaps* filter)
{
  GstVideoInfo vinfo;
  GstVideoFormat videoForamt = GST_VIDEO_FORMAT_RGB;
  guint32 width = 800;
  guint32 height = 600;

  gst_video_info_init (&vinfo);
  gst_video_info_set_format (&vinfo, videoForamt, width, height);

  vinfo.fps_n = 30;
  vinfo.fps_d = 1;
  GstCaps* caps = gst_video_info_to_caps (&vinfo);

  if (filter && caps)
  {
    GstCaps *tmp = gst_caps_intersect(caps, filter);
    gst_caps_unref (caps);
    caps = tmp;
  }

  GstSimplePluginSrc* simplePluginSrc = GST_SIMPLE_PLUGIN_SRC(src);

  if (!rawFrame)
  {
    frameFill = width * height * 3;
    rawFrame = malloc(frameFill * sizeof(guint8));
  }

  guint8* frame = (guint8*)rawFrame;
  for (guint i = 0; i < frameFill; i += 3)
  {
    frame[i + 0] = (guint8)simplePluginSrc->r;
    frame[i + 1] = (guint8)simplePluginSrc->g;
    frame[i + 2] = (guint8)simplePluginSrc->b;
  }

  return caps;
}

static gboolean gst_simple_plugin_src_set_caps (GstBaseSrc* src, GstCaps* caps)
{
  GstVideoInfo vinfo;

  gst_video_info_from_caps (&vinfo, caps);

  gst_base_src_set_blocksize(src, (guint)GST_VIDEO_INFO_SIZE(&vinfo));

  return TRUE;
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
