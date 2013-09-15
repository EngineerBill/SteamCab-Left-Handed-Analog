/* ------------------------------------------------------------------ *\

               SteamCab Left Handed Analog - Mark II

               A counter-revolutionary approach to time...


Modifies the example analog watchface app for counterclockwise operation.
Also demonstrates adding a TextLayer to an existing watchface app.


	Author:				Pebble Central
	Modified:			EngineerBill (engineerbill@stemchest.com)
	Date Created:		8/5/13

	Release History:
	
		Mark I		- counterclockwise watchface, TextLayer
		
		Mark II		- Removed Second Hand (customer request)


About:
------

Steamchest Enterprises' first foray into the world of Pebble Watchface design.

Our initial investigations into the world of Pebble programming focused on
understanding the structure and requirements for a working Watchface app.
After studying a number of examples and reviewing what was already out there,
we decided to roll our own. The result was the 'SteamCab Left Handed Analog - Mark I',
a "counter-revolutionary" approach to analog watch design. A follow-on Mark II release
eliminated the second hand at the request of redditor "/reddit.com/u/arsirion".

This project began life as the example "simple_analog" example watch in
thePebble SDK. The original was modified to provide counterclockwise hand
movement and a new layer containing a text string. We then made an annotation
pass through the source code to help those who would follow.

The changes to convert to counterclock operation are quite minor, but the
module serves to teach the basic structure of a watchface app and the steps
needed to add and display a TextBox to a window. 


You can find the original example source code in the Pebble SDK
watches directory:

       "SDK-1.12->Pebble->watches->simple_analog".


Contact Us:
-----------

email:  engineerbill@steamchest.com
web:    www.steamchest.com (coming soon)

blog:   steamcab.wordpress.com (documenting my voyage into Pebble programming)


The Steamchest Chronicles:
--------------------------

       (c) 2013
 the Steamchest Chronicles

"A Journey of Discovery through the World of Smart Mobile Technology…"

\* ------------------------------------------------------------------ */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "simpleAnalog.h"
//#include "string.h"
//#include "stdlib.h" 

/* -------------------------------------------------------- *\
              Program Declares Section
\* -------------------------------------------------------- */

//  UUID for Original Pebble Sample Program - SimpleAnalog.c
//#define MY_UUID {0xe0, 0xfc, 0x1c, 0x31, 0x46, 0x8c, 0x4f, 0x69, 0xba, 0xbd, 0xd2, 0xec, 0x7a, 0xb3, 0x18, 0x13}

//  UUID for SteamCab - Mark I (with second hand)
//#define MY_UUID {0x16, 0x9D, 0x54, 0xC0, 0xEB, 0x8a, 0x11, 0xE2, 0x91, 0xE2, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66}

//  UUID for SteamCab - Mark II (without second hand)
#define MY_UUID {0xd0, 0x40, 0x43, 0xfc, 0xed, 0xaf, 0x11, 0xe2, 0xac, 0x57, 0xf2, 0x3c, 0x91, 0xae, 0xc0, 0x5e}

//char *SteamCabVersion = ".com";   // holds current


PBL_APP_INFO(MY_UUID,
             "SteamCab Analog Mk II",
             "SteamChest Chronicles",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

// --------------------------------------------------------
//              SteamCab Declares
// --------------------------------------------------------

int left_handed = 1;	// set to 0 to run right-handed, 1 to run left-handed
int second_hand = 0;	// set to 0 to hide second hand, 1 to show

// --------------------------------------------------------
//              Original Simple Analog Declares
// --------------------------------------------------------
static struct SimpleAnalogData {
  Layer simple_bg_layer;

  Layer date_layer;

  TextLayer title_label;   // added for SteamCab title
  char title_buffer[10];   // added for SteamCab title
	
  TextLayer version_label; // added for SteamCab version number
  char version_buffer[6];  // added for SteamCab version number

  TextLayer day_label;
  char day_buffer[6];

  TextLayer num_label;
  char num_buffer[4];

  GPath minute_arrow, hour_arrow;
  GPath tick_paths[NUM_CLOCK_TICKS];
  Layer hands_layer;
  Window window;          // clock window
} s_data;


/* -------------------------------------------------------- *\
              Clock Update Routines
\* -------------------------------------------------------- */

static void bg_update_proc(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, me->bounds, 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_draw_filled(ctx, &s_data.tick_paths[i]);
  }
}

