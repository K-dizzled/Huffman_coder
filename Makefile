CFLAGS:=-std=c++17 -Wall -Werror -Wextra

all: obj obj/TreeNode.o obj/HuffTree.o obj/huffman.o obj/main.o hw_02
obj:
	if [ ! -d "obj" ]; then mkdir "obj"; fi
obj/TreeNode.o: src/TreeNode.cpp src/TreeNode.h
	g++ -c $(CFLAGS) src/TreeNode.cpp -o obj/TreeNode.o
obj/HuffTree.o: src/HuffTree.cpp src/HuffTree.h src/TreeNode.cpp src/TreeNode.h
	g++ -c $(CFLAGS) src/HuffTree.cpp -o obj/HuffTree.o
obj/huffman.o: src/huffman.cpp src/huffman.h src/HuffTree.cpp src/HuffTree.h
	g++ -c $(CFLAGS) src/huffman.cpp -o obj/huffman.o
obj/main.o: src/huffman.cpp src/huffman.h src/main.cpp
	g++ -c $(CFLAGS) src/main.cpp -o obj/main.o
hw_02: obj obj/huffman.o obj/main.o
	g++ obj/TreeNode.o obj/HuffTree.o obj/huffman.o obj/main.o -o hw_02
obj/huffman_test.o: src/huffman.cpp test/huffman_test.cpp
	g++ -c $(CFLAGS) test/huffman_test.cpp -o obj/huffman_test.o
obj/test.o: test/doctest.h test/test.cpp test/huffman_test.cpp src/huffman.cpp
	g++ -c $(CFLAGS) test/test.cpp -o obj/test.o
test: obj obj/TreeNode.o obj/HuffTree.o obj/huffman.o obj/huffman_test.o obj/test.o
	g++ obj/TreeNode.o obj/HuffTree.o obj/huffman.o obj/huffman_test.o obj/test.o -o hw_02_test
clean:
	rm obj/*
	if [ -f "hw_02" ]; then rm "hw_02"; fi
	if [ -f "hw_02_test" ]; then rm "hw_02_test"; fi
	rmdir obj