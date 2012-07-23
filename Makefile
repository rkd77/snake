snake:	snake.cpp
	g++ -fno-rtti -fno-exceptions -o snake snake.cpp `pkg-config --cflags --libs fox17`

