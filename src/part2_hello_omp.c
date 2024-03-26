/*Part 2: Adding OpenMP threading to a simple MPI application
Take a look at the Hello World applications that we have used in past assignments that include basic MPI functionality. 
Modify one of these applications to include OpenMP. Wrap the print statements in an omp parallel region.
Make sure to modify the MPI_Init call accordingly to allow for threads! What level of thread support do you need?

Run the code using 2 MPI ranks and 4 OpenMP threads per rank. 
To do this, prior to executing the run command, set the number of threads environment variable as 
> export OMP_NUM_THREADS=4. 
Then you can simply execute the application with the mpiexec command: 
> mpiexec -n 2 ./a.out.
Explain the output.
*/
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) 
{
    int numtasks, rank, len, provided;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    // Before MPI
    printf("Before 'MPI_Init': Hello, world.\n");

    // Initialize the MPI environment with thread support
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    /*In this simple hello application, MPI_THREAD_SINGLE suffice because 
     the parallel region only performs computations or outputs "hello world" messages without 
     involving MPI communication inside the parallel region.
     We can use MPI_THREAD_MULTIPLE, which indicates that the program intends to make MPI calls from multiple threads 
     concurrently, which is the highest level of thread support in MPI.*/

    // Print the provided thread support level and MPI_THREAD_MULTIPLE value
    printf("Actual level of thread support provided by MPI implementation is %d\n", provided);
    printf("MPI_THREAD_SINGLE is %d\n", MPI_THREAD_SINGLE);

    // Check for the level of MPI thread support
    if (provided < MPI_THREAD_SINGLE){
        printf("The MPI implementation does not have full thread support. \n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    // retrieves the total number of processes in the communicator.
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); 
    // get the rank, the unique identifier assigned to each process in the communicator, starting from 0 to numtasks-1.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    // retrieves the name of the processor on which the calling process is running.
    MPI_Get_processor_name(hostname, &len);

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();       // get threadID
        int total_threads = omp_get_num_threads();  // get the number of total threads

        // Between MPI
        // each thread executes this
        printf("Between 'MPI': Hello, World ~ from rank %d, thread %d/%d, running on host %s\n",
                                                    rank, thread_id, total_threads, hostname);
    }

    // done with MPI
    MPI_Finalize();

    //After MPI
    printf("After 'MPI_Finalize': Hello, World!!!\n");
    return 0;
}

/* 
Execution instructions:
$ mpicc -fopenmp part2_hello_omp.c -o hello_omp
$ export OMP_NUM_THREADS=4
$ mpiexec -n 2 ./hello_omp 
*/