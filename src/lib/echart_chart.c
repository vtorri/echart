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
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Enesim_Renderer *
echart_chart_compound_get(Echart_Chart *chart)
{
    Enesim_Renderer *c;
    Enesim_Renderer *r;
    Enesim_Renderer_Compound_Layer *l;
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
            enesim_renderer_shape_stroke_color_set(r, 0xffcccccc);
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
            enesim_renderer_shape_stroke_color_set(r, 0xffcccccc);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

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
    chart->grid.color = 0xffcccccc;

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
