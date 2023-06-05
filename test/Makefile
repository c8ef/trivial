toolchain:
	wget https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf.tar.xz
	tar xf gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf.tar.xz
	mv gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf aarch32-gcc
	rm gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf.tar.xz

build:
	mkdir -p aarch32
	./aarch32-gcc/bin/arm-none-linux-gnueabihf-gcc sysy.c -c -o aarch32/sysy.o
	ar rcs aarch32/libsysy.a aarch32/sysy.o
	rm aarch32/sysy.o

clean:
	rm -rf aarch32 aarch32-gcc gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf.tar.xz
