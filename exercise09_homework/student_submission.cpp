//
// Created by Vincent Bode on 23/06/2020.
//

#include <cstddef>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include "Utility.h"
#include "StringSearch.h"
#include "mpi.h"

int longestCommonSubsequence(const unsigned char* str1, const unsigned char* str2, size_t len);

// Routine where we match our input sequence across the entire document and return the number of matches.
// If you're just splitting the work by dividing the queries, then you don't need to touch this function.
int count_occurrences(const unsigned char *searchString, std::size_t searchStringSize,
                      const unsigned char *document, std::size_t documentSize) {
    int occurrences = 0;
    // Search from every possible start string position and determine whether there is a match.
    for (std::size_t startIndex = 0; startIndex < documentSize - searchStringSize; ++startIndex) {
        // We consider 2 sequences to match if the longest common subsequence contains >= 70% the number of characters
        // of the query. In that case, they are close enough so that we count them as the same.
        if (longestCommonSubsequence(searchString, document + startIndex, searchStringSize) >= 0.7 * searchStringSize) {
            occurrences++;
        }
    }
    return occurrences;
}

int main(int argc, char **argv) {
    

    // TODO@Students: Implement the steps from the slides in here.
    MPI_Init (&argc, &argv);
    int rank, size;
    MPI_Comm_rank ( MPI_COMM_WORLD , &rank); 
    MPI_Comm_size ( MPI_COMM_WORLD , &size);

    unsigned char* document = (unsigned char* ) malloc ( sizeof(unsigned char) * DOCUMENT_SIZE);
    Utility::readEncyclopedia(document);

    
    unsigned char *query = (unsigned char* ) malloc ( sizeof(unsigned char ) * MAX_QUERY_LENGTH * NUM_QUERIES);
    unsigned char *query_local =  (unsigned char* ) malloc ( sizeof(unsigned char ) * MAX_QUERY_LENGTH);
    int *global_occurrences = (int *) malloc ( sizeof( int ) * NUM_QUERIES);
    int *temp = (int *) malloc ( sizeof( int ));
    //int* global_occurrences = (int* ) malloc ( sizeof( int ) * NUM_QUERIES);
    if (rank == 0){
        // This loads the text and the query strings into memory. You can only call this from process 0 w.r.t MPI_COMM_WORLD
        // all other processes will hang if you call this method
        Utility::generateProblemFromInput(document);
        // Utility::getQuery can be used to get a single query (this also only works on process 0). Also take a look at
        // Utility::getQueryBuffer which allows you to access the entire buffer as one.
        query = Utility::getQueryBuffer();        
    }    
    MPI_Scatter(query, MAX_QUERY_LENGTH, MPI_UNSIGNED_CHAR, query_local, MAX_QUERY_LENGTH, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    // Utility::getQueryLength works from all processes
    std::size_t queryLength = Utility::getQueryLength(rank);     
    
    
    // This is where we do the actual work of going through the encyclopedia and counting the matches.
    int occurrences = count_occurrences(query_local, queryLength, document, DOCUMENT_SIZE);
    // Gather all occurrences and query length from all processers
    MPI_Gather(&occurrences, 1, MPI_INT, global_occurrences, 1, MPI_INT, 0, MPI_COMM_WORLD);  
    
    // Afterwards, we output the number of matches. Note that the order needs to be maintained in the parallel
    // version.  
    if (rank == 0){
        for (int i = 0; i < NUM_QUERIES; i++){
        queryLength = Utility::getQueryLength(i);
        printf("Query %i: %i occurrences (query length %zu).\n", i, global_occurrences[i], queryLength);
        }
    }    
    
    MPI_Finalize();
    std::cout << "DONE" << std::endl;
    return 0;
}

int cache[MAX_QUERY_LENGTH + 1][MAX_QUERY_LENGTH + 1];
// We use the longest common subsequence as our metric for string similarity. Both buffers must have at least length len.
// Note that this is different from the longest common substring. If you just want to parallelize, you do not need to
// touch this implementation.
// https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
int longestCommonSubsequence(const unsigned char* str1, const unsigned char* str2, size_t len) {
    for (unsigned int i = 0; i <= len; ++i) {
        cache[i][0] = 0;
        cache[0][i] = 0;
    }
    for(unsigned int i = 1; i <= len; i++) {
        for(unsigned int j = 1; j <= len; j++)
            if(str1[i - 1] == str2[j - 1]) {
                cache[i][j] = cache[i - 1][j - 1] + 1;
            } else {
                cache[i][j] = std::max(cache[i][j - 1], cache[i - 1][j]);
            }
    }
    return cache[len][len];
}
