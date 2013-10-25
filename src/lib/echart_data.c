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

struct _Echart_Data_Item
{
    char *title;
    Echart_Colors color;
    Eina_List *values;
    double vmin;
    double vmax;
};

struct _Echart_Data
{
    char *title;
    Echart_Data_Item *absciss;
    Eina_List *items;
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Echart_Data *
echart_data_stacked_get(const Echart_Data *data)
{
    Echart_Data *stacked;
    Echart_Data_Item *item;
    Echart_Data_Item *item_prev;
    Echart_Data_Item *stacked_item;
    unsigned int i;
    unsigned int j;

    stacked = echart_data_new();
    if (!stacked)
        return NULL;

    stacked->title = strdup(data->title);
    if (!stacked->title)
        goto free_data;

    for (i = 0; i < eina_list_count(data->items); i++)
    {
        /* absciss  and 1st value */
        if ((i == 0) || (i == 1))
        {
            item = eina_list_nth(data->items, i);
            stacked_item = echart_data_item_new();
            stacked_item->title = strdup(item->title);
            stacked_item->color = item->color;
            stacked_item->vmin = item->vmin;
            stacked_item->vmax = item->vmax;
            stacked_item->values = eina_list_clone(item->values);
            echart_data_items_set(stacked, stacked_item);
        }
        else
        {
            item_prev = eina_list_nth(data->items, i-1);
            item = eina_list_nth(data->items, i);
            stacked_item = echart_data_item_new();
            stacked_item->title = strdup(item->title);
            stacked_item->color = item->color;
            stacked_item->vmin = item->vmin;
            stacked_item->vmax = item->vmax;
            for (j = 0; j < eina_list_count(item->values); j++)
            {
                double d1;
                double d2;

                d1 = *(double *)eina_list_nth(item_prev->values, j);
                d2 = *(double *)eina_list_nth(item->values, j);
                echart_data_item_value_add(stacked_item, d1 + d2);
                printf("%f   %f %f\n", d1, d2, d1 + d2);
            }
            echart_data_items_set(stacked, stacked_item);
        }
    }

    return stacked;

  free_data:
    echart_data_free(stacked);
    return NULL;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Echart_Data *
echart_data_new(void)
{
    Echart_Data *data;

    data = (Echart_Data *)calloc(1, sizeof(Echart_Data));
    if (!data)
        return NULL;

    return data;
}

EAPI void
echart_data_free(Echart_Data *data)
{
    if (!data)
        return;

    if (data->title)
        free(data->title);
    free(data);
}

EAPI void
echart_data_title_set(Echart_Data *data, const char *title)
{
    if (!data || !title || !*title)
        return;

    data->title = strdup(title);
}

EAPI const char *
echart_data_title_get(const Echart_Data *data)
{
    if (!data)
        return NULL;

    return data->title;
}

EAPI void
echart_data_absciss_set(Echart_Data *data, const Echart_Data_Item *absciss)
{
    unsigned int count;

    if (!data || !absciss)
        return;

    data->absciss = (Echart_Data_Item *)absciss;
}

EAPI const Echart_Data_Item *
echart_data_absciss_get(const Echart_Data *data)
{
    if (!data)
        return NULL;

    return data->absciss;
}

EAPI void
echart_data_items_set(Echart_Data *data, Echart_Data_Item *item)
{
    unsigned int count;

    if (!data || !item)
        return;

    count = eina_list_count(data->items);
    if (count == 20)
    {
        WRN("Maximum items count reached");
        return;
    }

    if (eina_list_count(data->absciss->values) != eina_list_count(item->values))
    {
        WRN("Adding an item with different values count");
        return;
    }
    item->color = echart_chart_default_colors[count];
    data->items = eina_list_append(data->items, item);
}

EAPI unsigned int
echart_data_items_count(const Echart_Data *data)
{
    if (!data)
        return 0;

    return eina_list_count(data->items);
}

EAPI const Echart_Data_Item *
echart_data_items_get(const Echart_Data *data, int idx)
{
    if (!data || (idx < 0) || ((unsigned int)idx >= eina_list_count(data->items)))
        return NULL;

    return eina_list_nth(data->items, idx);
}

EAPI Echart_Data_Item *
echart_data_item_new(void)
{
    Echart_Data_Item *item;

    item = (Echart_Data_Item *)calloc(1, sizeof(Echart_Data_Item));
    if (!item)
        return NULL;

    return item;
}

EAPI void
echart_data_item_free(Echart_Data_Item *item)
{
    if (!item)
        return;

    if (item->title)
        free(item->title);
    free(item);
}

EAPI void
echart_data_item_title_set(Echart_Data_Item *item, const char *title)
{
    if (!item || !title || !*title)
        return;

    item->title = strdup(title);
}

EAPI const char *
echart_data_item_title_get(const Echart_Data_Item *item)
{
    if (!item)
        return NULL;

    return item->title;
}

EAPI void
echart_data_item_color_set(Echart_Data_Item *item, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    if (!item)
        return;

    enesim_argb_components_from(&item->color.line, a, r, g, b);
}

EAPI Echart_Colors
echart_data_item_color_get(const Echart_Data_Item *item)
{
    Echart_Colors black = { 0xff000000, 0xff000000 };

    if (!item)
        return black;

    return item->color;
}

EAPI void
echart_data_item_value_add(Echart_Data_Item *item, double value)
{
    double *v;

    if (!item)
        return;

    v = (double *)malloc(sizeof(double));
    if (!v)
        return;

    *v = value;

    item->values = eina_list_append(item->values, v);
    if (eina_list_count(item->values) == 1)
    {
        item->vmin = value;
        item->vmax = value;
    }
    else
    {
        if (value < item->vmin) item->vmin = value;
        if (value > item->vmax) item->vmax = value;
    }
}

EAPI const Eina_List *
echart_data_item_values_get(const Echart_Data_Item *item)
{
    if (!item)
        return NULL;

    return item->values;
}

EAPI void
echart_data_item_interval_get(const Echart_Data_Item *item, double *vmin, double *vmax)
{
    if (!item)
    {
        if (vmin) *vmin = 0.0;
        if (vmax) *vmax = 0.0;
    }

    if (vmin) *vmin = item->vmin;
    if (vmax) *vmax = item->vmax;
}
