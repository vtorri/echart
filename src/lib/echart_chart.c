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

struct _Echart_Chart
{
    Echart_Data *data;
    Enesim_Argb background_color;
    int width;
    int height;
    struct
    {
        int x_nbr;
        int y_nbr;
        Enesim_Argb color;
    } grid;
    struct
    {
        int x_nbr;
        int y_nbr;
        Enesim_Argb color;
    } sub_grid;
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Echart_Colors echart_chart_default_colors[20] =
{
    { 0xff3366CC, 0xffc2d1f0 },
    { 0xffDC3912, 0xfff5c4b8 },
    { 0xffFF9900, 0xffffe0b3 },
    { 0xff109618, 0xffb7dfba },
    { 0xff990099, 0xffe0b3e0 },
    { 0xff3B3EAC, 0xffb3e0ee },
    { 0xff0099C6, 0xfff5c7d6 },
    { 0xffDD4477, 0xffd1e6b3 },
    { 0xff66AA00, 0xffeac0c0 },
    { 0xffB82E2E, 0xffc1d0df },
    { 0xff316395, 0xffe0c7e0 },
    { 0xff994499, 0xffbde6e0 },
    { 0xff22AA99, 0xffe6e6b8 },
    { 0xffAAAA11, 0xffd1c2f0 },
    { 0xff6633CC, 0xfff7d5b3 },
    { 0xffE67300, 0xffdcb5b5 },
    { 0xff8B0707, 0xffd1b7d1 },
    { 0xff329262, 0xffc2ded0 },
    { 0xff5574A6, 0xffccd5e4 },
    { 0xff3B3EAC, 0xffc4c5e6 }
};

Enesim_Renderer *
echart_chart_compound_get(const Echart_Chart *chart)
{
    Enesim_Renderer *c;
    Enesim_Renderer *r;
    Enesim_Renderer_Compound_Layer *l;
    Enesim_Text_Font *f;
    Enesim_Text_Engine *e;
    Enesim_Rectangle geom;
    int i;

    if (!chart)
        return NULL;

    c = enesim_renderer_compound_new();

    /* background */
    r = enesim_renderer_rectangle_new();
    enesim_renderer_rectangle_position_set(r, 0, 0);
    enesim_renderer_rectangle_size_set(r, chart->width, chart->height);
    enesim_renderer_shape_fill_color_set(r, chart->background_color);
    enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);

