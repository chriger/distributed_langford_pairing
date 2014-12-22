//
//  langford_solver.cpp
//  langford_pairing
//
//  Created by Gerloff, Christian on 13.11.14.
//  Copyright (c) 2014 Gerloff, Christian. All rights reserved.
//

#include "langford_solver.h"
#include <iostream>


atomic<unsigned long> LangfordSolver::solution_counter = 0;

LangfordSolver::LangfordSolver(unsigned long field, char k, char length): field(field), k(k), length(length){
    
    
}

task* LangfordSolver::execute() {
    if (2*k > length) {
        int solution = 1;
        MPI_Send(&solution, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Lösung gefunden!");
    }
    else {
        size_t possibilities = (k==1) ? (length - k - 1)/2 : length - k - 1;
        task_list list;
        unsigned int mask = (1 << (k+1)) | 1;
        
        // Print out binary
        //std::bitset<8> bmask(mask);
        //std::cout << bmask << std::endl;
        int count = 0;
        for (size_t i = 0; i < possibilities; i++) {
            if ((field & mask) == 0) {
                
                list.push_back(*new(allocate_child())
                        LangfordSolver(field | mask, k+1, length));
                count+=1;
                
            }
            mask <<= 1;
        }
        set_ref_count(count+1);
        spawn_and_wait_for_all(list);
    }
    
    return 0;
}

unsigned long LangfordSolver::getSolutionCounter() {
    return solution_counter;
}
