# <p style="text-align: center;"> CMSE 822 &nbsp;&nbsp;&nbsp; SS24
## <p style="text-align: center;">  Project 4 OpenMP and Hybrid Parallelism
<p style="text-align: right; font-size: 18px;">  &#9786; Team 1  </p>
<p style="text-align: right; font-size: 18px;">  Benjamin DalFavero, Reza Khan Mohammadi, Tairan Song, Xiaotian Hua, Ziyu Cheng</p>  


## Part 1 : OpenMP Matrix-Matrix Multiplication

Q 2. As shown in the figure below, when we run the MMM program on `amd-20` using `export OMP_NUM_THREADS=1`, the time consumed is almost the same as that for serial computing of MMM. When the matrix size is very small, the error is more noticeable, but still less than 3%. The reason for the minor discrepancies is that running OMP code also requires time, but as the scale of computation increases, the proportion of this time becomes smaller and smaller.
<img src="./part1_1.png" alt="error" width="800"/>

Q 3. In `AMD-20`, using the `nproc` command to query the maximum number of processes is `128`, so T=128.

## Part 2 : Adding OpenMP threading to a simple MPI application



## Part 3: Hybrid Parallel Matrix Multiplication

