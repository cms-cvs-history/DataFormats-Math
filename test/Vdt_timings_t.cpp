/*******************************************************************************
 * 
 * Test program to keep under control the speed of the mathematical functions
 * 
 * ****************************************************************************/

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <cmath>
#include <memory>
#include <string>
#include "DataFormats/Math/interface/vdt_math.h"


void test_input(int argc, char** argv);
void generate_rndm_numbers(double* rnd_numbers,const unsigned int n_rnd_numbers, double min, double max);
void time_simple_function(double* rnd_numbers,
                                        unsigned int n_rnd_numbers, 
                                        const char*, 
                                        double(*func) (double));
void time_vector_function(double* rnd_numbers,
                                        unsigned int n_rnd_numbers, 
                                        const char*, 
                                        void (*func) (double*,double*,const unsigned int));

//------------------------------------------------------------------------------

const unsigned int REPETITIONS = 1000;
const double MAX_RND_EXP = vdt::EXP_LIMIT;
const double MIN_RND_LOG = vdt::LOG_LOWER_LIMIT;
const double MAX_RND_LOG = vdt::LOG_UPPER_LIMIT;

int main(int argc, char** argv){

  // Usage
  test_input(argc,argv);

  // print the instruction sets that are used
  vdt::print_instructions_info();

  // Fill a vector of random numbers
  int n_rnd_numbers = 10000;
  if (argc>=2)
    n_rnd_numbers = atoi(argv[1]);

  // Presentation of the test_input
  std::cout << "--------------------------------------------------------\n";
  std::cout << "Testing " << n_rnd_numbers << " random numbers " << REPETITIONS << " times.\n";  
   
  double* rnd_numbers = new double[n_rnd_numbers];
 
  // Generate the random numbers
  generate_rndm_numbers(rnd_numbers,n_rnd_numbers,-MAX_RND_EXP, MAX_RND_EXP);

  // Test the timings
  time_simple_function(rnd_numbers,n_rnd_numbers, "std::exp", std::exp );
  time_simple_function(rnd_numbers,n_rnd_numbers, "vdt::fast_exp", vdt::fast_exp);  
  time_vector_function(rnd_numbers,n_rnd_numbers, "vdt::std_exp_vect", vdt::std_exp_vect );
  time_vector_function(rnd_numbers,n_rnd_numbers, "vdt::fast_exp_vect", vdt::fast_exp_vect );
  time_vector_function(rnd_numbers,n_rnd_numbers, "vdt::__future_fast_exp_vect", vdt::__future_fast_exp_vect );  
  
  generate_rndm_numbers(rnd_numbers,n_rnd_numbers,MIN_RND_LOG, MAX_RND_LOG);  
  time_simple_function(rnd_numbers,n_rnd_numbers, "std::log", std::log );
  time_simple_function(rnd_numbers,n_rnd_numbers, "vdt::fast_log", vdt::fast_log);  
  time_vector_function(rnd_numbers,n_rnd_numbers, "vdt::std_log_vect", vdt::std_log_vect );  
  time_vector_function(rnd_numbers,n_rnd_numbers, "vdt::fast_log_vect", vdt::fast_log_vect );  
  time_vector_function(rnd_numbers,n_rnd_numbers, "vdt::__future_fast_log_vect", vdt::__future_fast_log_vect );

   delete[] rnd_numbers;

}

//------------------------------------------------------------------------------

void time_vector_function(double* rnd_numbers,
                                        unsigned int n_rnd_numbers, 
                                        const char* name, 
                                        void (*func) (double*,double*,const unsigned int)){

  double* cache = new double [n_rnd_numbers];

  double init = 0;
  double delta_t=0;
  double tot_time=0;
  double tot_time_2=0; 
  for (unsigned int j=0;j<REPETITIONS;++j){
    init = omp_get_wtime();
    func(rnd_numbers,cache,n_rnd_numbers);
    delta_t = omp_get_wtime() - init;
    delta_t*=1000.;    
    tot_time+=delta_t;
    tot_time_2+=delta_t*delta_t; 
    // to fool the extreme compiler optimisations like -Ofast
    for (unsigned int i=0;i<n_rnd_numbers;++i)
      if (cache[i] == 0.66666){
        std::cout << "Found!\n";    
        std::cout << rnd_numbers[i] << " " << cache[i]<<"\n";
        }
  }
  double mean_time = tot_time/REPETITIONS;
  double mean_time_e = sqrt((tot_time_2 - tot_time*tot_time/REPETITIONS)/(REPETITIONS-1));
  std::cout << "o Timing of "<< name << " is " << mean_time << "+-" << mean_time_e  << " ms" << std::endl;
  
  delete[] cache;

}

//------------------------------------------------------------------------------

void time_simple_function(double* rnd_numbers,
			  unsigned int n_rnd_numbers,
			  const char* name, 
			  double(*func) (double)){

  double* cache = new double [n_rnd_numbers];

  double init = 0;
  double delta_t=0;
  double tot_time=0;  
  double tot_time_2=0;  
  for (unsigned int j=0;j<REPETITIONS;++j){
    init = omp_get_wtime();
    for (unsigned int i=0;i<n_rnd_numbers;++i)
      cache[i]=func(rnd_numbers[i]);
    delta_t = omp_get_wtime() - init;
    delta_t*=1000.;    
    tot_time+=delta_t;   
    tot_time_2+=delta_t*delta_t;  
    // to fool the extreme compiler optimisations like -Ofast
    for (unsigned int i=0;i<n_rnd_numbers;++i)
      if (cache[i] == 0.000066666){
        std::cout << "Found!\n";
        }
    }

  double mean_time = tot_time/REPETITIONS;
  double mean_time_e = sqrt((tot_time_2 - tot_time*tot_time/REPETITIONS)/(REPETITIONS-1));
  std::cout << "o Timing of "<< name << " is " << mean_time << "+-" << mean_time_e << " ms" << std::endl;

  delete [] cache;
  
}

//------------------------------------------------------------------------------
void generate_rndm_numbers(double* rnd_numbers,const unsigned int n_rnd_numbers,double min, double max){
/**
 * Generate between -MAX_RND and MAX_RND double numbers
 **/
  double norm_rnd=0;
  double rnd_delta = 0;
  double init = omp_get_wtime();    
  for (unsigned int i=0;i<n_rnd_numbers;++i){  
    norm_rnd = rand()/RAND_MAX;
    rnd_delta = norm_rnd * (max - min);        
    rnd_numbers[i] = min + rnd_delta;
    //std::cout << "o " << rnd_numbers[i] << std::endl;
  }
  double delta_t = omp_get_wtime() - init;
  std::cout << "\n*** " << n_rnd_numbers 
            << " numbers (" << min << "," << max<< ") generated in " 
            << delta_t << " s \n";

  }

//------------------------------------------------------------------------------

void test_input(int argc, char** argv){

  if (argc!=1 and argc!=2){
    std::cout << "Usage: " << argv[0] << " [number of random numbers]\n";
    exit(1);
    }
}

//------------------------------------------------------------------------------
