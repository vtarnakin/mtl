// c++17 @Tarnakin V.D.
//this header has a description of the matrix transforms
#pragma once
#ifndef TVD_MATH_HPP
#define TVD_MATH_HPP

#include "tvd/matrix/matrix.hpp"
#include "tvd/matrix/matrix_view.hpp"
#include "tvd/math_defines.hpp"
#include "tvd/algorithm.hpp"

#include <cmath>

#ifdef CXX_BUILDER_CXX17
# include <boost/optional.hpp>
# define TVD_NULLOPT boost::none
# define TVD_OPTIONAL(type) boost::optional<type>
#else
# include <optional>
# define TVD_NULLOPT std::nullopt
# define TVD_OPTIONAL(type) std::optional<type>
#endif

namespace tvd {
//
template<typename _Ty,
    is_arithmetic_t<_Ty> = true >
    TVD_OPTIONAL( detail::matrix_3xn_t<size_t> ) lee_neumann( matrix_view<_Ty> const& map,
                                                              size_t x_from, size_t y_from,
                                                              size_t x_to,   size_t y_to,
                                                              _Ty blank                     )
    {
      if( map.size() <= y_from || map.csize() <= x_from || map.size() <= y_to || map.csize() <= x_to ) {
          throw TVD_EXCEPTION("<tvd::lee_neumann> : out of range");
      }
      if( map[y_from][x_from] != blank || map[y_to][x_to] != blank ) {
          return TVD_NULLOPT;
      }

      using matrix_3xn_t = detail::matrix_3xn_t<size_t>;

      matrix_3xn_t    way  = { y_from, x_from, 0 };
      const int       dx[] = { 1, 0, -1,  0 };
      const int       dy[] = { 0, 1,  0, -1 };
      bool            stop;

      auto wave_propagation = [&map,  &blank,
                               &way,  &stop,
                               &dx,   &dy,
                               &x_to, &y_to  ]( const auto y, const auto x, const auto & d ) -> int
      {
        for( size_t i(0); i < 4; i++ )
        {
            int iy = y + dy[i], ix = x + dx[i];
            if ( (map.size() > iy || map.csize() > ix) &&
                  iy          >= 0                     &&
                  ix          >= 0                     &&
                  map[iy][ix] == blank                    )
            {
                if( insert_if( way, { iy, ix, d + 1 },
                    [&ix, &iy]( auto const& v ) {
                    return !(v[0] == iy && v[1] == ix);
                }) )
                {
                    if( iy == y_to && ix == x_to) {
                        return std::size(way) - 1;
                    }
                    stop = false;
                }
            }
        }
        return 0;
      };

      int y_end(0);
      size_t d(0);

      do {
          stop = true;
          for( size_t i(0); i < std::size(way); i++ )
          {
              if( way[i][2] == d ) {
                  y_end = wave_propagation( way[i][0], way[i][1], d );
              }
              if( y_end ) {
                  break;
              }
          }
          d++;
      } while( !stop );

      if( !y_end ) return TVD_NULLOPT;

      auto neighbour = [&dx, &dy]( auto const& curr_v, auto const& last_v )
      {
        return ( ( last_v[0] - 1 == curr_v[0]   || last_v[0] + 1 == curr_v[0] ) &&
                 ( last_v[1]     == curr_v[1] ) ||
                 ( last_v[1] - 1 == curr_v[1]   || last_v[1] + 1 == curr_v[1] ) &&
                 ( last_v[0]     == curr_v[0] )                                    );
      };

      size_t x( x_to ), y( y_to );
      decltype( way[0] ) last_v( way[y_end] );
      d = way[y_end][2] - 1;
      y_end--;
      matrix_3xn_t min_w;
      min_w.push_back( { x_to, y_to, 1 } );

      while( d > 0 ) {
          auto l = d;
          for( size_t i(y_end); i != 0; i-- )
          {
              if( way[i][2] == d && neighbour(way[i], last_v) ) {
                  min_w.push_back( { way[i][1], way[i][0], 1 } );
                  last_v = way[i];
                  d--;
                  break;
              }
          }
          if( l == d ) {
              return TVD_NULLOPT;
          }
      }

      min_w.push_back( { x_from, y_from, 1 } );
      return min_w;
    }
// LU
template<typename _MatrixTy,
    is_arithmetic_t<typename _MatrixTy::type_t> = true >
    std::pair<_MatrixTy, _MatrixTy> LU( _MatrixTy const& A )
    {
      auto size = A.csize();
      if( size != std::size(A) ) {
          throw TVD_EXCEPTION( "<tvd::LU> : <matrix.size> != <matrix.csize>" );
      }

      _MatrixTy L( size );
      _MatrixTy U = A;

      for(size_t i(0); i < size; i++)
          for(size_t j(i); j < size; j++)
              L[j][i] = U[j][i]/U[i][i];

      for(size_t k(1); k < size; k++)
      {
          for(size_t i(k - 1); i < size; i++)
              for(size_t j(i); j < size; j++)
                  L[j][i] = U[j][i]/U[i][i];
          for(size_t i(k); i < size; i++)
              for(size_t j(k - 1); j < size; j++)
                  U[i][j] = U[i][j] - L[i][k - 1]*U[k - 1][j];
      }
      return { L, U };
    }

template<typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void move( detail::matrix_3xn_t<_Ty> & m_res, _Ty x0, _Ty y0, _Ty x1, _Ty y1 )
    {
      detail::matrix_3xn_t<_Ty> t_tr
      {   1,          0,       0,
          0,          1,       0,
          x1 - x0,    y1 - y0, 1   };

      m_res *= t_tr;
    }

template<typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void move( detail::matrix_3xn_t<_Ty> & m_res, _Ty x, _Ty y ) {
      move( m_res, 0, 0, x, y );
    }

template<typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void scale( detail::matrix_3xn_t<_Ty> & m_res, _Ty k_x, _Ty k_y )
    {
      _Ty m = m_res[0][0]*(1 - k_x);
      _Ty l = m_res[0][1]*(1 - k_y);

      detail::matrix_3xn_t<_Ty> t_scl
      {   k_x, 0,   0,
          0,   k_y, 0,
          m,   l,   1   };

      m_res *= t_scl;
    }

template<typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void rotate( detail::matrix_3xn_t<_Ty> & m_res, int r_ang, _Ty x, _Ty y )
    {
      _Ty sin = std::sin(r_ang);
      _Ty cos = std::cos(r_ang);

      detail::matrix_3xn_t<_Ty> t_rot
      {   cos,                 sin,                 0,
         -sin,                 cos,                 0,
          x*(1 - cos) + y*sin, y*(1 - cos) - x*sin, 1   };

      m_res *= t_rot;
    }
} // tvd
# undef TVD_NULLOPT
# undef TVD_OPTIONAL
#endif
