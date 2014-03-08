//
//  Node.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "pockets/Locus.h"
#include "pockets/ConnectionManager.h"
#include "cinder/app/App.h"

namespace pockets
{
  namespace cobweb
  {
  /**
  A thing positioned on screen that can connect to a window.
  Uses UI signals for events, does not connect to draw signal.
  Has no default interactions, instead lets any interested children connect
  or disconnect to signals they care about.

  A simple scene graph, modeled loosely on AS3 Sprites.
  */

  typedef std::shared_ptr<class Node> NodeRef;
  typedef std::unique_ptr<class Node> NodeUniqueRef;
  typedef std::weak_ptr<class Node>   NodeWeakRef;
  class Node
  {
  public:
    static NodeUniqueRef create();
	//! construct an empty node
	//! note that stack-allocated nodes are only useful as a root node
	Node():
      mLocus( std::make_shared<Locus2D>() ),
      mParent( nullptr )
    {}
    virtual ~Node();
    void            deepDraw();
    void            deepConnect( ci::app::WindowRef window );
    void            deepDisconnect();

    //! call block() on all child objects
    void            blockChildren();
    //! call unblock() on all child objects
    void            unblockChildren();
    virtual void    draw(){}
    //! called in deepdraw before drawing children
    virtual void    preChildDraw(){}
    //! called in deepdraw after drawing children
    virtual void    postChildDraw(){}
    //! block all stored signals
    void            block();
    //! called immediately before stored signals are blocked
    virtual void    customBlock(){}
    //! unblock all stored signals
    void            unblock();
    //! called immediately after signals are unblocked
    virtual void    customUnblock(){}
    //! connect to any UI signals of interest here
    virtual void    connect( ci::app::WindowRef window ){}
    //! disconnects all stored connections
    void            disconnect();
    //! disconnect from any non-stored signals here
    virtual void    customDisconnect(){}
    //! stop whatever event-related tracking this object was doing
    virtual void    cancelInteractions(){}
    void            deepCancelInteractions();
    //! set top-left of element
    void            setPosition( const ci::Vec2f &pos ){ mLocus->position = pos; }
    ci::Vec2f       getPosition() const { return mLocus->getPosition(); }
	//! set xy scale of element
	void			setScale( const ci::Vec2f &scale ){ mLocus->scale = scale; }
	ci::Vec2f		getScale() const { return mLocus->getScale(); }
    //! set rotation around z-axis
    void            setRotation( float radians ){ mLocus->rotation = radians; }
    void            setRegistrationPoint( const ci::Vec2f &loc ){ mLocus->registration_point = loc; }
    pk::Locus2DRef  getLocus(){ return mLocus; }
    //! add a Node as a child; will receive connect/disconnect events and have its locus parented
    void            appendChild( NodeRef element );
    //! called when a child is added to this Node
    virtual void    childAdded( NodeRef element ){}
    void            removeChild( NodeRef element );
    Node*           getParent(){ return mParent; }

    // Child Manipulation
    size_t          numChildren() const { return mChildren.size(); }
    NodeRef         getChildAt( size_t index ){ return mChildren.at( index ); }
    void            setChildIndex( NodeRef child, size_t index );
    //! return child vector, allowing manipulation of each child, but not the vector
    const std::vector<NodeRef>& getChildren() const { return mChildren; }
  protected:
    //! store a connection so it can be blocked/unblocked/disconnected later
    void            storeConnection( const ci::signals::connection &connection ){ mConnectionManager.store( connection ); }
  private:
    pk::Locus2DRef          mLocus;
    Node*                   mParent;
    std::vector<NodeRef>    mChildren;
    ConnectionManager       mConnectionManager;
    void setParent( Node *parent );
  };
  } // cobweb::
} // pockets::
