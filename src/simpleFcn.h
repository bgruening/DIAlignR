#ifndef SIMPLEFCN_H
#define SIMPLEFCN_H

#include <Rcpp.h>
#include <vector>
#include "affinealignobj.h"
using namespace Rcpp;

#ifdef USE_PRECONDITION
#define PRECONDITION(condition, message) assert(condition); // If you don't put the message, C++ will output the code.
#else
#define PRECONDITION(condition, message) ; // If you don't put the message, C++ will output the code.
#endif

//' Outputs a NumericMatrix of given row and column size.
//'
//' @author Shubham Gupta, \email{shubh.gupta@mail.utoronto.ca}
//' ORCID: 0000-0003-3500-8152
//' License: (c) Author (2019) + MIT
//' Date: 2019-03-05
//' @param initVal (char) Matrix intialization value
//' @param ROW_SIZE (int) Number of rows
//' @param COL_SIZE (int) Number of columns
//' @return s (NumericMatrix) A matrix
//' @examples
//' # Get a matrix of type NumericMatrix
//' initializeMatrix(0, ROW_SIZE = 4, COL_SIZE = 5)
//' @export
// [[Rcpp::export]]
NumericMatrix initializeMatrix(float initVal, int ROW_SIZE, int COL_SIZE);

// Template definitions should always be in header file.
template<class T>
void printMatrix(T Mat, int ROW_SIZE, int COL_SIZE){
  for(int i = 0; i < ROW_SIZE; i++){
    for(int j = 0; j < COL_SIZE; j++){
      Rcpp::Rcout << Mat[i*COL_SIZE + j] << " ";
    }
    Rcpp::Rcout << std::endl;
  }
}

struct SimMatrix
{
  std::vector<double> data;
  int n_row;
  int n_col;
};

struct SimMatrix_bool
{
  std::vector<bool> data;
  int n_row;
  int n_col;
};

double getQuantile(std::vector<double> vec, double quantile);

#endif // SIMPLEFCN_H
