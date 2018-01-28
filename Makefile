cppt: cpptest.o file_buffer.o frame_util.o pseudorandom_sequence.cpp
	g++ -o test cpptest.cpp file_buffer.cpp frame_util.cpp pseudorandom_sequence.cpp
cpptest.o: cpptest.cpp
	g++ -c cpptest.cpp
file_buffer.o: file_buffer.cpp
	g++ -c file_buffer.cpp
frame_util.o: frame_util.cpp
	g++ -c frame_util.cpp
pseudorandom_sequence.cpp: pseudorandom_sequence.cpp
	g++ -c pseudorandom_sequence.cpp
.PHONY: clean
clean:
	del cppt.exe
	del *.o