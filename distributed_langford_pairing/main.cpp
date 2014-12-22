//
//  main.cpp
//  langford_pairing
//
//  Created by Gerloff, Christian on 13.11.14.
//  Copyright (c) 2014 Gerloff, Christian. All rights reserved.
//

#include <iostream>
#include "tbb/tbb.h"
#include "langford_solver.h"
#include "mpi.h"
using namespace tbb;
using namespace std;

int solveLangford(unsigned long field, char k, char length) {
    int count = 0;
    if (2*k > length) {
        return 1;
    }
    else {
        size_t possibilities = length - k - 1;
        unsigned int mask = (1 << (k+1)) | 1;
        
        // Print out binary
        //std::bitset<8> bmask(mask);
        //std::cout << bmask << std::endl;
        for (size_t i = 0; i < possibilities; i++) {
            if ((field & mask) == 0) {
                
                count += solveLangford(field | mask, k+1, length);
                
            }
            mask <<= 1;
        }
        return count;
    }
}

int main(int argc, const char * argv[]) {
    
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    // Print off a hello world message
    printf("Hello world from processor %s, rank %d"
           " out of %d processors\n",
           processor_name, world_rank, world_size);
    
    
    /* master */
    if (world_rank == 0) {
        printf("Hello from master");
        // Print off a hello world message
        // Size of the langford pairing problem 4m & 4m-1 [3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 23, 24,...]
        char n = 11;
        char k = 1;
        char length = 2*n;
        unsigned long field = 0;
        int count = 0;
        int solutions = 0;
        int solutions_counter = 0;
        
        int processor_count = 1;
        
        size_t possibilities = (length - k - 1)/2;
        unsigned int mask = (1 << (k+1)) | 1;
        
        // Print out binary
        //std::bitset<8> bmask(mask);
        //std::cout << bmask << std::endl;
        for (size_t i = 0; i < possibilities; i++) {
            if ((field & mask) == 0) {
                
                printf("Sende Feld...\n");
                
                MPI_Send(&field, 1, MPI_UNSIGNED_LONG, processor_count, 0, MPI_COMM_WORLD);
                MPI_Send(&mask, 1, MPI_UNSIGNED_LONG, processor_count, 1, MPI_COMM_WORLD);
                MPI_Send(&k, 1, MPI_CHAR, processor_count, 2, MPI_COMM_WORLD);
                MPI_Send(&length, 1, MPI_CHAR, processor_count, 3, MPI_COMM_WORLD);
                
                solutions++;
                if (processor_count == world_size - 1) {
                    processor_count = 1;
                } else {
                    processor_count++;
                }
                
            }
            mask <<= 1;
        }
        
        printf("Solutions: %d\n", solutions);

        do {
            int slave_count = 0;
            MPI_Status status;
            MPI_Recv(&slave_count, 1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &status);
            count += slave_count;
            solutions_counter++;
            printf("Solution Counter: %d\n", solutions_counter);
        } while (solutions_counter < solutions);
        
        
        //tick_count t0 = tick_count::now();




        //tick_count t1 = tick_count::now();
        
        //printf("Zeit für Algorithmus: %g seconds\n", (t1-t0).seconds());
        
        cout << "Lösungen: " << count << endl;
        
        // Finalize the MPI environment.
        MPI_Finalize();
    }
    
    /* client */
    if (world_rank != 0) {
        printf("Hello from slave\n");
        char k, length;
        unsigned long field;
        unsigned int mask;
        int count = 0;
        MPI_Status status;
        
        MPI_Recv(&field, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&mask, 1, MPI_UNSIGNED_LONG, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&k, 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&length, 1, MPI_CHAR, 0, 3, MPI_COMM_WORLD, &status);
        
        count = solveLangford(field | mask, k+1, length);
        printf("Lösungen: %d\n", count);
        
        MPI_Send(&count, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
        
        
        // Finalize the MPI environment.
        MPI_Finalize();
    }
}
