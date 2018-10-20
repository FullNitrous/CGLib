#include "cglib.h"
#include "cglib-other.h"

void print_slice(FILE* file, struct pie_data* pd, float* origin, float* sum_counter, int i, float radius, float sum, uint8_t overlap)
{
  float percentage_overlap = ((i + 1 < pd->n_slices) && overlap) ? pd->slices[i + 1].percentage / 2 : 0;
  percentage_overlap = (percentage_overlap == 0 && sum == 1.0 && overlap) ? pd->slices[0].percentage / 2 : percentage_overlap;
  float radians[2];
  radians[0] = 2 * (*sum_counter)*PI;
  radians[1] = 2 * ((pd->slices[i].percentage + percentage_overlap) + (*sum_counter))*PI;
  *sum_counter   += pd->slices[i].percentage;
  uint8_t large_arc_flag = (pd->slices[i].percentage + percentage_overlap > 0.5) ? 1 : 0;

  float start[2], stop[2];
  start[0] = cos(radians[0]) * radius + origin[0];
  start[1] = sin(radians[0]) * radius + origin[1];
  stop[0] =  cos(radians[1]) * radius + origin[0];
  stop[1] =  sin(radians[1]) * radius + origin[1];

  pd->theme->percentage = (i + 1) / (pd->n_slices*1.0);

  pd->theme->color_function(pd->theme);

  fprintf(file, svg_slice,
    start[0],
    start[1],
    radius,
    radius,
    large_arc_flag,
    stop[0],
    stop[1],
    origin[0],
    origin[1],
    pd->theme->out.r,
    pd->theme->out.g,
    pd->theme->out.b);
  return;
}

void pie(struct pie_data* pd)
{
  FILE* file;
  uint8_t large_arc_flag;
  float percentage_overlap, 
        radius, 
        origin[2], 
        radians[3], 
        start[2], 
        stop[2],
        origin_radius;

  origin_radius = 0.0;


  file = fopen(pd->general->file_name, "wb");
  print_top_header(file, pd->general);
  radius = (pd->general->viewport_y <= pd->general->viewport_x) ? (pd->general->viewport_y - pd->general->margin) / 2 : (pd->general->viewport_x - pd->general->margin) / 2;
  origin[0] = pd->general->viewport_x / 2.0 + pd->general->margin / 2.0;
  origin[1] = pd->general->viewport_y / 2.0 + pd->general->margin / 2.0;

  fprintf(file, svg_circle, origin[0], origin[1], origin_radius);

  float sum = 0.0;

  select_color_function(pd->theme);

  loop(pd->n_slices)
  {
    sum += pd->slices[i].percentage;
  }

  sum = roundf(sum * 100) / 100;

  print_font_size_group(file, pd->general);
  valid_pie_data(pd, sum)
  {
    float sum_counter;

    fprintf(file, svg_limiter_box);
    loop(pd->n_slices)
    {
      print_slice(file, pd, origin, &sum_counter, i, radius, sum, 1);
    }
    if(sum == 1.0)
    {
      sum_counter = 0.0;
      print_slice(file, pd, origin, &sum_counter, 0, radius, sum, 0);
    }
    print_slice_pointers(file, pd);
    fprintf(file, svg_group_stop);
  }
  fprintf(file, svg_group_stop);
  fprintf(file, svg_top_header_stop);
  fclose(file);
}