    /* grid */
    l = enesim_renderer_compound_layer_new();
    enesim_renderer_compound_layer_renderer_set(l, r);
    enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_FILL);
    enesim_renderer_compound_layer_add(c, l);

    for (i = 0; i < chart->grid.x_nbr; i++)
    {
        r = enesim_renderer_line_new();
        enesim_renderer_line_coords_set(r, (i * (chart->width - 1)) / (double)(chart->grid.x_nbr - 1), 0, (i * (chart->width - 1)) / (double)(chart->grid.x_nbr - 1), chart->height);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        if (i == 0)
            enesim_renderer_shape_stroke_color_set(r, 0xff000000);
        else
            enesim_renderer_shape_stroke_color_set(r, chart->grid.color);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    for (i = 0; i < chart->grid.y_nbr; i++)
    {
        r = enesim_renderer_line_new();
        enesim_renderer_line_coords_set(r, 0, (i * (chart->height - 1)) / (double)(chart->grid.y_nbr - 1), chart->width, (i * (chart->height - 1)) / (double)(chart->grid.y_nbr - 1));
        enesim_renderer_shape_stroke_weight_set(r, 1);
        if (i == (chart->grid.y_nbr - 1))
            enesim_renderer_shape_stroke_color_set(r, 0xff000000);
        else
            enesim_renderer_shape_stroke_color_set(r, chart->grid.color);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    for (i = 0; i < chart->grid.x_nbr; i++)
    {
        int j;

        for (j = 1; j < (chart->sub_grid.x_nbr - 1); j++)
        {
            Enesim_Path *p;

            p = enesim_path_new();
            enesim_path_move_to(p, (chart->width - 1) * (j + i * (chart->sub_grid.x_nbr - 1)) / (double)((chart->grid.x_nbr - 1) * (chart->sub_grid.x_nbr - 1)), 1);
            enesim_path_line_to(p, (chart->width - 1) * (j + i * (chart->sub_grid.x_nbr - 1)) / (double)((chart->grid.x_nbr - 1) * (chart->sub_grid.x_nbr - 1)), chart->height - 1);

            r = enesim_renderer_path_new();
            enesim_renderer_path_path_set(r, p);
            enesim_renderer_shape_stroke_weight_set(r, 1);
            enesim_renderer_shape_stroke_dash_add_simple(r, 10, 8);
            enesim_renderer_shape_stroke_color_set(r, chart->sub_grid.color);
            enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

            l = enesim_renderer_compound_layer_new();
            enesim_renderer_compound_layer_renderer_set(l, r);
            enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
            enesim_renderer_compound_layer_add(c, l);
        }
    }

    for (i = 0; i < chart->grid.y_nbr; i++)
    {
        int j;

        for (j = 1; j < (chart->sub_grid.y_nbr - 1); j++)
        {
            Enesim_Path *p;

            p = enesim_path_new();
            enesim_path_move_to(p, 1, (chart->height - 1) * (j + i * (chart->sub_grid.y_nbr - 1)) / (double)((chart->grid.y_nbr - 1) * (chart->sub_grid.y_nbr - 1)));
            enesim_path_line_to(p, chart->width - 1, (chart->height - 1) * (j + i * (chart->sub_grid.y_nbr - 1)) / (double)((chart->grid.y_nbr - 1) * (chart->sub_grid.y_nbr - 1)));

            r = enesim_renderer_path_new();
            enesim_renderer_path_path_set(r, p);
            enesim_renderer_shape_stroke_weight_set(r, 1);
            enesim_renderer_shape_stroke_dash_add_simple(r, 10, 8);
            enesim_renderer_shape_stroke_color_set(r, chart->sub_grid.color);
            enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

            l = enesim_renderer_compound_layer_new();
            enesim_renderer_compound_layer_renderer_set(l, r);
            enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
            enesim_renderer_compound_layer_add(c, l);
        }
    }

    e = enesim_text_engine_default_get();
    f = enesim_text_font_new_description_from(e, "arial", 16);
    enesim_text_engine_unref(e);

    r = enesim_renderer_text_span_new();
    enesim_renderer_color_set(r, 0xff000000);
    enesim_renderer_text_span_text_set(r, "Hello World!");
    enesim_renderer_text_span_font_set(r, f);

    enesim_renderer_shape_destination_geometry_get(r, &geom);
    enesim_renderer_origin_set(r, (chart->width - geom.w) / 2, chart->height - geom.h);

    l = enesim_renderer_compound_layer_new();
    enesim_renderer_compound_layer_renderer_set(l, r);
    enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
    enesim_renderer_compound_layer_add(c, l);

    return c;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Echart_Chart *
echart_chart_new(void)
{
    Echart_Chart *chart;

    chart = (Echart_Chart *)calloc(1, sizeof(Echart_Chart));
    if (!chart)
        return NULL;

    chart->width = 800;
    chart->height = 600;
    enesim_argb_components_from(&chart->background_color, 255, 128, 128, 128);
    chart->grid.x_nbr = 5;
    chart->grid.y_nbr = 5;
    chart->grid.color = 0xff888888;
    chart->sub_grid.x_nbr = 5;
    chart->sub_grid.y_nbr = 5;
    chart->sub_grid.color = 0xffeeeeee;

    return chart;
}

EAPI void
echart_chart_free(Echart_Chart *chart)
{
    if (!chart)
        return;

    if (chart->data)
        echart_data_free(chart->data);
    free(chart);
}

EAPI void
echart_chart_size_set(Echart_Chart *chart, int width, int height)
{
    if (!chart || (width <= 0) || (height >= 0))
        return;

    chart->width = width;
    chart->height = height;
}

EAPI void
echart_chart_size_get(const Echart_Chart *chart, int *width, int *height)
{
    if (!chart)
    {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }

    if (width) *width = chart->width;
    if (height) *height = chart->height;
}

EAPI void
echart_chart_background_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    if (!chart)
        return;

    enesim_argb_components_from(&chart->background_color, a, r, g, b);
}

EAPI Enesim_Argb
echart_chart_background_color_get(const Echart_Chart *chart)
{
    if (!chart)
        return 0xffffffff;

    return chart->background_color;
}

EAPI void
echart_chart_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr)
{
    if (!chart || (grid_x_nbr < 0) || (grid_y_nbr < 0))
        return;

    chart->grid.x_nbr = grid_x_nbr;
    chart->grid.y_nbr = grid_y_nbr;
}

EAPI void
echart_chart_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr)
{
    if (!chart)
    {
        if (grid_x_nbr) *grid_x_nbr = 0;
        if (grid_y_nbr) *grid_y_nbr = 0;
        return;
    }

    if (grid_x_nbr) *grid_x_nbr = chart->grid.x_nbr;
    if (grid_y_nbr) *grid_y_nbr = chart->grid.y_nbr;
}

EAPI void
echart_chart_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    if (!chart)
        return;

    enesim_argb_components_from(&chart->grid.color, a, r, g, b);
}

EAPI Enesim_Argb
echart_chart_grid_color_get(const Echart_Chart *chart)
{
    if (!chart)
        return 0xffcccccc;

    return chart->grid.color;
}

EAPI void
echart_chart_data_set(Echart_Chart *chart, Echart_Data *data)
{
    if (!chart || !data)
        return;

    chart->data = data;
}

EAPI const Echart_Data *
echart_chart_data_get(const Echart_Chart *chart)
{
    if (!chart)
        return NULL;;

    return chart->data;
}
