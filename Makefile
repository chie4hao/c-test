all: cpptest

cpptest: cpptest.o file_buffer.o frame_util.o pseudorandom_sequence.o
#clang++ -Xclang -flto-visibility-public-std -o cpptest.exe -O3 cpptest.cc -O3 file_buffer.cc -O3 frame_util.cc -O3 pseudorandom_sequence.cc
	clang++ -o cpptest.exe -Wall -g -O3 -static-libgcc -fcolor-diagnostics --target=x86_64-w64-mingw -std=c++17 cpptest.cc file_buffer.cc frame_util.cc pseudorandom_sequence.cc
#clang++ $fileName -o $fileNameWithoutExt.exe -Wall -g -Og -static-libgcc -fcolor-diagnostics --target=x86_64-w64-mingw -std=c++17 && $dir$fileNameWithoutExt",

cpptest.o: cpptest.cc
	clang++ -c -O3 cpptest.cc
file_buffer.o: file_buffer.cc
	clang++ -c -O3 file_buffer.cc
frame_util.o: frame_util.cc
	clang++ -c -O3 frame_util.cc
pseudorandom_sequence.o: pseudorandom_sequence.cc
	clang++ -c -O3 pseudorandom_sequence.cc
.PHONY: clean
clean:
	del cpptest.exe
	del *.o