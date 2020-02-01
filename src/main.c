#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "graph.h"
#include "dfs_stack.h"

/* Referenced from textbook */
/* Splits string into tokens based on delimiter */
int makeargv(const char *s, const char *delimiters, char ***argvp)
{
	int error;
	int i;
	int numtokens;
	const char *snew;
	char *t;

	if ((s == NULL) || (delimiters == NULL) || (argvp == NULL))
	{
		errno = EINVAL;
		return -1;
	}
	*argvp = NULL;
	snew = s + strspn(s, delimiters); /* snew is real start of string */
	if ((t = malloc(strlen(snew) + 1)) == NULL)
		return -1;
	strcpy(t, snew);
	numtokens = 0;
	if (strtok(t, delimiters) != NULL) /* count the number of tokens in s */
		for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++);

	/* create argument array for ptrs to the tokens */
	if ((*argvp = malloc((numtokens + 1) * sizeof(char *))) == NULL)
	{
		error = errno;
		free(t);
		errno = error;
		return -1;
	}
	/* insert pointers to tokens into the argument array */
	if (numtokens == 0)
		free(t);
	else
	{
		strcpy(t, snew);
		**argvp = strtok(t, delimiters);
		for (i = 1; i < numtokens; i++)
			*((*argvp) + i) = strtok(NULL, delimiters);
	}
	*((*argvp) + numtokens) = NULL; /* put in final NULL pointer */
	return numtokens;
}

/* Remove newline character if found */
void remove_newline_character (char * token){
	int len = strlen(token) - 1;
	if(token[len] == '\n'){
		token[len] = '\0';
	}
}

/* Remove trailing space if found */
void remove_trailing_space (char * token) {
	int len = strlen(token);
	if(token[0] == ' '){
		for (int i = 1; i < len; i++){
			token[i-1] = token[i];
		}
	}
}

/* Parse the input makefile to determine targets, dependencies, and recipes */
int process_file(char *fname, target targets[])
{
	FILE *fp = fopen(fname, "r"); // open the file (with fname) with read permissions
	char line[LINE_SIZE];		  // create an array of characters with the size of each line
	int i = 0;
	char **temp;
	char **tempRecipe;

	// exception flow
	if (!fp)
	{
		printf("Failed to open the file: %s \n", fname);
		return -1;
	}

	//Read the contents and store in lines
	while (fgets(line, LINE_SIZE, fp))
		strncpy(lines[i++], line, strlen(line));

	int numtoksTarget;
	int targetIterator = 0; // go through target struct array
	int lineIterator;		// go line by line
	int recipes = 0;

	for (lineIterator = 0; lineIterator < i; lineIterator++)
	{
		numtoksTarget = makeargv(lines[lineIterator], ": ", &temp);

		if (numtoksTarget > 1)
		{ // as long as it isn't a blank line 
			if (temp[0][0] == '\t')
			{ // check for a recipe
				int recipeToks;
				recipeToks = makeargv(lines[lineIterator], "\t,", &tempRecipe);
				for (int r = 0; r < recipeToks; r++){
					remove_newline_character(tempRecipe[r]);
					remove_trailing_space(tempRecipe[r]);
					targets[targetIterator - 1].recipe[recipes] = tempRecipe[r];
					recipes++;
				}
				
				targets[targetIterator - 1].recipe_count = recipes + '0'; // convert number of recipes to char
				continue;
			}
			else
			{
				recipes = 0; // haven't found a recipe (yet) for this iteration, at a target and dependency line
				int dependencies = 0;
				targets[targetIterator].name = temp[0]; // store name
				targets[targetIterator].visited = 0;
				for (int d = 1; d < numtoksTarget; d++)
				{
					if (strcmp("\n", temp[d]) == 0)
					{ // check if there are no dependencies
						break;
					}
					else
					{
						remove_newline_character(temp[d]);													 // add dependency starting from 2nd token (1st token is name of target)
						targets[targetIterator].depend[d - 1] = temp[d]; // store dependencies
						dependencies++;
					}
				} 
				targets[targetIterator].dep_count = dependencies + '0'; // store the number of dependencies in a char
				targetIterator++;
				targets[targetIterator - 1].recipe_count = recipes + '0'; // no recipes found (yet), set char to read 0
			}
		}
	}

	fclose(fp);

	return 0;
}

