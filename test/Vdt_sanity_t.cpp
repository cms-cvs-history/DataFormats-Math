/*******************************************************************************
 * 
 * Test program to keep under control the numerical output of the functions
 * 
 * ****************************************************************************/

#include <cmath>
#include <stdlib.h>
#include "DataFormats/Math/interface/VDTMath.h"

//------------------------------------------------------------------------------
void test_input(int argc, char** argv);
void test_simple_function(double* rnd_numbers,
                          unsigned int n_rnd_numbers, 
                          const char*, 
                          double(*func) (double),
                          const char*, 
                          double(*ref_func) (double) = NULL);

void test_vector_function(double* rnd_numbers,
                          unsigned int n_rnd_numbers, 
                          const char*, 
                          void (*func) (double*,double*,const unsigned int),
                          const char*, 
                          void (*ref_func) (double*,double*,const unsigned int));

void generate_rndm_numbers(double* rnd_numbers,
                           const unsigned int n_rnd_numbers, 
                           double min, 
                           double max);


bool check_dp(double arg, double a, double b);
//------------------------------------------------------------------------------

const double MAX_RND_EXP = vdt::EXP_LIMIT;
const double MIN_RND_LOG = vdt::LOG_LOWER_LIMIT;
const double MAX_RND_LOG = 1e200;
const double EPSILON = 1e-5;

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
  std::cout << "Testing " << n_rnd_numbers << " random numbers\n";  
   
  double* rnd_numbers = new double[n_rnd_numbers];
 
  // Generate the random numbers
  generate_rndm_numbers(rnd_numbers,n_rnd_numbers,-MAX_RND_EXP, MAX_RND_EXP);

  // Test the timings
  test_simple_function(rnd_numbers,n_rnd_numbers, 
                       "vdt::fast_exp", vdt::fast_exp, "std::exp", std::exp);  
  test_vector_function(rnd_numbers,n_rnd_numbers, 
                       "vdt::fast_exp_vect", vdt::fast_exp_vect, "vdt::std_exp_vect", vdt::std_exp_vect );
  test_vector_function(rnd_numbers,n_rnd_numbers, 
                       "vdt::__future_fast_exp_vect", vdt::__future_fast_exp_vect, "vdt::std_exp_vect", vdt::std_exp_vect  );  
  
  generate_rndm_numbers(rnd_numbers,n_rnd_numbers,MIN_RND_LOG, MAX_RND_LOG);  
  test_simple_function(rnd_numbers,n_rnd_numbers, 
                       "vdt::fast_log", vdt::fast_log, "std::log", std::log);  
  test_vector_function(rnd_numbers,n_rnd_numbers, 
                       "vdt::fast_log_vect", vdt::fast_log_vect, "vdt::std_log_vect", vdt::std_log_vect);  
//   test_vector_function(rnd_numbers,n_rnd_numbers, 
//                        "vdt::__future_fast_log_vect", vdt::__future_fast_log_vect, "vdt::std_log_vect", vdt::std_log_vect );

  delete[] rnd_numbers;
  
  
}
  
//------------------------------------------------------------------------------
  
void generate_rndm_numbers(double* rnd_numbers,const unsigned int n_rnd_numbers,double min, double max){

  double norm_rnd=0;
  double rnd_delta = 0;   
  for (unsigned int i=0;i<n_rnd_numbers;++i){  
    norm_rnd = rand()/double(RAND_MAX);
    rnd_delta = norm_rnd * (max - min);        
    rnd_numbers[i] = min + rnd_delta;
  }
  std::cout << "\n*** " << n_rnd_numbers 
            << " numbers (" << min << "," << max<< ")\n";

  }

//------------------------------------------------------------------------------

bool check_dp(double arg, double a, double b){
  
  double ratio = a/b;
  if (fabs(ratio-1) > EPSILON){
    std::cout << "Difference: " << "x=" << arg
              << "    --> f1(x)="<< a << " --- f2(x)=" << b << std::endl;
  }
  
  return true;
  
}

//------------------------------------------------------------------------------

void test_simple_function(double* rnd_numbers,
                          unsigned int n_rnd_numbers, 
                          const char* name, 
                          double(*func) (double),
                          const char* ref_name, 
                          double(*ref_func) (double)){
  
  std:: cout << "Testing " << name << " VS " << ref_name << std::endl;
  
  double fx1=0;
  double fx2=0;
  for (unsigned int i=0;i<n_rnd_numbers;++i){  
    fx1=func(rnd_numbers[i]);
    fx2=ref_func(rnd_numbers[i]);
    check_dp(rnd_numbers[i],fx1,fx2);    
    }
  
  std:: cout << "----------------" << std::endl;                            
  }


//------------------------------------------------------------------------------

void test_vector_function(double* rnd_numbers,
                          unsigned int n_rnd_numbers, 
                          const char* name, 
                          void (*func) (double*,double*,const unsigned int),
                          const char* ref_name, 
                          void (*ref_func) (double*,double*,const unsigned int)){

  std:: cout << "Testing " << name << " VS " << ref_name << std::endl;  
  
  double* fx1  = new double[n_rnd_numbers];  
  double* fx2 = new double[n_rnd_numbers];     
  
  func (rnd_numbers,fx1, n_rnd_numbers);
  ref_func (rnd_numbers,fx2, n_rnd_numbers);
  
  for (unsigned int i=0;i<n_rnd_numbers;++i)
    check_dp(rnd_numbers[i],fx1[i],fx2[i]);   

  delete [] fx1;
  delete [] fx2;
  
  std:: cout << "----------------" << std::endl;
  }
  
//------------------------------------------------------------------------------

void test_input(int argc, char** argv){

  if (argc!=1 and argc!=2){
    std::cout << "Usage: " << argv[0] << " [number of random numbers]\n";
    exit(1);
    }
}
//------------------------------------------------------------------------------
