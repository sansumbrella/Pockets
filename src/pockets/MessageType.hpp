//
//  MessageType.h
//  WordShift
//
//  Created by David Wicks on 2/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

namespace pockets
{
	/**
   MessageT<T>: A template for Messages containing a subject (T) and event (enumerated type).

	 The Message carries a const ref to the subject and a string describing what happened
	 MessageT<YourClass>( yourInstance, Enumeration::WhatHappened );
   A convenience class so you don't always need to spell out your basic event types.
	 */
	template <class T>
	class MessageT
	{
	public:
		typedef T Subject;
		MessageT( const Subject &subject, int event_id ):
		mSubject( subject )
		, mEvent( event_id )
		{}
		//! what is the subject of this message, typically will be the sender
		const Subject&		getSubject() const { return mSubject; }
		//! what happened that I should be interested in subject
		int	getEvent() const { return mEvent; }
	private:
		const Subject	&mSubject;
		int		mEvent;
	};
}