// print dependencies for -p flag
int print_p(target targets[]) {
	int count = 0;
	while (targets[count].name != NULL) // go through targets until the end (when theres no more targets)
	{
		printf("target `%s` has %c dependencies and %c recipe(s)\n", targets[count].name, targets[count].dep_count, targets[count].recipe_count); // print name, dependency and recipe count
		for (int k = 0; k < targets[count].dep_count - '0'; k++)
		{ // list each dependency
			char *dependency = targets[count].depend[k];
			printf("Dependency %d is %s\n", k, dependency);
		}
		for (int r = 0; r < targets[count].recipe_count - '0'; r++)
		{ // list each recipe
			char *recipe = targets[count].recipe[r];
			printf("Recipe %d is %s\n", r, recipe);
		}
		printf("\n");
		count++;
	}
	return 0;
}

int process_stack(target targets[], target stack[], int is_specified, char *target_name) {
	int completed = 0;
	if (is_specified) { // custom target: find it and recurse dependencies
		int i = 0;
		while(targets[i].name != NULL){
			if (strcmp(targets[i].name, target_name) == 0){
				DFS_RECURSIVE(targets, targets[i], stack);
				completed = 1;
			}
			i++;
		}

	} else { // makefile without specific target
		DFS_RECURSIVE(targets, targets[0], stack); 
		completed = 1;
	}

	if (completed) { // successful execution
		return 0;
	} else {
		perror("The target was not found!");
		_exit(1);
	}

}

int print_r(target stack[]) {
	int i = 0;
	while(stack[i].name != NULL) {
        i++; // count elements in stack
    }
	
	for (int k = i; k >= 0; k--){ // print recipes in order of stack pop
		target myTarget = stack[k];
		for (int r = 0; r < myTarget.recipe_count - '0'; r++){
			char * recipe = myTarget.recipe[r];
			printf("%s\n", recipe);
		}
	}
		
	return 0;
}

int execute(target stack[], int specific){
	int stack_count = 0;
	while(stack[stack_count].name != NULL) {
		stack_count++;
	} // count items in stack

	for (int targetNum = stack_count-1; targetNum >= 0; targetNum--) {
		target myTarget = stack[targetNum];
		pid_t child_pids[myTarget.recipe_count - '0']; // create an array

		/* create a child process for each recipe (process fan) */
		for(int r = 0; r < myTarget.recipe_count - '0'; r++){ 
			child_pids[r] = fork();
			if(child_pids[r] == 0) {
				char * recipe = myTarget.recipe[r];
				if(specific){ // print if specific target
					printf("Executing recipe %s\n", recipe);
				}
				char **recipeTokens;
				int recipe_toks_count = makeargv(recipe, " ", &recipeTokens); // get arguments from recipe
				char * main_command = recipeTokens[0];
				for (int i = 1; i < recipe_toks_count; i++){
					recipeTokens[i-1] = recipeTokens[i]; // set arguments

				}
				recipeTokens[recipe_toks_count] = NULL; // set last argument to NULL as per exec rules
				execvp(main_command, recipeTokens);
				_exit(1); // failure

			}
			else { // parent process
				wait(NULL); // wait for children
			}
			
		}
		
	}
	return 0;


}




//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[])
{
	target targets[MAX_TARGETS];
	target stack[MAX_TARGETS];
	//$./mymake Makefile
	//Similarly account for -r flag
	if ((argc == 2 && strncmp(argv[1], "-p", 2)) || (argc==2 && strncmp(argv[1], "-r", 2)))
	{ // Just do the make
		process_file(argv[1], targets); // store in struct array
		process_stack(targets, stack, 0, NULL); // organize recipes by doing DFS
		execute(stack, 0); // fork and exec
	}


	if (argc == 3)
	{
		// $./mymake Makefile target [specific]
		if (strncmp(argv[1], "-p", 2) && strncmp(argv[1], "-r", 2)){
			if(!process_file(argv[1], targets)) { // store in struct array
				process_stack(targets, stack, 1, argv[2]); // organize recipes by doing DFS
				execute(stack, 1); // fork and exec
			} 
			
		}
		else if (!strncmp(argv[1], "-p", 2)) // if it equals p
		{
			// $./mymake -p Makefile
			if (!process_file(argv[2], targets))
			{
				print_p(targets); // print all details
			}
		}

		else if (!strncmp(argv[1], "-r", 2)) // if it equals r
		{
			// $./mymake -r Makefile
			if(!process_file(argv[2], targets)){ // store in struct array
				process_stack(targets, stack, 0, NULL); // organize recipes by doing DFS
				print_r(stack); // print recipes in executive order
			}

		}
		
		else if (strncmp(argv[2], "-r", 2) || strncmp(argv[2], "-p", 2)) {
			perror("Incorrect argument order\n");
			_exit(1);
		}
	} else if (argc > 3 || argc < 2){
		perror("Incorrect number of arguments!\n");
		_exit(1);
	}

	exit(EXIT_SUCCESS);
}