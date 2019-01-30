//rememeber that you can't access private memebers of a class outside of that class for example you can't create an object 
of the Class A and try to access its private members within main you cant, thats  the whole point of getters and setters 
to access the information indirectly

useful git commands

git add . //saves all additions we added into the code
git commit -m "message goes here" //makes a commit to git and then you can add in a message
git push origin master //pushes the master branch of origin remote to gitlab
git pull //pulls ant changes into the workstation


redirecting input

./p1 <./tests/t1_ss_outputstmt.txt // example



//debugging/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//first in the terminal type 
g++ -g inputbuff.cc lexer.cc parser.cc -o p1 // to create the executable p1 that we will need to run in the gdb debugger
gdb p1 // to start the gdb debugger with p1 
run // to let the file run in the terminal

//now in VS code press ctrl+shift+D to shift to the debugger window
click the launch button in the top left corner , make sure that you add breakpoints if you need
code should run and stop at the breakpoints
//if you want to change what gets redirected to the program go into the JSON file and give it the file you want to use 

//put this in the args section of the json file to redirect stdin
"${workspaceFolder}<./tests/t1_ss_outputstmt.txt"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


usefyl shortcuts in vscode
ctrl+k ctrl+0 //to collapse all th functions
ctrl+k ctrl+j // to expand all
