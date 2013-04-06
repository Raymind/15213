#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    ENSURES(is_transpose(M, N, A, B));

    /*i used 11 variables in total. 8 have been declared here, 3 are in 64x64.
        purpose for each will be described later. */
    int* temp;
    int diag = 0;
    int i;
    int j;
    int b_row;
    int b_col;
    int limit_row;
    int limit_col;
    int k;
    if (M==32 && N==32)
        //in the 32x32 matrix case
    {
        for (i = 0; i < (N/8); i++)
        {
            /*i used 8x8 blocking and iterated through each of the 16 blocks
                in these two loops
                8x8 blocking helps greatly reduce the number of cache misses*/
            for(j = 0; j < (M/8); j++)
            {
                /*in the next two loops, we're iterating through the rows
                    and columns of each 8x8 block*/
                for(b_row = 8*i; b_row < (8*i + 8); b_row++)
                {
                    //store the current row of the block in a temp variable
                    temp = A[b_row];
                    /*if we're at a diagonal then store it in a variable and
                        add the diagonal element once it iterates through
                        all the columns in the particular row. note that theres
                        only one diagonal per row. diagonals don't need to be
                        transposed so we can do this. it really helps in
                        reducing cache misses :) */
                    if(i==j)
                      diag = temp[b_row];
                    for(b_col = 8*j; b_col < (8*j + 8); b_col++)
                    {
                        /*if its not a diagonal element then swap
                            current indices and store in B*/
                        if(b_row != b_col)
                            B[b_col][b_row] = temp[b_col];
                    }
                   if(i == j)
                    //put it in after we traverse the block row
                       B[b_row][b_row] = diag;
                }
            }
        }
    }
    if(M == 61 && N == 67 )
    {
        /*the case of the 61x67 matrix
            we employ 8x8 blocking */
        for(i = 0; i < N; i+=8)
        {
            for(j = 0; j < M; j+=8)
            {
                /*since the matrix is rectangular
                    we take care of the case where there's an
                    irregular number of rows and columns in
                    the block matrix. in that case the actual
                    row and column limit is the minimum of the two*/
                limit_row = ((i+8) <= N) ? (i+8) : N;
                limit_col = ((j+8) <= M) ? (j+8) : M;
                /*now we traverse through the rows and columns of
                    the 8x8 matrix */
                for(b_row = i; b_row < limit_row; b_row++)
                {
                    /*we store the block row temporarily to
                    increase cache efficiency*/
                    temp = A[b_row];
                    /*if diagonal then handle same way as 32x32 case*/
                    if(i == j)
                        diag = temp[b_row];
                    for(b_col = j; b_col < limit_col; b_col++)
                    {
                        //swap similarly as in 32x32 case
                        if(b_row != b_col)
                            B[b_col][b_row] = temp[b_col];
                    }
                    if (i == j)
                        B[b_row][b_row] = diag;
                }
            }
        }
    }

    if(M == 64 && N == 64)
    {
        //case of 64x64 matrix
    for(i = 0; i < M; i+=8)
    {
    /* this uses a 8 by 8 blocking and then uses 4 by 4 blocking in each 8 by
        8 block to be cache efficient. Further, the 4 by 4 matrices in the 8 by
        8 block are accessed using a U pattern, to further save misses in each
        iteration */

        int b4r = 0;
        int b4c = 0;
        for(j = 0; j < N; j+=8)
        {
            //iterating through the 8x8 blocks
            for(k = 0; k < 4; k++)
            {
                /*have a for loop which accesses the 4x4 sub-blocks
                    in a U-fashion*/
            switch(k)
            {
                //switch statement to assign offsets accordingly
                case 0:
                    b4r = i;
                    b4c = j;
                    break;
                case 1:
                    b4r = i+4;
                    b4c = j;
                    break;
                case 2:
                    b4r = i + 4;
                    b4c = j + 4;
                    break;
                case 3:
                    b4r = i;
                    b4c = j + 4;
                    break;
                default:
                    break;
            }
            for(b_row = b4r; b_row < (b4r+4); b_row++)
            {
                /*iterating through the 4x4 subblock
                    using diagonal element method as employed
                    in previous 2 cases*/
                temp = A[b_row];
                if(i == j)
                    diag = temp[b_row];
                for(b_col = b4c; b_col < (b4c+4); b_col++)
                {
                    if(b_row!=b_col)
                        B[b_col][b_row] = A[b_row][b_col];
                }
                if(i == j)
                    B[b_row][b_row] = diag;
            }
            }
        }
    }
    }
}
//#done :)

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
	/* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
			if (A[i][j] != B[j][i]) {
				return 0;
			}
		}
    }
    return 1;
}

