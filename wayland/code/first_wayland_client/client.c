#include <stdio.h>
#include <string.h>
#include <wayland-client.h>
struct wl_seat *my_seat = NULL;

static void registry_handler(void *data, struct wl_registry *registry,
			     uint32_t id, const char *interface, uint32_t version) {
	if (strcmp(interface, "wl_seat") == 0) {
		printf("Đã tìm thấy wl_seat! (ID: %d)\n", id);
		my_seat = wl_registry_bind(registry, id, &wl_seat_interface, version);
	}
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
	// Không làm gì cả
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remover,
};

int main() {
	struct wl_display *display = wl_display_connect(NULL);
	if (!display) {
		printf("Lỗi: Không thể kết nối tới Wayland Compositor!\n");
		return -1;
	}
	printf("Connected wayland\n");

	struct wl_registry *registry = wl_display_get_registry(display);

	wl_registry_add_listener(registry, &registry_listener, NULL);

	wl_display_roundtrip(display);

	wl_registry_destroy(registry);
	wl_display_disconnect(display);

	return 0;
}