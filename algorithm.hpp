// c++17 @Tarnakin V.D.
//this header has a description of the algorithms
#pragma once
#ifndef TVD_ALGORITHM_HPP
#define TVD_ALGORITHM_HPP

#include <algorithm>

namespace tvd {
// insert vector to matrix if
template<
    class _MatrixTy,
    class _VectorTy,
    class _ConditionTy>
    bool insert_if( _MatrixTy & m, _VectorTy const& v, _ConditionTy const& c) noexcept
    {
      for( size_t i(0); i < std::size(m); i++ )
          if( !c(m[i]) ) return false;
      m.push_back( v );
      return true;
    }
// swap if
template<
    typename _Ty,
    typename _ConditionTy>
    void swap_if( _Ty & l, _Ty & r, _ConditionTy const& condition )
    {
      if( !condition(l, r) ) return;
      std::swap(l, r);
    }
// min value in matrix column
template<typename _MatrixTy>
    auto min( _MatrixTy const& m, size_t j_pos ) -> decltype( *m.begin() )
    {
      if( m.empty() ) {
          throw TVD_EXCEPTION("<tvd::min> : <matrix> is empty");
      }
      auto size = m.csize();
      if( size <= j_pos ) {
          throw TVD_EXCEPTION("<tvd::min> : <matrix.csize> <= <j_pos>");
      }
      auto first = m.cbegin();
      auto last = m.cend();
      first += j_pos;
      auto min = (*first);
      for(auto & fst(first); fst < last; fst += size)
      {
          if( min > (*fst) ) min = (*fst);
      }
      return min;
    }
// max value in matrix column
template<typename _MatrixTy>
    auto max( _MatrixTy const& m, size_t j_pos ) -> decltype( *m.begin() )
    {
      if( m.empty() ) {
          throw TVD_EXCEPTION("<tvd::max> : <matrix> is empty");
      }
      auto size = m.csize();
      if( size <= j_pos ) {
          throw TVD_EXCEPTION("<tvd::max> : <matrix.csize> <= <j_pos>");
      }
      auto first = m.cbegin();
      auto last = m.cend();
      auto max = (*first);
      first += j_pos;
      for(auto & fst(first); fst < last; fst += size)
      {
          if( max < (*fst) ) max = (*fst);
      }
      return max;
    }
// min & max value in matrix column
template<typename _MatrixTy>
    auto minmax( _MatrixTy const& m, size_t j_pos )
      -> std::pair<decltype( min( m, j_pos ) ), decltype( max( m, j_pos ) )>
    {
      return { min( m, j_pos ), max( m, j_pos ) };
    }
} // tvd
#endif
