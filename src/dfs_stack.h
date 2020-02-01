#ifndef __DFS_STACK_H
#define __DFS_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

//Implement a stack for DFS


/* push to stack */
void push(target dependency, target stack[]){
    int i = 0;
    while(stack[i].name != NULL) {
        i++;
    }
    stack[i] = dependency;
}

/* standard dfs that finds the deepest dependencies */
void DFS_RECURSIVE(target targets[], target startingTarget, target stack[]) {
    startingTarget.visited = 1; // visited current target
    push(startingTarget, stack);
    
    for (int dependencyIterator = 0; dependencyIterator < startingTarget.dep_count - '0'; dependencyIterator++){
        char* target_name = startingTarget.depend[dependencyIterator];
        int finder = 0;
        while(targets[finder].name != NULL){
            if (strncmp(targets[finder].name, target_name, MAX_PARM) == 0){
                if(targets[finder].visited == 0){
                    DFS_RECURSIVE(targets, targets[finder], stack); // go to deepest target that hasn't been visited
                }
            }
            finder++;
        }
        
        

        
    }
    

}
#endif