static void hands_update_proc(Layer* me, GContext* ctx) {
  const GPoint center = grect_center_point(&me->bounds);
  const int16_t secondHandLength = me->bounds.size.w / 2;

  GPoint secondHand;
  PblTm t;
  int leftTime;
  get_time(&t);

// ------------------------------------------------
//   Remove this section to eliminate second hand  
// ------------------------------------------------

	if(second_hand) {
	// Added for Left Handed Operation
		if(left_handed) {
		  	leftTime = 60 - t.tm_sec;
		}
		else {
			leftTime = t.tm_sec;
		}
		
	// calculate new angle
	//  int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;  // uncomment for right-handed operation
		int32_t second_angle = TRIG_MAX_ANGLE * leftTime / 60;  // uncomment for left-handed operation

		secondHand.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.y;
		secondHand.x = (int16_t)(sin_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.x;

  // draw second hand
		graphics_context_set_stroke_color(ctx, GColorWhite);
		graphics_draw_line(ctx, secondHand, center);
	}
	
  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

	// Added for Left Handed Operation
		if(left_handed) {
		  	leftTime = 60 - t.tm_min;
		}
		else {
			leftTime = t.tm_min;
		}

	//  gpath_rotate_to(&s_data.minute_arrow, TRIG_MAX_ANGLE * t.tm_min / 60);
  gpath_rotate_to(&s_data.minute_arrow, TRIG_MAX_ANGLE * leftTime / 60);
  gpath_draw_filled(ctx, &s_data.minute_arrow);
  gpath_draw_outline(ctx, &s_data.minute_arrow);

	// Added for Left Handed Operation
		if(left_handed) {
		  	leftTime = 12 - t.tm_hour;
		}
		else {
			leftTime = t.tm_hour;
		}

//  gpath_rotate_to(&s_data.hour_arrow, (TRIG_MAX_ANGLE * (((t.tm_hour % 12) * 6) + (t.tm_min / 10))) / (12 * 6));
  gpath_rotate_to(&s_data.hour_arrow, (TRIG_MAX_ANGLE * (((leftTime % 12) * 6) - (t.tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, &s_data.hour_arrow);
  gpath_draw_outline(ctx, &s_data.hour_arrow);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(me->bounds.size.w / 2-1, me->bounds.size.h / 2-1, 3, 3), 0, GCornerNone);
}

static void date_update_proc(Layer* me, GContext* ctx) {

  PblTm t;
  get_time(&t);

  string_format_time(s_data.day_buffer, sizeof(s_data.day_buffer), "%a", &t);
  text_layer_set_text(&s_data.day_label, s_data.day_buffer);

  string_format_time(s_data.num_buffer, sizeof(s_data.num_buffer), "%d", &t);
  text_layer_set_text(&s_data.num_label, s_data.num_buffer);
}

/* -------------------------------------------------------- *\
              handle_init()
			  
		Initialize Program on start
\* -------------------------------------------------------- */

static void handle_init(AppContextRef app_ctx) {

  // create clock window 
  window_init(&s_data.window, "SteamCab Analog");
 
  // set up new Text Layer, hand paths, init layers, hands, etc
  // Change text here to customize for your needs... 
  strcpy(s_data.title_buffer, "SteamCab");		// Max buffer sizes are currently 9 chars & 5 chars!! 
  strcpy(s_data.version_buffer, ".com");		// (to allow for trailing nulls)
  s_data.day_buffer[0] = '\0';					// used to hold current date
  s_data.num_buffer[0] = '\0';					// 

  // init hand paths
  gpath_init(&s_data.minute_arrow, &MINUTE_HAND_POINTS);
  gpath_init(&s_data.hour_arrow, &HOUR_HAND_POINTS);

  const GPoint center = grect_center_point(&s_data.window.layer.bounds);
  gpath_move_to(&s_data.minute_arrow, center);
  gpath_move_to(&s_data.hour_arrow, center);

  // init clock face paths
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_init(&s_data.tick_paths[i], &ANALOG_BG_POINTS[i]);
  }

  // init layers
  layer_init(&s_data.simple_bg_layer, s_data.window.layer.frame);
  s_data.simple_bg_layer.update_proc = &bg_update_proc;
  layer_add_child(&s_data.window.layer, &s_data.simple_bg_layer);

  // init date layer -> a plain parent layer to create a date update proc
  layer_init(&s_data.date_layer, s_data.window.layer.frame);
  s_data.date_layer.update_proc = &date_update_proc;
  layer_add_child(&s_data.window.layer, &s_data.date_layer);

//  added for SteamCab - init title on face
  text_layer_init(&s_data.title_label, GRect(40, 27, 87, 18));			// tweak here to adjust title location
  text_layer_set_text(&s_data.title_label, s_data.title_buffer);		// this is 40 pixels across, 27 down
  text_layer_set_background_color(&s_data.title_label, GColorBlack);	// 87 wide, 18 high
  text_layer_set_text_color(&s_data.title_label, GColorWhite);
  GFont norm18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  text_layer_set_font(&s_data.title_label, norm18);

  layer_add_child(&s_data.window.layer, &s_data.title_label.layer);

//  added for SteamCab - init extension on face (was to be version, now ".com")
  text_layer_init(&s_data.version_label, GRect(56, 45, 57, 18));			// tweak here to adjust version location
  text_layer_set_text(&s_data.version_label, s_data.version_buffer);		// this is 56 pixels across, 45 down 
  text_layer_set_background_color(&s_data.version_label, GColorBlack);		// 57 wide, 18 high
  text_layer_set_text_color(&s_data.version_label, GColorWhite);
  text_layer_set_font(&s_data.version_label, norm18);

  layer_add_child(&s_data.window.layer, &s_data.version_label.layer);

  // init day
  text_layer_init(&s_data.day_label, GRect(46, 114, 27, 20));
  text_layer_set_text(&s_data.day_label, s_data.day_buffer);
  text_layer_set_background_color(&s_data.day_label, GColorBlack);
  text_layer_set_text_color(&s_data.day_label, GColorWhite);
//  GFont norm18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  text_layer_set_font(&s_data.day_label, norm18);

  layer_add_child(&s_data.date_layer, &s_data.day_label.layer);

  // init num
  text_layer_init(&s_data.num_label, GRect(73, 114, 18, 20));

  text_layer_set_text(&s_data.num_label, s_data.num_buffer);
  text_layer_set_background_color(&s_data.num_label, GColorBlack);
  text_layer_set_text_color(&s_data.num_label, GColorWhite);
  GFont bold18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(&s_data.num_label, bold18);

  layer_add_child(&s_data.date_layer, &s_data.num_label.layer);

  // init hands
  layer_init(&s_data.hands_layer, s_data.simple_bg_layer.frame);
  s_data.hands_layer.update_proc = &hands_update_proc;
  layer_add_child(&s_data.window.layer, &s_data.hands_layer);

  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(&s_data.window, animated);

}

static void handle_second_tick(AppContextRef ctx, PebbleTickEvent* t) {
  layer_mark_dirty(&s_data.window.layer);
}

void pbl_main(void* params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}