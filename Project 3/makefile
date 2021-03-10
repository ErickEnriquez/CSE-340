default : lexer.cc inputbuf.cc defn.cpp
	g++ -g -std=c++11 -o a.out inputbuf.cc lexer.cc defn.cpp

run : a.out
	./a.out <./tests/test00.txt

gitSave :
	git add .
	git commit -m "placeholder text"
	git push origin master

windows:
	g++ -o a.exe Main.cpp inputbuf.cc lexer.cc




