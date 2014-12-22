//
//  langford_solver.h
//  langford_pairing
//
//  Created by Gerloff, Christian on 13.11.14.
//  Copyright (c) 2014 Gerloff, Christian. All rights reserved.
//

#ifndef __langford_pairing__langford_solver__
#define __langford_pairing__langford_solver__

#include <stdio.h>
#include "tbb/tbb.h"
#include "mpi.h"
using namespace tbb;

class LangfordSolver : public task {
public:
    
    LangfordSolver(unsigned long field, char k, char length);
    
    task* execute();
    
    static unsigned long getSolutionCounter();
    
private:
    // --Globale Variable zum Zählen der Lösungen
    static atomic<unsigned long> solution_counter;
    
    unsigned long field;
    char k;
    char length;
};

#endif /* defined(__langford_pairing__langford_solver__) */
