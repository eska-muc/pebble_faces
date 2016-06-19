#include <pebble.h>

/*
 * See: https://de.wikipedia.org/wiki/Berlin-Uhr
 *
 * Stefan Kuehnel, January 2016
 */

Window *my_window;
Layer *my_layer;

GRect *row0;
GRect *row1;
GRect *row2;
GRect *row3;

static void update_display(Layer *layer,GContext *ctx) {
  
  time_t current_time = time(NULL);
  struct tm *t = localtime(&current_time);
  
  int lights_in_row0 = t->tm_hour/5;
  int lights_in_row1 = t->tm_hour%5;
  int lights_in_row2 = t->tm_min/5;
  int lights_in_row3 = t->tm_min%5;
   
  APP_LOG(APP_LOG_LEVEL_DEBUG,"%d %d -> %d %d %d %d",t->tm_hour,t->tm_min,lights_in_row0,lights_in_row1,lights_in_row2,lights_in_row3);  
  graphics_context_set_fill_color(ctx,GColorBlack);  
  // clear background
  graphics_fill_rect(ctx,layer_get_bounds(layer),0,GCornerTopLeft);
  
  // draw rows 0,1 and 3
  
  for (int x=0;x<4;x++) {    
    graphics_context_set_stroke_color(ctx,GColorYellow);  
    if (x<lights_in_row0) {
      graphics_context_set_fill_color(ctx,GColorRed);      
    } else {
      graphics_context_set_fill_color(ctx,GColorBlack);  
    }
    graphics_fill_rect(ctx,row0[x],0,GCornerTopLeft);
    graphics_draw_rect(ctx,row0[x]);
    
    if (x<lights_in_row1) {
      graphics_context_set_fill_color(ctx,GColorRed);      
    } else {
      graphics_context_set_fill_color(ctx,GColorBlack);  
    }
    graphics_fill_rect(ctx,row1[x],0,GCornerTopLeft);
    graphics_draw_rect(ctx,row1[x]);
    
    graphics_context_set_stroke_color(ctx,GColorRed);
  
    if (x<lights_in_row3) {
      graphics_context_set_fill_color(ctx,GColorYellow);      
    } else {
      graphics_context_set_fill_color(ctx,GColorBlack);  
    }
    graphics_fill_rect(ctx,row3[x],0,GCornerTopLeft);
    graphics_draw_rect(ctx,row3[x]);
  }
  // draw row 2
  graphics_context_set_stroke_color(ctx,GColorRed);  
  for (int x=0;x<11;x++) {
    if (x<lights_in_row2) {
      if ((x+1)%3 == 0) {
        graphics_context_set_fill_color(ctx,GColorRed);   
      } else {
        graphics_context_set_fill_color(ctx,GColorYellow);    
      }
    } else {
      graphics_context_set_fill_color(ctx,GColorBlack);    
    }
    graphics_fill_rect(ctx,row2[x],0,GCornerTopLeft);
    graphics_draw_rect(ctx,row2[x]);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  // Bounds of the root window  0,0,144,168 for model "Time"
  GRect bounds = layer_get_bounds(window_layer);
  int width1=bounds.size.w/4;  
  int width2=bounds.size.w/12;  
  int x=0; 
  int y_offset = (bounds.size.h-bounds.size.w)/2;
  
  my_layer = layer_create(bounds);
  layer_add_child(window_layer,my_layer);
  
  row0 = malloc(4*sizeof(GRect));
  row1 = malloc(4*sizeof(GRect));
  row2 = malloc(11*sizeof(GRect));
  row3 = malloc(4*sizeof(GRect));

  // Init the rects for row 0, 1 and 3
  for (x=0;x<4;x++) {
    row0[x]=GRect(width1*x,y_offset+0,width1,width1);    
    row1[x]=GRect(width1*x,y_offset+width1,width1,width1);
    row3[x]=GRect(width1*x,y_offset+3*width1,width1,width1);
  }
  // Init the rects for row 2
  for (x=0;x<11;x++) {
    row2[x]=GRect(width2*x+6,y_offset+2*width1,width2,width1);    
  }  
  layer_set_update_proc(my_layer, update_display);
}

static void window_unload(Window *window) {
  layer_destroy(my_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(window_get_root_layer(my_window));
}

void handle_init(void) {
  my_window = window_create();
  window_set_window_handlers(my_window,(WindowHandlers){
    .load=window_load,
    .unload=window_unload,
  });    
  window_stack_push(my_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void handle_deinit(void) {
  window_destroy(my_window);
  tick_timer_service_unsubscribe();
  free(row0);
  free(row1);
  free(row2);
  free(row3);  
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}