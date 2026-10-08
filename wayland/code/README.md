# thiết lập folder chứa lib, header của wlroots
`meson setup build --wipe --prefix=/home/as/Desktop/LinuxEmbeddedWithBBB_Ver2/wayland/code/wlroots_src/wlroots-0.20.2 --libdir=lib --buildtype=debugoptimized -Dwerror=false`

# setup môi trường mới cho project mới dùng file .ini
`meson setup build --native-file path.ini`

# lệnh debug
`gdb -batch -ex run -ex bt --args ./build/simple`

# trình tự code
- simple.c - done
- scene-graph.c
    + `./build/my-scene-graph -s "weston-terminal --shell=/home/as/Desktop/LinuxEmbeddedWithBBB_Ver2/wayland/code/my_code/anim"`
- tinywl.c
- pointer.c
- output-layout.c
- rotation.c
- cairo-buffer.c
- touch.c
- tablet.c
- fullscreen-shell.c
- output-layers.c
- embedded.c