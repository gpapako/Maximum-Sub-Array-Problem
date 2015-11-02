#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <mutex>

#define num_threads 48



// #define _PRINT_INFO 

using namespace std; 
mutex m;

void ps_func(int n_threads,int thr_id,int **ps,int **mat,int dim)
{ int i,j;
  int start=thr_id*dim/n_threads;
  int end=min((thr_id+1)*dim/n_threads, dim);
   
  for ( j=start; j<end; j++) {
        ps[0][j] = mat[0][j];
        for ( i=1; i<dim; i++) {
            ps[i][j] = ps[i-1][j] + mat[i][j];
        }
    }

}


long get_usecs (void)
{
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec*1000000+t.tv_usec;
}

void usage(const char* app_name) {
    printf("Argument error! Usage: %s <input_file> \n", app_name);
    exit(0);
}

void clear(int* a, int len) {
int index;
       //for (i=0;i<num_threads;i++)
         for ( index=0; index<len; index++) 
            a[index]=0;
}

void max_func(int dim,int n_threads,int thr_id,int **ps,int *top,int *left,int *bottom,int *right,int *max_sum)
{  int i,k,j;
   
   int sum[dim];
   int pos[dim];
   int local_max;
  // int start=thr_id*dim/n_threads;
   //int end=min((thr_id+1)*dim/n_threads, dim);

for ( i=thr_id; i<dim; i+=n_threads) {
        for ( k=i; k<dim; k++) {
            // Kandane over all columns with the i..k rows
           clear(sum, dim);
           clear(pos, dim);
            local_max=0;
            

            // We keep track of the position of the max value over each Kandane's execution
            // Notice that we do not keep track of the max value, but only its position
            sum[0] = ps[k][0] - (i==0 ? 0 : ps[i-1][0]);
            pos[0] = 0;
            for ( j=1; j<dim; j++) {
                if (sum[j-1] > 0){
                    sum[j] = sum[j-1] + ps[k][j] - (i==0 ? 0 : ps[i-1][j]);
                    pos[j] = pos[j-1];
                } 
                else {
                    sum[j] = ps[k][j] - (i==0 ? 0 : ps[i-1][j]);
                    pos[j] = j;
                }
                if (sum[j] > sum[local_max]){
                    local_max = j;
                }
            } //Kandane ends here
     
               m.lock();
            if (sum[local_max] > *max_sum) {
                // sum[local_max] is the new max value
                // the corresponding submatrix goes from rows i..k.
                // and from columns pos[local_max]..local_max

                *max_sum = sum[local_max];
                *top = i;
                *left = pos[local_max];
                *bottom = k;
                *right = local_max;
            }
             m.unlock();
        }
        
    }

}



int main(int argc, char* argv[]) {
    int i,j,k,l;
    if(argc != 2) {
        usage(argv[0]);
    }

    // Open files
    FILE* input_file = fopen(argv[1], "r");
    if(input_file == NULL) {
        usage(argv[0]);
    }

    // Read the matrix
    int dim = 0;
    fscanf(input_file, "%u\n", &dim);
    
    int **mat=(int **)malloc(dim*sizeof(int *));
      for (i=0;i<dim;i++)
       mat[i]=(int*)malloc(dim*sizeof(int));

    //int mat[dim][dim];
    int element = 0;

    for( i=0; i<dim; i++) {
        for( j=0; j<dim; j++) {
            if (j != (dim-1)) 
                fscanf(input_file, "%d\t", &element);
            else 
                fscanf(input_file, "%d\n",&element);
            mat[i][j] = element;
        }
    }

#ifdef _PRINT_INFO
    // Print the matrix
    printf("Input matrix [%d]\n", dim);
    for( i=0; i<dim; i++) {
        for( j=0; j<dim; j++) {
            printf("%d\t", mat[i][j]);
        }
        printf("\n");
    }
#endif

    // Algorithm based on information obtained here:
    // http://stackoverflow.com/questions/2643908/getting-the-submatrix-with-maximum-sum
    long alg_start = get_usecs();
    // Compute vertical prefix sum

    
    int **ps=(int **)malloc(dim*sizeof(int*));
      for (i=0;i<dim;i++)
       ps[i]=(int *)malloc(dim*sizeof(int));
   //int ps[dim][dim];
    /*for (int i=0; i<dim; i++) {
        for (int j=0; j<dim; j++) {
            if (j == 0) {
                ps[j][i] = mat[j][i];
            } 
            else {
                ps[j][i] = mat[j][j] + ps[i-1][i];
            }
        }
    }*/
    //int num_threads=48;
    
    thread t[num_threads];
    for ( i = 0; i < num_threads; i++)
     t[i] = thread(ps_func, num_threads, i,ps,mat,dim);

    for ( i = 0; i < num_threads; i++)
        t[i].join();

    
#ifdef _PRINT_INFO
    // Print the matrix
    printf("Vertical prefix sum matrix [%d]\n", dim);
    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++) {
            printf("%d\t", ps[i][j]);
        }
        printf("\n");
    }
#endif

    int max_sum = mat[0][0];
    int top = 0, left = 0, bottom = 0, right = 0; 

    //Auxilliary variables 
    
    //int **sum=(int **)malloc(num_threads*sizeof(int*));
      //for (i=0;i<num_threads;i++)
       //sum[i]=(int *)malloc(dim*sizeof(int));
    
    //int **pos=(int **)malloc(num_threads*sizeof(int*));
      //for (i=0;i<num_threads;i++)
       //pos[i]=(int *)malloc(dim*sizeof(int));
    
    //int *local_max=(int *)malloc(48*sizeof(int));
    //int local_max[num_threads];


    
    int *top_ptr,*left_ptr,*right_ptr,*bottom_ptr,*max;
    top_ptr=&top;
    bottom_ptr=&bottom;
    left_ptr=&left;
    right_ptr=&right;
    max=&max_sum;
    

    

    thread t1[num_threads];
    for ( i = 0; i < num_threads; i++)    
     t1[i] = thread(max_func, dim, num_threads, i,ps, top_ptr, left_ptr, bottom_ptr, right_ptr, max);
    for ( i = 0; i < num_threads; i++)
        t1[i].join();

   

    // Compose the output matrix
    int outmat_row_dim = bottom - top + 1;
    int outmat_col_dim = right - left + 1;
    int outmat[outmat_row_dim][outmat_col_dim];

    for( i=top, k=0; i<=bottom; i++, k++) {
        for( j=left, l=0; j<=right ; j++, l++) {
            outmat[k][l] = mat[i][j];
        }
    }

    long alg_end = get_usecs();

    // Print output matrix
    printf("Sub-matrix [%dX%d] with max sum = %d, left = %d, top = %d, right = %d, bottom = %d\n", outmat_row_dim, outmat_col_dim, max_sum, left, top, right, bottom);
#ifdef _PRINT_INFO
    for(int i=0; i<outmat_row_dim; i++) {
        for(int j=0; j<outmat_col_dim; j++) {
            printf("%d\t", outmat[i][j]);
        }
        printf("\n");
    }
#endif

    // Release resources
    fclose(input_file);

    // Print stats
    printf("%s,arg(%s),%s,%f sec\n", argv[0], argv[1], "CHECK_NOT_PERFORMED", ((double)(alg_end-alg_start))/1000000);
    return 0;
}
