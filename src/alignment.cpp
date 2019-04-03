#include "alignment.h"

AlignObj doAlignment(NumericMatrix s, int signalA_len, int signalB_len, float gap, bool OverlapAlignment){
  AlignObj alignObj(signalA_len+1, signalB_len+1);
  alignObj.FreeEndGaps = OverlapAlignment;
  alignObj.GapOpen = gap;
  alignObj.GapExten = gap;
  alignObj.signalA_len = signalA_len;
  alignObj.signalB_len = signalB_len;

  NumericMatrix M;
  M = initializeMatrix(0, signalA_len+1, signalB_len+1);

  // Rcpp::Rcout << M << std::endl;
  std::vector<TracebackType> Traceback;
  Traceback.resize((signalA_len+1)*(signalB_len+1), SS);

  // Initialize first row and first column for global and overlap alignment.
  for(int i = 0; i<=signalA_len; i++){
    M(i, 0) = -i*gap;
    Traceback[i*(signalA_len+1)+0] = TM; //Top
  }

  for(int j = 0; j<=signalB_len; j++){
    M(0, j) = -j*gap;
    Traceback[0*(signalA_len+1)+j] = LM; //Left
  }
  Traceback[0*(signalA_len+1) + 0] = SS; //STOP

  // Rcpp::Rcout << M << std::endl;

  // Perform dynamic programming for alignment
  float Diago, gapInA, gapInB;
  for(int i=1; i<=signalA_len; i++ ){
    for(int j=1; j<=signalB_len; j++){
      Diago = M(i-1, j-1) + s(i-1, j-1);
      gapInA = M(i-1, j) - gap;
      gapInB = M(i, j-1) - gap;
      if(Diago>=gapInA && Diago>=gapInB){
        Traceback[i*(signalA_len+1) + j] = DM; // D: Diagonal
        M(i, j) = Diago;
      }
      else if (gapInA>=Diago && gapInA>=gapInB){
        Traceback[i*(signalA_len+1) + j] = LM; // L: Left
        M(i, j) = gapInA;
      }
      else{
        Traceback[i*(signalA_len+1) + j] = TM; // T: Top
        M(i, j) = gapInB;
      }
    }
  }

  // Rcpp::Rcout << M << std::endl;
  for (int i = 0; i < signalA_len+1; i++) {
    for (int j = 0; j < signalB_len+1; j++) {
      // Rcpp::Rcout << i*(signalA_len+1) + j << std::endl;
      // Rcpp::Rcout << M(i, j) << std::endl;
      // Rcpp::Rcout << alignObj.M[i*(signalA_len+1) + j] << std::endl;
      alignObj.M[i*(signalA_len+1) + j] = M(i, j); // Add an element (column) to the row
      // Rcpp::Rcout << alignObj.M[i*(signalA_len+1) + j]<< std::endl;
      alignObj.Traceback[i*(signalA_len+1) + j] = Traceback[i*(signalA_len+1)+j]; // Add an element (column) to the row
    }
  }

  // for (auto i = alignObj.M.begin(); i != alignObj.M.end(); ++i)
  //   Rcpp::Rcout << *i << ' ';
  // for (auto i = alignObj.Traceback.begin(); i != alignObj.Traceback.end(); ++i)
  //   Rcpp::Rcout << *i << ' ';
  return alignObj;
}

AlignedIndices getAlignedIndices(AlignObj &alignObj){
  AlignedIndices alignedIdx;
  TracebackType TracebackPointer;
  float alignmentScore;
  int ROW_IDX = alignObj.signalA_len;
  int COL_IDX = alignObj.signalB_len;
  int ROW_SIZE = alignObj.signalA_len + 1;
  int COL_SIZE = alignObj.signalB_len + 1;

  alignedIdx.indexA_aligned.insert(alignedIdx.indexA_aligned.begin(), ROW_IDX);
  alignedIdx.indexB_aligned.insert(alignedIdx.indexB_aligned.begin(), COL_IDX);
  alignedIdx.score.insert(alignedIdx.score.begin(), alignObj.M[ROW_IDX*COL_SIZE+COL_IDX]);
  TracebackPointer = alignObj.Traceback[ROW_IDX*COL_SIZE+COL_IDX];
  // Traceback path and align row indices to column indices.
  while(TracebackPointer != SS){
    // D: Diagonal, T: Top, L: Left
    switch(TracebackPointer){
    case DM: {
      ROW_IDX = ROW_IDX - 1;
      COL_IDX = COL_IDX - 1;
      alignedIdx.indexA_aligned.insert(alignedIdx.indexA_aligned.begin(), ROW_IDX);
      alignedIdx.indexB_aligned.insert(alignedIdx.indexB_aligned.begin(), COL_IDX);
      alignedIdx.score.insert(alignedIdx.score.begin(), alignObj.M[ROW_IDX*COL_SIZE+COL_IDX]);
      break;
    }
    case TM:
    {
      ROW_IDX = ROW_IDX - 1;
      alignedIdx.indexA_aligned.insert(alignedIdx.indexA_aligned.begin(), ROW_IDX);
      alignedIdx.indexB_aligned.insert(alignedIdx.indexB_aligned.begin(), NA);
      alignedIdx.score.insert(alignedIdx.score.begin(), alignObj.M[ROW_IDX*COL_SIZE+COL_IDX]);
      break;
    }
    case LM:
    {
      COL_IDX = COL_IDX - 1;
      alignedIdx.indexA_aligned.insert(alignedIdx.indexA_aligned.begin(), NA);
      alignedIdx.indexB_aligned.insert(alignedIdx.indexB_aligned.begin(), COL_IDX);
      alignedIdx.score.insert(alignedIdx.score.begin(), alignObj.M[ROW_IDX*COL_SIZE+COL_IDX]);
      break;
    }
    }
    TracebackPointer = alignObj.Traceback[ROW_IDX*COL_SIZE+COL_IDX];
  }
  alignedIdx.score.erase(alignedIdx.score.begin());
  alignedIdx.indexA_aligned.erase(alignedIdx.indexA_aligned.begin());
  alignedIdx.indexB_aligned.erase(alignedIdx.indexB_aligned.begin());
  return alignedIdx;
}