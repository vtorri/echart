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
    const Echart_Chart *chart;
    unsigned int area : 1;
    unsigned int additive: 1;
};

static Enesim_Renderer *
_echart_line_text_renderer_from_double(Enesim_Text_Font *f, double d)
{
    char buf[256];
    Enesim_Renderer *r;

    snprintf(buf, sizeof(buf), "%d", (int)d);
    buf[sizeof(buf) - 1] = '\0';

    r = enesim_renderer_text_span_new();
    enesim_renderer_color_set(r, 0xff000000);
    enesim_renderer_text_span_text_set(r, buf);
    enesim_renderer_text_span_font_set(r, f);

    return r;
}

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Echart_Line *
echart_line_new(void)
{
    Echart_Line *line;

    line = (Echart_Line *)calloc(1, sizeof(Echart_Line));
    if (!line)
        return NULL;

    return line;
}

EAPI void
echart_line_chart_free(Echart_Line *line)
{
    if (!line)
        return;

    free(line);
}

EAPI void
echart_line_chart_set(Echart_Line *line, const Echart_Chart *chart)
{
    if (!line || !chart)
        return;

    line->chart = chart;
}

EAPI const Echart_Chart *
echart_line_chart_get(const Echart_Line *line)
{
    if (!line)
        return NULL;

    return line->chart;
}

EAPI void
echart_line_area_set(Echart_Line *line, Eina_Bool area)
{
    if (!line || (line->area == !! area))
        return;

    line->area = !!area;
}

EAPI Eina_Bool
echart_line_area_get(const Echart_Line *line)
{
    if (!line)
        return EINA_FALSE;

    return line->area;
}

