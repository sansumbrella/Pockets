/*
 * Copyright (c) 2015 David Wicks, sansumbrella.com
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
#include "Pockets.h"

namespace pockets {

///
/// Generic cache structure.
/// Removes oldest items first when max cache size is reached.
/// Uses string keys, since most common use case is for cacheing disk assets.
///
/// Basic usage:
/// if (cache.hasEntry("thing"))
///	{
///		auto thing = cache.retrieve("thing");
/// }
/// else
/// {
///   auto thing = createThing();
///   cache.store(thing, "thing", sizeof(thing));
/// }
template <typename T>
class Cache
{
public:
	struct CacheEntry
	{
		CacheEntry() = default;

		CacheEntry( const std::string &iName, const T &iItem, uint32_t iSize, uint64_t iRequestTime )
		: name( iName ),
			item( iItem ),
			size( iSize ),
			requestTime( iRequestTime )
		{}

		std::string		name;
		T							item;
		uint32_t			size = 0;
		uint64_t			requestTime = 0;
	};

	bool contains( const std::string &name ) { return cache.count( name ); }

	/// Returns item if it exists in the cache. Otherwise returns default-constructed item.
	T	retrieve( const std::string &name );

	/// Store an item in the cache by name. Tell the cache how large the item is (e.g. in bytes).
	void store( const T &item, const std::string &name, uint32_t size );
	/// Store an item in the cache by name. Size is calculated using templated measure function.
	void store( const T &item, const std::string &name) { store(item, name, measure(item)); }

	/// Erase an item from the cache by name. For advanced use cases.
	void erase( const std::string &name );

	/// Set the maximum size of the cache. This is in whatever units you use to specify size when storing elements.
	/// The default cache size assumes size is specified in bytes and allows for up to 1GB of space to be used.
	void setMaxSize( uint32_t size ) { maxStoredBytes = size; }

	uint32_t getCurrentSize() const { return storedBytes; }

	uint32_t measure( const T &item );

private:

	void makeRoom();

	std::map<std::string, CacheEntry>		cache;

	uint64_t														requestCount = 0;
	uint32_t														storedBytes = 0;
	uint32_t														maxStoredBytes = 1000 * 1000 * 1000; // ~1GB
};

// ===================================
// Cache Template Implementation
// ===================================

template <typename T>
uint32_t Cache<T>::measure( const T &item )
{
	return sizeof(item);
}

template <typename T>
void Cache<T>::store( const T &item, const std::string &name, uint32_t size )
{
	auto iter = cache.find(name);
	if (iter != cache.end()) {
		storedBytes -= iter->second.size;
		cache.erase(iter);
	}

	storedBytes += size;
	makeRoom();

	requestCount += 1;
	cache[name] = CacheEntry( name, item, size, requestCount );
}

template <typename T>
void Cache<T>::erase( const std::string &name )
{
	auto iter = cache.find(name);
	if (iter != cache.end()) {
		storedBytes -= iter->second.size;
		cache.erase(iter);
	}
}

template <typename T>
T Cache<T>::retrieve( const std::string &name )
{
	auto iter = cache.find( name );
	if( iter != cache.end() ) {
		requestCount += 1;

		auto &entry = iter->second;
		entry.requestTime = requestCount;
		return entry.item;
	}

	return T();
}

template <typename T>
void Cache<T>::makeRoom()
{
	while( storedBytes > maxStoredBytes ) {
		if( cache.empty() ) {
			return;
		}

		auto oldest = cache.begin();
		uint64_t age = oldest->second.requestTime;

		for( auto iter = cache.begin(); iter != cache.end(); ++iter ) {
			if( iter->second.requestTime < age ) {
				oldest = iter;
				age = oldest->second.requestTime;
			}
		}

		storedBytes -= oldest->second.size;
		cache.erase( oldest );
	}
}

} // namespace pockets
