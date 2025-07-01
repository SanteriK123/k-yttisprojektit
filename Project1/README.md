# Project 1: Warmup to C and Unix programming
In this project, I implemented the reverse program that reverses all the rows of an input file, and this includes all the features with all the error checking and also even making sure that the input and output aren't the same files (symlink). This would ideally score the maximum of 5 points.

# Usage

To compile and run `reverse.c` use the following commands. As described in the assignment, there are 3 different ways use the program.  
```
gcc -Wall -Werror -o reverse reverse.c
./reverse
./reverse input.txt
./reverse input.txt output.txt
```
The first option takes input from the stdin, and then reverses it into the stdout. Just type a message, and then use CTRL + D to signal the end of the input.   
The second option takes the input from a file, and then outputs into the stdout.  
In the third option, the program takes input from a file and outputs to a file.  
  
Example of usage:
![Alt text](./testreverse.png?raw=true "Optional Title")
# Documentation and code explanation
The code is fairly thoroughly documented, so this will be mostly a quick explanation on how everything works.  
  
To handle the different arguments, the code has both an input and output variable of type FILE that gets dynamically changed based on the arguments. If no arguments are given, they retain their default state of stdin and stdout. If 1 argument is given, so the input file, it attempts to read it using fopen and it is assigned to the input variable. If 2 arguments are given, it first goes through the first if check, and then the second if check where the output variable is changed to the given file name. This includes extensive error checking and all the error messages and considerations, so for example if 2 files are the same.  
  
The actual program implementation is very simple. I created a linked list, where each element is 1 row of the input, and to reverse it I simply add all new elements to the beginning of the linked list rather than to the end, so when the linked list is read in the usual order so from start to end, the elements are listed out in reverse. This implementation also includes dynamic memory allocation and freeing of it.  
  
Error checking is done as mentioned in the assignment, so `fprintf(stderr, "something\ŋ")` All the considerations are taken into account.

# AI declaration

No AI generated code was used, but AI was used mainly to help with the concepts regarding C. Any place where third-party code was used is sourced in the `reverse.c` file comments.