EAPI Enesim_Renderer *
echart_line_renderer_get(const Echart_Line *line)
{
    const Echart_Chart *chart;
    const Echart_Data *data;
    const Echart_Data_Item *absciss;
    const Echart_Data_Item *item;
    Enesim_Renderer *c;
    Enesim_Renderer *r;
    Enesim_Renderer *r_first;
    Enesim_Renderer_Compound_Layer *l;
    Enesim_Text_Font *f;
    Enesim_Text_Engine *e;
    Enesim_Rectangle geom;
    Enesim_Path *p;
    Enesim_Color color;
    Eina_Rectangle rect_first;
    Eina_Rectangle rect;
    double avmin;
    double avmax;
    int x_area;
    int y_area;
    int w_area;
    int h_area;
    int w;
    int h;
    unsigned int i;
    unsigned int j;

    if (!line)
        return NULL;

    chart = line->chart;

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

    e = enesim_text_engine_default_get();
    f = enesim_text_font_new_description_from(e, "arial", 16);
    enesim_text_engine_unref(e);

    /* title */

    h_area = 0;
    if (echart_data_title_get(data))
    {
        r = enesim_renderer_text_span_new();
        enesim_renderer_color_set(r, 0xff000000);
        enesim_renderer_text_span_text_set(r, echart_data_title_get(data));
        enesim_renderer_text_span_font_set(r, f);

        enesim_renderer_shape_destination_geometry_get(r, &geom);
        enesim_renderer_origin_set(r, (w - geom.w) / 2, 0);
        enesim_rectangle_normalize(&geom, &rect);
        h_area = rect.h;

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    /* abscisses */

    absciss = echart_data_items_get(data, 0);
    echart_data_item_interval_get(absciss, &avmin, &avmax);

    r_first = _echart_line_text_renderer_from_double(f, *(double *)eina_list_nth(echart_data_item_values_get(absciss), 0));

    enesim_renderer_shape_destination_geometry_get(r_first, &geom);
    enesim_renderer_origin_set(r_first, 0, h - geom.h);
    enesim_rectangle_normalize(&geom, &rect_first);
    x_area = rect_first.w / 2 + 1;
    y_area = rect_first.h;

    l = enesim_renderer_compound_layer_new();
    enesim_renderer_compound_layer_renderer_set(l, r_first);
    enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
    enesim_renderer_compound_layer_add(c, l);

    r = _echart_line_text_renderer_from_double(f, *(double *)eina_list_nth(echart_data_item_values_get(absciss), eina_list_count(echart_data_item_values_get(absciss)) - 1));

    enesim_renderer_shape_destination_geometry_get(r, &geom);
    enesim_renderer_origin_set(r, w - geom.w, h - geom.h);
    enesim_rectangle_normalize(&geom, &rect);
    w_area = w - (rect_first.w + rect.w) / 2;
    if (y_area < rect.h)
        y_area = rect.h;

    l = enesim_renderer_compound_layer_new();
    enesim_renderer_compound_layer_renderer_set(l, r);
    enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
    enesim_renderer_compound_layer_add(c, l);

    for (i = 1; i < eina_list_count(echart_data_item_values_get(absciss)) - 1; i++)
    {
        double d1;

        r = _echart_line_text_renderer_from_double(f, *(double *)eina_list_nth(echart_data_item_values_get(absciss), i));

        d1 = *(double *)eina_list_nth(echart_data_item_values_get(absciss), i);
        d1 = x_area + w_area * (d1 - avmin) / (avmax - avmin);

        enesim_renderer_shape_destination_geometry_get(r, &geom);
        enesim_renderer_origin_set(r, d1 - geom.w / 2, h - geom.h);
        enesim_rectangle_normalize(&geom, &rect);
        if (y_area < rect.h)
            y_area = rect.h;

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }
    h_area = h - y_area - h_area;

    if (line->area)
    {
        for (j = 1; j < echart_data_items_count(data); j++)
        {
            double vmin;
            double vmax;
            double d1;
            double d2;
            uint8_t ca, cr, cg, cb;

            item = echart_data_items_get(data, j);
            echart_data_item_interval_get(item, &vmin, &vmax);

            p = enesim_path_new();
            enesim_path_move_to(p, 1, h - 1);
            for (i = 0; i < eina_list_count(echart_data_item_values_get(item)); i++)
            {
                d1 = *(double *)eina_list_nth(echart_data_item_values_get(absciss), i);
                d1 = x_area + w_area * (d1 - avmin) / (avmax - avmin);
                d2 = *(double *)eina_list_nth(echart_data_item_values_get(item), i);
                d2 = h_area * (d2 - vmin) / (vmax - vmin);
                enesim_path_line_to(p, d1, h - y_area - d2);
            }
            enesim_path_line_to(p, x_area + w_area - 1, h - y_area - 1);
            enesim_path_close(p);

            r = enesim_renderer_path_new();
            enesim_renderer_path_path_set(r, p);
            enesim_argb_components_to(echart_data_item_color_get(item).area, &ca, &cr, &cg, &cb);
            ca = 220;
            enesim_color_components_from(&color, ca, cr, cg, cb);
            enesim_renderer_shape_fill_color_set(r, color);
            enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);

            l = enesim_renderer_compound_layer_new();
            enesim_renderer_compound_layer_renderer_set(l, r);
            enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
            enesim_renderer_compound_layer_add(c, l);
        }
    }

    for (j = 1; j < echart_data_items_count(data); j++)
    {
        double vmin;
        double vmax;
        double d1;
        double d2;

        item = echart_data_items_get(data, j);
        echart_data_item_interval_get(item, &vmin, &vmax);

        p = enesim_path_new();
        for (i = 0; i < eina_list_count(echart_data_item_values_get(item)); i++)
        {
            d1 = *(double *)eina_list_nth(echart_data_item_values_get(absciss), i);
            d1 = x_area + w_area * (d1 - avmin) / (avmax - avmin);
            d2 = *(double *)eina_list_nth(echart_data_item_values_get(item), i);
            d2 = h_area * (d2 - vmin) / (vmax - vmin);
            if (i == 0)
                enesim_path_move_to(p, d1, h - y_area - d2);
            else
                enesim_path_line_to(p, d1, h - y_area - d2);
        }

        r = enesim_renderer_path_new();
        enesim_renderer_path_path_set(r, p);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        enesim_renderer_shape_stroke_color_set(r, echart_data_item_color_get(item).line);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    return c;
}
