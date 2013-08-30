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
#include <vector>

namespace pockets
{
	template<class T>
	class Messenger;

	/**
   Receiver receives messages of type T
   Keeps track of which messengers it's listening to and unregisters itself in
   the destructor.

   For usage explanation, see Messenger.hpp
   */
	template <class T>
	class Receiver
	{
	public:
		//! default constructor
		Receiver() = default;
		//! Copy the Messenger/Receiver relationship of \a other
		Receiver( const Receiver &other ):
		mMessengers()
		{
			for( auto m : other.mMessengers )
			{
				m->appendReceiver( this );
			}
		}
		//! Copy the Messenger/Receiver relationship of \a rhs
		Receiver& operator=( const Receiver &rhs )
		{
			for( auto m : mMessengers )
			{
				m->removeDestructingReceiver( this );
			}
			mMessengers.clear();
			for( auto m : rhs.mMessengers )
			{
				m->appendReceiver( this );
			}
      return *this;
		}
		//! Destructor removes Receiver from all Messengers
		virtual ~Receiver()
		{
			for( auto m : mMessengers )
			{
				m->removeDestructingReceiver( this );
			}
		}
    //! Implement receive( const T& ) to handle incoming messages.
		virtual void receive( const T &message ) = 0;
	private:
    typedef Messenger<T> MessengerT;
		friend MessengerT;
		//! Called by Messenger when this Receiver is appended
		void registerMessenger( MessengerT *m )
		{
			mMessengers.push_back( m );
		}
		//! Called by Messenger when this Receiver is removed
		void unregisterMessenger( const MessengerT *messenger )
		{
			mMessengers.erase( remove_if( mMessengers.begin()
                                   , mMessengers.end()
                                   , [=]( MessengerT *other ){ return other == messenger; } )
                        , mMessengers.end() );
		}
		//! All messengers to whom we are listening
		std::vector<MessengerT*>	mMessengers;
	};
}
