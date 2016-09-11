files = Interpreter.cpp Input.cpp ParseTree.cpp main.cpp
flags = -std=c++11 -pedantic-errors
output = blip

blip:
	g++ $(files) $(flags) -O3 -o $(output)

clean:
	rm $(output)
