/*
 * Copyright (c) 2013 David Wicks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

/**
 Functions for easier manipulation of STL containers.
*/

#include "Pockets.h"

namespace pockets
{
	//! Remove all elements from \a map for which \a compare returns true
	template<class MAP_TYPE, class COMPARATOR>
	void map_erase_if( MAP_TYPE *map, COMPARATOR compare )
	{
		auto iter = map->begin();
		while( iter != map->end() )
		{
			if( compare( iter->second ) )
			{
				map->erase( iter++ );
			}
			else
			{
				iter++;
			}
		}
	}

  //! Remove all elements from \a vec that match \a compare
  template<class ELEMENT_TYPE, class COMPARATOR>
  void vector_erase_if( std::vector<ELEMENT_TYPE> *vec, COMPARATOR compare )
  {
    vec->erase( std::remove_if( vec->begin()
                              , vec->end()
                              , compare )
               , vec->end() );
  }

  //! Remove all copies of the element \a compare from \a vec
  template<class ELEMENT_TYPE>
  void vector_remove( std::vector<ELEMENT_TYPE> *vec, const ELEMENT_TYPE &compare )
  {
    vec->erase( std::remove_if( vec->begin()
                               , vec->end()
                               , [=](const ELEMENT_TYPE &element){ return element == compare; } )
               , vec->end() );
  }

  //! Returns true if \a vec contains the element \a compare
  template<class ELEMENT_TYPE>
  bool vector_contains( const std::vector<ELEMENT_TYPE> &vec, const ELEMENT_TYPE &compare )
  {
    return std::find( vec.begin(), vec.end(), compare ) != vec.end();
  }

  //! Returns true if \a compare function returns true for an element in \a vec
  template<class ELEMENT_TYPE, class COMPARATOR>
  bool vector_contains( const std::vector<ELEMENT_TYPE> &vec, COMPARATOR compare )
  {
    return std::find_if( vec.begin(), vec.end(), compare ) != vec.end();
  }
}
