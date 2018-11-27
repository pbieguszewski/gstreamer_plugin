#include <gst/app/gstappsink.h>

typedef enum
{
  APP_OK = 0,
  APP_CANNOT_CREATE_PIPELINE = -1,
  APP_CANNOT_CREATE_SOURCE = -2,
  APP_CANNOT_ADD_SOURCE_TO_PIPELINE = -3,
  APP_CANNOT_CREATE_SINK = -4,
  APP_CANNOT_ADD_SINK_TO_PIPELINE = -5,
  APP_CANNOT_LINK_SOURCE_AND_SINK = -6,
  APP_CANNOT_START_PIPELINE = -7,
} APP_ERROR;

typedef struct _Frame
{
  gint w;
  gint h;
  void* data;
} Frame;

GstFlowReturn newSampleOccur(GstAppSink *appsink, gpointer user_data);

int main(int argc, char** argv)
{
  GstElement* pipeline = NULL;
  GstElement* source = NULL;
  GstElement* sink = NULL;

  Frame currentFrame = { 0, 0, NULL };

  int ret = APP_OK;

  gst_init(&argc, &argv);

  do
  {
    pipeline = gst_pipeline_new("pipeline");
    if (!pipeline)
    {
      ret = APP_CANNOT_CREATE_PIPELINE;
      break;
    }

    source = gst_element_factory_make("simplepluginsrc", "source");
    if (!source)
    {
      ret = APP_CANNOT_CREATE_SOURCE;
      break;
    }
    if (!gst_bin_add(GST_BIN(pipeline), source))
    {
      gst_object_unref(source);
      source = NULL;
      ret = APP_CANNOT_ADD_SOURCE_TO_PIPELINE;
      break;
    }

    sink = gst_element_factory_make("appsink", "sink");
    if (!sink)
    {
      ret = APP_CANNOT_CREATE_SINK;
      break;
    }
    if (!gst_bin_add(GST_BIN(pipeline), sink))
    {
      gst_object_unref(sink);
      sink = NULL;
      ret = APP_CANNOT_ADD_SOURCE_TO_PIPELINE;
      break;
    }

    g_object_set(G_OBJECT(sink), "emit-signals", TRUE, NULL);
    g_object_set(G_OBJECT(sink), "max-buffers", 30, NULL);

    g_signal_connect(G_OBJECT(sink), "new_sample", (GCallback)newSampleOccur, &currentFrame);

    if (!gst_element_link(source, sink))
    {
      ret = APP_CANNOT_LINK_SOURCE_AND_SINK;
      break;
    }

    if (GST_STATE_CHANGE_FAILURE == gst_element_set_state(pipeline, GST_STATE_PLAYING))
    {
      ret = APP_CANNOT_START_PIPELINE;
      break;
    }

    GMainLoop* main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
  } while (FALSE);

  if (pipeline)
  {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    pipeline = NULL;
  }

  gst_deinit();

  return ret;
}

GstFlowReturn newSampleOccur(GstAppSink *appsink, gpointer user_data)
{
  Frame* frame = (Frame*)user_data;

  GstSample* sample = gst_base_sink_get_last_sample(GST_BASE_SINK(appsink));
  GstBuffer* buffer = gst_sample_get_buffer(sample);
  GstMapInfo map;
  gst_buffer_map(buffer, &map, GST_MAP_READ);
  frame->data = malloc(map.size);
  memcpy(frame->data, map.data, map.size);
  gst_buffer_unmap(buffer, &map);

  gint width = 0;
  gint height = 0;

  GstCaps* caps = gst_sample_get_caps(sample);
  if(caps)
  {
    GstStructure* gstr = gst_caps_get_structure(caps, 0);

    gst_structure_get_int(gstr, "width", &width);
    gst_structure_get_int(gstr, "height", &height);

    frame->w = width;
    frame->h = height;
  }

  return GST_FLOW_OK;
}
