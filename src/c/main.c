#include <pebble.h>

#define CHAR_LIMIT 140

Window *my_window;
TextLayer *time_layer, *message_layer;

GFont font_time;
GFont font_message;

char message[CHAR_LIMIT];

GColor color_top_text;
GColor color_bot_text;

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

// Update the time and lol
void update_time(struct tm *tick_time) {
    text_layer_set_text(time_layer, write_time(*tick_time));
}

// Update the lol
void update_lol(char *text) {
    text_layer_set_text(message_layer, text);
}

// Ticker handler
void handle_tick(struct tm *tick_time, TimeUnits units) {
    if(init_finished==1) {
        update_time(tick_time);
    }
    init_finished = 1;
}

void init_colors() {
    color_bot_text = GColorBlack;
    color_top_text = GColorBlack;
}

void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *chosen_color_t = dict_find(iter, MESSAGE_KEY_MESSAGE);

    if (chosen_color_t) {
        strncpy(message, chosen_color_t->value->cstring, CHAR_LIMIT);
        persist_write_string(MESSAGE_KEY_MESSAGE, message);
    }
    update_lol(message);
}

void main_window_load() {
    printf("main_window_load");
    Layer *window_layer = window_get_root_layer(my_window);
    GRect bounds = layer_get_bounds(window_layer);

    font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_CUSTOM_FONT_42));
    font_message = fonts_get_system_font(FONT_KEY_GOTHIC_28 );

    time_layer = text_layer_create(GRect(0, 14, bounds.size.w, 80));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, color_top_text);
    text_layer_set_font(time_layer, font_time);
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);

    message_layer = text_layer_create(GRect(0, 55, bounds.size.w, 113));
    text_layer_set_background_color(message_layer, GColorClear);
    text_layer_set_text_color(message_layer, color_bot_text);
    text_layer_set_font(message_layer, font_message);
    text_layer_set_text_alignment(message_layer, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(time_layer));
    layer_add_child(window_layer, text_layer_get_layer(message_layer));

    // If any persistant data then load those and update colors
    if (persist_read_string(MESSAGE_KEY_MESSAGE, message, CHAR_LIMIT)) {
        printf("persist_read_string");
        // Do nothing?
    } else {
    }
    update_lol(message);

}

void main_window_unload() {
    layer_destroy(text_layer_get_layer(time_layer));
    layer_destroy(text_layer_get_layer(message_layer));
    fonts_unload_custom_font(font_time);
}

void init() {
    my_window = window_create();

    srand(time(NULL));
    init_colors();

    // Set handlers to manage the elements inside the Window
    printf("init");
    window_set_window_handlers(my_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(my_window, true);

    // Time & LOL for display on start up
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