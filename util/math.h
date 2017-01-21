#pragma once
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <algorithm>

template<typename T> std::vector<T> polyfit (const std::vector<T> &x, const std::vector<T> &y, int nDegree)
{
  using boost::numeric::ublas;
  
  if(x.size() != y.size())
  {
   throw std::invalid_argument('x and y size mismatch in polyfit!'); 
  }
  
  //TODO: WHY?
  nDegree++;
  
  auto nCount = x.size();
  matrix<T> xMatrix(nCount,nDegree);
  matrix<T> yMatrix(nCount,1);
  
  
  
  
  
}