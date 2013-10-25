/* Echart - Chart rendering library
 * Copyright (C) 2013 Vincent Torri
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include <Enesim.h>

#include <Echart.h>

#define LINE 0

static void
_echart_delete_cb(Ecore_Evas *ee EINA_UNUSED)
{
    ecore_main_loop_quit();
}

int main()
{
    Ecore_Evas *ee;
    Evas *evas;
    Evas_Object *o;
    void *m;
    size_t stride;
    Enesim_Surface *s;
    Enesim_Renderer *rline;
    Enesim_Log *error = NULL;
    Echart_Line *line;
    Echart_Column *column;
    Echart_Chart *chart;
    Echart_Data *data;
    Echart_Data_Item *item;
    int w;
    int h;

    if (!ecore_evas_init())
        return -1;

    if (!echart_init())
        goto shutdown_ecore_evas;

    /* we create the data */
    data = echart_data_new();
    echart_data_title_set(data, "Test");

    /* we first create the absciss and add it to the data */
    item = echart_data_item_new();
    echart_data_item_title_set(item, "Year");
    echart_data_item_value_add(item, 2004);
    echart_data_item_value_add(item, 2005);
    echart_data_item_value_add(item, 2006);
    echart_data_item_value_add(item, 2007);
    echart_data_absciss_set(data, item);

    /* then we create the items for the data and add it to the data */
    item = echart_data_item_new();
    echart_data_item_title_set(item, "Sales");
    echart_data_item_value_add(item, 1000);
    echart_data_item_value_add(item, 1170);
    echart_data_item_value_add(item, 660);
    echart_data_item_value_add(item, 1030);
    echart_data_items_set(data, item);

    item = echart_data_item_new();
    echart_data_item_title_set(item, "Expenses");
    echart_data_item_value_add(item, 400);
    echart_data_item_value_add(item, 460);
    echart_data_item_value_add(item, 1120);
    echart_data_item_value_add(item, 540);
    echart_data_items_set(data, item);

    /* we create the chart and set the data to it*/
    chart = echart_chart_new();
    echart_chart_data_set(chart, data);
    echart_chart_size_get(chart, &w, &h);
    echart_chart_background_color_set(chart, 255, 255, 255, 255);

#if LINE
    line = echart_line_new();
    echart_line_chart_set(line, chart);
    /* echart_line_area_set(line, EINA_TRUE); */
    /* echart_line_stacked_set(line, EINA_TRUE); */
    rline = echart_line_renderer_get(line);
#else
    column = echart_column_new();
    echart_column_chart_set(column, chart);
    rline = echart_column_renderer_get(column);
#endif

    ee = ecore_evas_new(NULL, 0, 0, 1, 1, NULL);
    if (!ee)
        goto shutdown_echart;

    ecore_evas_callback_delete_request_set(ee, _echart_delete_cb);
    evas = ecore_evas_get(ee);

    o = evas_object_image_add(evas);
    evas_object_image_size_set(o, w, h);
    evas_object_image_fill_set(o, 0, 0, w, h);
    m = evas_object_image_data_get(o, EINA_TRUE);
    stride = evas_object_image_stride_get(o);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, w, h);
    evas_object_show(o);

    s = enesim_surface_new_data_from(ENESIM_FORMAT_ARGB8888, w, h, EINA_FALSE, m, stride, NULL, NULL);
    if (!enesim_renderer_draw(rline, s, ENESIM_ROP_FILL, NULL, 0, 0,
                              &error))
    {
        printf("merde\n");
        enesim_log_dump(error);
    }

    ecore_evas_resize(ee, w, h);
    ecore_evas_show(ee);

    ecore_main_loop_begin();

    ecore_evas_shutdown();

    return 0;

  shutdown_echart:
    echart_shutdown();
  shutdown_ecore_evas:
    ecore_evas_shutdown();

    return -1;
}
