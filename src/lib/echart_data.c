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

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

struct _Echart_Data_Item
{
    char *title;
    Enesim_Argb color;
    Eina_List *values;
    double vmin;
    double vmax;
};

struct _Echart_Data
{
    char *title;
    Eina_List *items;
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
echart_data_items_set(Echart_Data *data, Echart_Data_Item *item)
{
    if (!data || !item)
        return;

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

    enesim_argb_components_from(&item->color, a, r, g, b);
}

EAPI Enesim_Argb
echart_data_item_color_get(const Echart_Data_Item *item)
{
    if (!item)
        return 0xff000000;

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
