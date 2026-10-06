#include "types/wlr_output.h"
#include <stdio.h>
#include <wayland-server-core.h>
#include <wayland-util.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_scene.h>

struct my_server {
	struct wl_display *display;
	struct wlr_backend *backend;
	struct wlr_renderer *renderer;
	struct wlr_allocator *allocator;
	struct wl_listener *listener;
	struct wlr_output *output;
	struct wlr_scene *scene;
};

// on_new_output chạy khi có màn hình được nhận diện
void on_new_output(struct wl_listener *listener, void *data) {

	struct my_server *server = wl_container_of(listener, server, listener);

	if (server == NULL) {
		printf("Cannot find server\n");
	} else {
		server->output = data;
		// Khởi tạo màn hình này với renderer
		wlr_output_init_render(server->output, server->allocator, server->renderer);

		// Bật màn hình
		struct wlr_output_state state;
		wlr_output_state_init(&state);
		wlr_output_state_set_enabled(&state, true);
		wlr_output_commit_state(server->output, &state);
	}
}

void on_frame(struct wl_listener *listener, void *data) {
	float color[4] = {1.0, 0.0, 0.0, 1.0};

	// gắn color vào màn hình output
	struct my_server *server = wl_container_of(listener, server, listener);
	if (server == NULL) {
		printf("Cannot find server\n");
	} else {
		server->output = data;
	}
}

int main() {
	struct my_server server = {0};

	// tạo bộ não
	server.display = wl_display_create();

	// tạo backend - kết nối vào GPU
	server.backend = wlr_backend_autocreate(wl_display_get_event_loop(server.display), NULL);

	// Tạo render để vẽ
	server.renderer = wlr_renderer_autocreate(server.backend);

	server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);

	server.scene = wlr_scene_create();

	// 2. Tạo một hình chữ nhật màu Đỏ, kích thước vô cực, và treo nó lên Cây
	float red_color[4] = {1.0, 0.0, 0.0, 1.0};
	// Cú pháp: Gắn vào đâu (Nút gốc của cây), Kích thước (RxD), Màu sắc
	wlr_scene_rect_create(&server.scene->tree, 10000, 10000, red_color);

	// ----------------------------------------

	// Đăng ký sự kiện nghe màn hình
	server.listener->notify = on_new_output;
	wl_signal_add(&server.backend->events.new_output, server.listener);

	// bắt đầu kết nối vào GPU
	wlr_backend_start(server.backend);

	// chạy vòng lặp vô tận
	wl_display_run(server.display);

	wl_display_destroy(server.display);

	return 0;
}