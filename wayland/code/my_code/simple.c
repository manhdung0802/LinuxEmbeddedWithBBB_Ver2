#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/util/log.h>

struct my_server {
	struct wl_display *display;
	struct wlr_renderer *renderer;
	struct wlr_allocator *allocator;
	struct wlr_backend *backend;
};

int main() {
	wlr_log_init(WLR_DEBUG, NULL);
	wlr_log(WLR_INFO, "Start my wayland");

	struct my_server my_server = {0};
	my_server.display = wl_display_create();

	if (!my_server.display) {
		wlr_log(WLR_DEBUG, "Cannot create a display");
	}

	return 0;
}