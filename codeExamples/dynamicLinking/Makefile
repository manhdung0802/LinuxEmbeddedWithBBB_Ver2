
build: # build object chương trình chính
	mkdir -p ./output
	gcc -c dynamicLinking.c 

clean:
	rm -f ./output/*
	rm -f *.o *.so

build_lib: # buid thư viện động
# build ra file obj của thư viện
	gcc -c -fPIC libsum.c -o output/libsum.o
# build obj của thư viện thành dạng thư viện động .so
	gcc -shared -o output/libsum.so output/libsum.o

build_app:
# build app, link file .so vào chương trình chính
# -lsum: mention tới thư viện động -> chú ý: trong linux,rule đặt tên của thư viện động cần có lib ở đầu
	gcc -o main_dynamic dynamicLinking.o -L./output -lsum -Wl,-rpath='$$ORIGIN/output'