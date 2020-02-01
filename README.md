/*test machine: CSELAB_KH-1250-16
* date: 10/08/2019
* name: Ansh Sikka
* x500: sikka008
*/

# MakefileRunner
Custom implementation of a program that runs and executes data from Makefiles

## Purpose
The purpose of this software is to mimic _make_, which builds executables off of given rules/paths from a dependency tree. It shall behave the same way as _make_ and should be able to print out makefile details such as 
* Targets
* Dependencies for each target
* Recipes for each target

## How to compile
There are 3 main functions for this program: printing all details (no specified order), printing recipes (in execution build order), and building executables from the instructions in the makefile (given specific ttarget or no target).

Before executing all recipes, do this: 
`make clean`
`make`

Printing all details:
`./mymake -p <INSERT_MAKEFILE_NAME_HERE>`

Printing recipes (in execution build order)
`./mymake -r <INSERT_MAKEFILE_NAME_HERE>`

Executing with no specific target
`./mymake <INSERT_MAKEFILE_NAME_HERE>`

Executing with a specific target:
`./mymake <INSERT_MAKEFILE_NAME_HERE> <INSERT_TARGET_NAME_HERE>`

## Assumptions
* Can use strlen function from standard c library
* Can utilize makeargv() from textbook as given
* Can create an array of child processes (process fan under parent)
* No heap variables need to be used

## Team Names and x500 + Contributions
Ansh Sikka -sikka008

I did have a partner: Jacob Isler: isle0011
We did work on the project individually. However we did use each other to
bounce off each others ideas on how to move forward with the product. There 
was little to no overlap of code. The general strategies were similar between the projects.

## Extra Credit
Extra credit was attempted and appeared to be successful when tested on Makefile4 when I added a recipe with a comma.

 
