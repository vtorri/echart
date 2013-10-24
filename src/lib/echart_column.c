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

/*
 * Some notes on the API:
 * 1. Every "drawer" (that is, an element that actually returns an enesim
 * renderer) on the chart library is not prefixed, so at the end it is
 * confusing to have echart_column, echart_line, echart_bar, echart_pie, etc
 * instead of having something like echart_drawer_column, echart_drawer_line,
 * or any other prefix that you prefer :)
 *
 * 2. Every "drawer" has some common functionality:
 *  - free(): Common function to free
 *  - chart_get(): Gets the drawer chart being used
 *  - chart_set(): A drawer needs a chart to know what to draw
 *  - renderer_get(): Gets the renderer based definition of the chart data
 * 3. It might be useful to define an interface for the drawing plus some common
 *
 * functions like: echart_drawer_free, echart_drawer_renderer_get, etc.
 * 4. Using the above we dont need to define new types for every chart type
 *
 * 5. We can define a common code to create the "background" of a chart. The
 * main compound renderer, the grids, the title, etc. In case the abstraction
 * of the "drawer" is done, then we can pass the compound renderer as first
 * parameter of the internal renderer_get() function.
 *
 * 6. Why the echart_line_chart_free()? it should free line, not the chart of the line
 *
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

/* TODO if this function is used too many times, we better add a shortcut
 * on enesim itself
 */
#define ECHART_RENDERER_LAYER_ADD(c,l,r,rop) \
do \
{ \
    l = enesim_renderer_compound_layer_new(); \
    enesim_renderer_compound_layer_renderer_set(l, r); \
    enesim_renderer_compound_layer_rop_set(l, rop); \
    enesim_renderer_compound_layer_add(c, l); \
} while (0)

struct _Echart_Column
{
    const Echart_Chart *chart;
};

static Enesim_Renderer *
_echart_text_renderer_from_double(Enesim_Text_Font *f, double d)
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

/* draw the main layout of a graph
 * x is the x coordinate labels
 * y is the y coordinate labels
 * inset if the chart area should have margins on left-right sides
 * outline if the chart area should be outlined
 */
