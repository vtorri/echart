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

#include <Enesim.h>

#include "Echart.h"
#include "echart_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

struct _Echart_Line
{
    Echart_Chart *chart;
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Enesim_Renderer *
echart_line_new(Echart_Chart *chart)
{
    const Echart_Data *data;
    const Echart_Data_Item *absciss;
    const Echart_Data_Item *item;
    Enesim_Renderer *c;
    Enesim_Renderer *r;
    Enesim_Renderer_Compound_Layer *l;
    Enesim_Path *p;
    Enesim_Text_Font *f;
    Enesim_Text_Engine *e;
    Enesim_Rectangle geom;
    double avmin;
    double avmax;
    int w;
    int h;
    unsigned int i;
    unsigned int j;

    if (!chart)
        return NULL;

    data = echart_chart_data_get(chart);
    if (!chart)
    {
        ERR("A chart must have at least a data");
        return NULL;
    }

    if (echart_data_items_count(data) < 2)
    {
        ERR("Data must have at least 2 items");
        return NULL;
    }

    echart_chart_size_get(chart, &w, &h);

    c = echart_chart_compound_get(chart);

    absciss = echart_data_items_get(data, 0);
    echart_data_item_interval_get(absciss, &avmin, &avmax);

    for (j = 1; j < echart_data_items_count(data); j++)
    {
        double vmin;
        double vmax;

        item = echart_data_items_get(data, j);
        echart_data_item_interval_get(item, &vmin, &vmax);

        p = enesim_path_new();
        for (i = 0; i < eina_list_count(echart_data_item_values_get(item)); i++)
        {
            double d1;
            double d2;

            d1 = *(double *)eina_list_nth(echart_data_item_values_get(absciss), i);
            d1 = w * (d1 - avmin) / (avmax - avmin);
            d2 = *(double *)eina_list_nth(echart_data_item_values_get(item), i);
            d2 = h * (d2 - vmin) / (vmax - vmin);
            if (i == 0)
                enesim_path_move_to(p, d1, h - d2);
            else
                enesim_path_line_to(p, d1, h - d2);
        }

        r = enesim_renderer_path_new();
        enesim_renderer_path_path_set(r, p);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        enesim_renderer_shape_stroke_color_set(r, echart_data_item_color_get(item));
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);

        e = enesim_text_engine_default_get();
        f = enesim_text_font_new_description_from(e, "arial", 16);
        enesim_text_engine_unref(e);

        r = enesim_renderer_text_span_new();
        enesim_renderer_color_set(r, 0xff000000);
        enesim_renderer_text_span_text_set(r, "Hello World!");
        enesim_renderer_text_span_font_set(r, f);

        enesim_renderer_shape_destination_geometry_get(r, &geom);
        enesim_renderer_origin_set(r, (w - geom.w) / 2, h - geom.h);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    return c;
}
