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

#ifndef ECHART_H
#define ECHART_H

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ECHART_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

typedef struct _Echart_Line Echart_Line;
typedef struct _Echart_Chart Echart_Chart;
typedef struct _Echart_Data Echart_Data;
typedef struct _Echart_Data_Item Echart_Data_Item;

typedef struct _Echart_Colors Echart_Colors;

struct _Echart_Colors
{
    Enesim_Argb line;
    Enesim_Argb area;
};

EAPI int echart_init(void);
EAPI int echart_shutdown(void);

EAPI Echart_Chart *echart_chart_new(void);
EAPI void echart_chart_free(Echart_Chart *chart);
EAPI void echart_chart_size_set(Echart_Chart *chart, int width, int height);
EAPI void echart_chart_size_get(const Echart_Chart *chart, int *width, int *height);
EAPI void echart_chart_background_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI Enesim_Argb echart_chart_background_color_get(const Echart_Chart *chart);
EAPI void echart_chart_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr);
EAPI void echart_chart_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr);
EAPI void echart_chart_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI Enesim_Argb echart_chart_grid_color_get(const Echart_Chart *chart);
EAPI void echart_chart_sub_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr);
EAPI void echart_chart_sub_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr);
EAPI void echart_chart_sub_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI Enesim_Argb echart_chart_sub_grid_color_get(const Echart_Chart *chart);
EAPI void echart_chart_data_set(Echart_Chart *chart, Echart_Data *data);
EAPI const Echart_Data *echart_chart_data_get(const Echart_Chart *chart);

EAPI Echart_Data *echart_data_new(void);
EAPI void echart_data_free(Echart_Data *data);
EAPI void echart_data_title_set(Echart_Data *data, const char *title);
EAPI const char *echart_data_title_get(const Echart_Data *data);
EAPI void echart_data_absciss_set(Echart_Data *data, const Echart_Data_Item *absciss);
EAPI const Echart_Data_Item *echart_data_absciss_get(const Echart_Data *data);
EAPI void echart_data_items_set(Echart_Data *data, Echart_Data_Item *item);
EAPI unsigned int echart_data_items_count(const Echart_Data *data);
EAPI const Echart_Data_Item *echart_data_items_get(const Echart_Data *data, int idx);

EAPI Echart_Data_Item *echart_data_item_new(void);
EAPI void echart_data_item_free(Echart_Data_Item *item);
EAPI void echart_data_item_title_set(Echart_Data_Item *item, const char *title);
EAPI const char *echart_data_item_title_get(const Echart_Data_Item *item);
EAPI void echart_data_item_color_set(Echart_Data_Item *item, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI Echart_Colors echart_data_item_color_get(const Echart_Data_Item *item);
EAPI void echart_data_item_value_add(Echart_Data_Item *item, double d);
EAPI const Eina_List *echart_data_item_values_get(const Echart_Data_Item *item);
EAPI void echart_data_item_interval_get(const Echart_Data_Item *item, double *vmin, double *vmax);

EAPI Echart_Line *echart_line_new(void);
EAPI void echart_line_chart_free(Echart_Line *line);
EAPI void echart_line_chart_set(Echart_Line *line, const Echart_Chart *chart);
EAPI const Echart_Chart *echart_line_chart_get(const Echart_Line *line);
EAPI void echart_line_area_set(Echart_Line *line, Eina_Bool area);
EAPI Eina_Bool echart_line_area_get(const Echart_Line *line);
EAPI void echart_line_stacked_set(Echart_Line *line, Eina_Bool stacked);
EAPI Eina_Bool echart_line_stacked_get(const Echart_Line *line);
EAPI Enesim_Renderer *echart_line_renderer_get(const Echart_Line *line);

#endif
