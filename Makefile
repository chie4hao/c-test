all: cpptest

cpptest: cpptest.o file_buffer.o frame_util.o pseudorandom_sequence.o
	clang++ -Xclang -flto-visibility-public-std -o cpptest.exe -O3 cpptest.cc -O3 file_buffer.cc -O3 frame_util.cc -O3 pseudorandom_sequence.cc
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