//
//  Receiver.hpp
//  WordShift
//
//  Created by David Wicks on 2/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

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