static Enesim_Renderer *
_echart_grid_layout_renderer_get(const Echart_Chart *chart,
        const Echart_Data_Item *x_labels, const Echart_Data_Item *y_labels,
        Eina_Bool inset, Eina_Bool outline,
        Enesim_Rectangle *area)
{
    const Echart_Data *data;
    Enesim_Renderer *c, *r;
    Enesim_Renderer_Compound_Layer *l;
    Enesim_Text_Font *f;
    Enesim_Text_Engine *e;
    const char *label;
    int font_size = 16;
    int w, h;

    data = echart_chart_data_get(chart);
    /* initial chart area is everything */
    echart_chart_size_get(chart, &w, &h);
    enesim_rectangle_coords_from(area, 0, 0, w, h);
    
    /* main renderer */
    c = enesim_renderer_compound_new();

    /* background */
    r = enesim_renderer_rectangle_new();
    enesim_renderer_rectangle_position_set(r, 0, 0);
    enesim_renderer_rectangle_size_set(r, w, h);
    enesim_renderer_shape_fill_color_set(r, echart_chart_background_color_get(chart));
    enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);
    ECHART_RENDERER_LAYER_ADD(c, l, r, ENESIM_ROP_FILL);

    /* the common text properties */
    e = enesim_text_engine_default_get();
    f = enesim_text_font_new_description_from(e, "arial", font_size);
    enesim_text_engine_unref(e);

    /* title */
    label = echart_data_title_get(data);
    if (label)
    {
        Enesim_Rectangle geom;

        r = enesim_renderer_text_span_new();
        enesim_renderer_color_set(r, 0xff000000);
        enesim_renderer_text_span_text_set(r, echart_data_title_get(data));
        enesim_renderer_text_span_font_set(r, f);

        enesim_renderer_shape_destination_geometry_get(r, &geom);
        enesim_renderer_origin_set(r, (w - geom.w) / 2.0, 0);

        ECHART_RENDERER_LAYER_ADD(c, l, r, ENESIM_ROP_BLEND);
        area->y += font_size;
        area->h -= font_size;
    }

    if (x_labels)
    {
        area->h -= font_size;
    }

    if (y_labels)
    {
#if 0
        const Eina_List *labels = echart_data_item_values_get(y_labels);
        const Eina_List *ll;
        double *d;
        double y = area->y - font_size;
        double x = area->x;
        double label_area;
        int n_data;

        n_data = eina_list_count(echart_data_item_values_get(y_labels));
        if (inset)
        {
            label_area = area->w / (n_data + 1);
            x = area->x + label_area;
        }
        else
        {
            label_area = area->w / n_data;
            x = area->x;
        }

        EINA_LIST_FOREACH(labels, ll, d)
        {
            Enesim_Rectangle geom;

            r = _echart_text_renderer_from_double(f, *d);
            enesim_renderer_shape_destination_geometry_get(r, &geom);
            /* center the text */
            enesim_renderer_origin_set(r, x - (geom.w / 2), y);
            ECHART_RENDERER_LAYER_ADD(c, l, r, ENESIM_ROP_BLEND);

            x += label_area;
        }
        area->h -= font_size;
#endif
    }

    /* the x labels */
    if (x_labels)
    {
        const Eina_List *labels = echart_data_item_values_get(x_labels);
        const Eina_List *ll;
        double *d;
        double y = area->y + area->h;
        double x;
        double label_area;
        int n_data;

        n_data = eina_list_count(echart_data_item_values_get(x_labels));
        if (inset)
        {
            label_area = area->w / (n_data + 1);
            x = area->x + label_area;
        }
        else
        {
            label_area = area->w / n_data;
            x = area->x;
        }

        EINA_LIST_FOREACH(labels, ll, d)
        {
            Enesim_Rectangle geom;

            r = _echart_text_renderer_from_double(f, *d);
            enesim_renderer_shape_destination_geometry_get(r, &geom);
            /* center the text */
            enesim_renderer_origin_set(r, x - (geom.w / 2), y);
            ECHART_RENDERER_LAYER_ADD(c, l, r, ENESIM_ROP_BLEND);

            x += label_area;
        }
    }

    /* draw the border of the chart */
    if (outline)
    {
        r = enesim_renderer_rectangle_new();
        enesim_renderer_rectangle_position_set(r, area->x, area->y);
        enesim_renderer_rectangle_size_set(r, area->w, area->h);
        enesim_renderer_shape_stroke_color_set(r, 0xff000000);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);
        ECHART_RENDERER_LAYER_ADD(c, l, r, ENESIM_ROP_BLEND);
    }

    return c;
}

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

EAPI Echart_Column *
echart_column_new(void)
{
    Echart_Column *thiz;

    thiz = (Echart_Column *)calloc(1, sizeof(Echart_Column));
    return thiz;
}

EAPI void
echart_column_chart_free(Echart_Column *thiz)
{
    if (!thiz)
        return;

    free(thiz);
}

EAPI void
echart_column_chart_set(Echart_Column *thiz, const Echart_Chart *chart)
{
    if (!thiz || !chart)
        return;
    thiz->chart = chart;
}

EAPI Enesim_Renderer *
echart_column_renderer_get(Echart_Column *thiz)
{
    const Echart_Chart *chart;
    const Echart_Data *data;
    const Echart_Data_Item *absciss;
    Enesim_Rectangle geom;

    chart = thiz->chart;
    data = echart_chart_data_get(chart);

    absciss = echart_data_items_get(data, 0);
 
    return _echart_grid_layout_renderer_get(thiz->chart, absciss, NULL, EINA_TRUE, EINA_TRUE, &geom);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/


