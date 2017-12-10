#include <pebble.h>

#define WINDOW_W 144
#define WINDOW_H 168

#define TIME_X 0
#define TIME_Y 138
#define TIME_W WINDOW_W
#define TIME_H WINDOW_H - TIME_Y

#define TIME_TEXT_X TIME_X
#define TIME_TEXT_Y TIME_Y - 4
#define TIME_TEXT_W TIME_W
#define TIME_TEXT_H 40

#define MESSAGE_TEXT_X 4
#define MESSAGE_TEXT_Y -4
#define MESSAGE_TEXT_W WINDOW_W - 8
#define MESSAGE_TEXT_H 120

#define LOCATION_TEXT_X 4
#define LOCATION_TEXT_Y 115
#define LOCATION_TEXT_W WINDOW_W - 8
#define LOCATION_TEXT_H 20

#define CUSTOM_FONT RESOURCE_ID_CUSTOM_FONT_20

#define CHAR_LIMIT 140

Window *my_window;
TextLayer *text_layer_time;
TextLayer *text_layer_message;
TextLayer *text_layer_location;

GFont font_time;
GFont font_message;
GFont font_location;

char message[CHAR_LIMIT];
char location[CHAR_LIMIT];

GColor color_primary;
GColor color_secondary;

int init_finished;

// Returns the time
static char *write_time(struct tm tick_time) {
    // Create a long-lived buffer
    static char buffer[] = "00:00";

    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
        // Use 24 hour format
        strftime(buffer, sizeof("00:00"), "%H:%M", &tick_time);
    } else {
        // Use 12 hour format
        strftime(buffer, sizeof("00:00"), "%I:%M", &tick_time);
    }

    // Strip leading zero
    if(buffer[0]=='0') strcpy(buffer, buffer+1);

    return (char *)buffer;
}

void update_time(struct tm *tick_time) {
    text_layer_set_text(text_layer_time, write_time(*tick_time));
}

void update_message(char *text) {
    text_layer_set_text(text_layer_message, text);
}

void update_location(char *text) {
    text_layer_set_text(text_layer_location, text);
}

// Ticker handler
void handle_tick(struct tm *tick_time, TimeUnits units) {
    if(init_finished==1) {
        update_time(tick_time);
    }
    init_finished = 1;
}

void init_colors() {
    color_primary = GColorBlack;
    color_secondary = GColorWhite;
}

void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *recieved_message = dict_find(iter, MESSAGE_KEY_MESSAGE);
    Tuple *recieved_location = dict_find(iter, MESSAGE_KEY_LOCATION);

    if (recieved_message) {
        strncpy(message, recieved_message->value->cstring, CHAR_LIMIT);
        persist_write_string(MESSAGE_KEY_MESSAGE, message);
        update_message(message);
    }

    if (recieved_location) {
        strncpy(location, recieved_location->value->cstring, CHAR_LIMIT);
        persist_write_string(MESSAGE_KEY_LOCATION, location);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s\n", location);
        update_location(location);
    }
}

void draw_background(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_fill_color(ctx, color_secondary);
    graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, color_primary);
    graphics_fill_rect(ctx, GRect(TIME_X, TIME_Y, TIME_W, TIME_H), 8, GCornersTop);
}

void main_window_load() {
    Layer *layer_window = window_get_root_layer(my_window);
    layer_set_update_proc(layer_window, draw_background);

    font_time = fonts_get_system_font(FONT_KEY_GOTHIC_28);
    // font_message = fonts_load_custom_font(resource_get_handle(CUSTOM_FONT));
    font_message = fonts_get_system_font(FONT_KEY_GOTHIC_24);
    font_location = fonts_get_system_font(FONT_KEY_GOTHIC_18);

    text_layer_time = text_layer_create(GRect(TIME_TEXT_X, TIME_TEXT_Y, TIME_TEXT_W, TIME_TEXT_H));
    text_layer_set_background_color(text_layer_time, GColorClear);
    text_layer_set_text_color(text_layer_time, color_secondary);
    text_layer_set_font(text_layer_time, font_time);
    text_layer_set_text_alignment(text_layer_time, GTextAlignmentCenter);

    text_layer_message = text_layer_create(GRect(MESSAGE_TEXT_X, MESSAGE_TEXT_Y, MESSAGE_TEXT_W, MESSAGE_TEXT_H));
    text_layer_set_background_color(text_layer_message, GColorClear);
    text_layer_set_text_color(text_layer_message, color_primary);
    text_layer_set_font(text_layer_message, font_message);
    text_layer_set_text_alignment(text_layer_message, GTextAlignmentLeft);
    text_layer_set_overflow_mode(text_layer_message, GTextOverflowModeTrailingEllipsis);

    text_layer_location = text_layer_create(GRect(LOCATION_TEXT_X, LOCATION_TEXT_Y, LOCATION_TEXT_W, LOCATION_TEXT_H));
    text_layer_set_background_color(text_layer_location, GColorClear);
    text_layer_set_text_color(text_layer_location, color_primary);
    text_layer_set_font(text_layer_location, font_location);
    text_layer_set_text_alignment(text_layer_location, GTextAlignmentLeft);
    text_layer_set_overflow_mode(text_layer_location, GTextOverflowModeTrailingEllipsis);

    layer_add_child(layer_window, text_layer_get_layer(text_layer_time));
    layer_add_child(layer_window, text_layer_get_layer(text_layer_message));
    layer_add_child(layer_window, text_layer_get_layer(text_layer_location));

    if (persist_exists(MESSAGE_KEY_MESSAGE)) {
        persist_read_string(MESSAGE_KEY_MESSAGE, message, CHAR_LIMIT);
        update_message(message);
    }

    if (persist_exists(MESSAGE_KEY_LOCATION)) {
        persist_read_string(MESSAGE_KEY_LOCATION, location, CHAR_LIMIT);
        update_location(location);
    }

}

void main_window_unload() {
    layer_destroy(text_layer_get_layer(text_layer_time));
    layer_destroy(text_layer_get_layer(text_layer_message));
    layer_destroy(text_layer_get_layer(text_layer_location));
    // fonts_unload_custom_font(font_message);
}

void init() {
    my_window = window_create();

    srand(time(NULL));
    init_colors();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(my_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(my_window, true);

    // Time for display on start up
    init_finished = 0;
    time_t temp_time = time(NULL);
    struct tm *tick_time = localtime(&temp_time);
    update_time(tick_time);

    // Subcribe to ticker
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit() {
    window_destroy(my_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}