
runname = sorer

compile:
	g++ -g -Wall -std=c++11 -o $(runname) main.cpp
	
run: compile
	./$(runname) -f test.txt
	
docker: compile
	
clean:
	rm $(runname)
	
test: compile
	bash test.sh ./$(runname)