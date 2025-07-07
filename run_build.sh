#!/bin/sh

if [ ! -d ./build ]; then
	mkdir build && cd build
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
	cd ..
fi

if [ $? -eq 0 ]; then
	cd build && make -j16 && cd .. && ./build/main
fi
