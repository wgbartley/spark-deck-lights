#include <pebble.h>

#define KEY_TEMPERATURE	0
#define KEY_OUTLET1	1
#define KEY_OUTLET2	2
#define KEY_OUTLET3	3
#define KEY_OUTLET4	4
#define KEY_RELAYS	5

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

static Window *window;
static MenuLayer *menu_layer;
static TextLayer *text_layer;
static char buf[32] = "";
uint8_t relay_states = 0;

// Prototypes
static void send_spark(int key);
static void in_recv_handler(DictionaryIterator *iterator, void *context);
static void process_tuple(Tuple *t);
static void init(void);
static void deinit(void);
int main(void);
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);
void window_load(Window *window);
void window_unload(Window *window);
static void handle_accel_tap(AccelAxisType axis, int32_t direction);


static void send_spark(int key) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	Tuplet value = TupletCString(key, "");
	dict_write_tuplet(iter, &value);

	app_message_outbox_send();
}


static void in_recv_handler(DictionaryIterator *iter, void *ctx) {
	text_layer_set_text(text_layer, "in_recv_handler");

	Tuple *t = dict_read_first(iter);

	// If it's there
	if(t) process_tuple(t);

	while(t!=NULL) {
		t = dict_read_next(iter);

		if(t) process_tuple(t);
	}
}


static void process_tuple(Tuple *t) {
	text_layer_set_text(text_layer, "process_tuple");

	int key = t->key;

	int value = t->value->int32;

	char string_value[32];
	strcpy(string_value, t->value->cstring);

	switch(key) {
		case KEY_RELAYS:
			//snprintf(buf, 32, "Relay: %d", value);
			//text_layer_set_text(text_layer, buf);
			break;
		case KEY_TEMPERATURE:
			snprintf(buf, sizeof("XXX \u00B0F"), "%d \u00B0F", value);
			text_layer_set_text(text_layer, buf);
			break;
	}
}


static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return 4;
}


static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch(cell_index->row) {
		case 0:
			menu_cell_basic_draw(ctx, cell_layer, "Deck Lights", NULL, NULL);
			break;

		case 1:
			menu_cell_basic_draw(ctx, cell_layer, "Patio Rope Lights", NULL, NULL);
			break;

		case 2:
			menu_cell_basic_draw(ctx, cell_layer, "Umbrella", NULL, NULL);
			break;

		case 3:
			menu_cell_basic_draw(ctx, cell_layer, "Extra Outlet", NULL, NULL);
			break;

	}
}


void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch(cell_index->row) {
		case 0:
			send_spark(KEY_OUTLET2);
			break;

		case 1:
			send_spark(KEY_OUTLET1);
			break;

		case 2:
			send_spark(KEY_OUTLET4);
			break;

		case 3:
			send_spark(KEY_OUTLET3);
			break;
	}
}


static void handle_accel_tap(AccelAxisType axis, int32_t direction) {
	text_layer_set_text(text_layer, "Updating . . .");
	send_spark(KEY_TEMPERATURE);
}


void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

	text_layer = text_layer_create(GRect(0, 0, 144, 20));
	text_layer_set_text_alignment(text_layer, GAlignTopLeft);

	menu_layer = menu_layer_create(GRect(0, 20, 144, 132));

	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = NULL,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = NULL,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});

	menu_layer_set_click_config_onto_window(menu_layer, window);

	send_spark(KEY_TEMPERATURE);

	accel_tap_service_subscribe(&handle_accel_tap);

	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}


void window_unload(Window *window) {
	accel_tap_service_unsubscribe();
	menu_layer_destroy(menu_layer);
	text_layer_destroy(text_layer);
}


static void init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	// Prepare AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);

	send_spark(KEY_TEMPERATURE);

	light_enable(true);
	window_stack_push(window, false);
}


static void deinit(void) {
	light_enable(false);
	window_destroy(window);
}


int main(void) {
	init();
	app_event_loop();
	deinit();
}
