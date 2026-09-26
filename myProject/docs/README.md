# Ý tưởng
- Kết nối BeagleBone Black với màn hình HDMI, nút nhấn và LED.
- Tầng Userspace chạy ứng dụng Flutter đóng vai trò là **Wayland Client** giao tiếp qua **Wayland Compositor (Weston)** để hiển thị lên màn hình.
- Nút nhấn và LED được điều khiển thông qua Driver/Device Nodes ở tầng Kernel.

## Kiến trúc Hệ thống (System Architecture)
- `https://app.diagrams.net/#G1sM-bo-QgmpcibdnVVgfZikiQOy0sVZ-v#%7B%22pageId%22%3A%22t71Wivp8T_X_GTTvBR0P%22%7D`