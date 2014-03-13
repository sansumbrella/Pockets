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
  Nodes are connected in a tree; they can have multiple children
  but only one parent.
  */

  typedef std::shared_ptr<class Node> NodeRef;
  typedef std::unique_ptr<class Node> NodeUniqueRef;
  class Node
  {
  public:
    static NodeUniqueRef create();
  //! construct an empty node
  //! note that stack-allocated nodes are only useful as a root node
  Node():
      mLocus(),
      mParent( nullptr )
    {}
    virtual ~Node();
    // Mouse and touch interaction
    bool            deepTouchesBegan( ci::app::TouchEvent &event );
    bool            deepTouchesMoved( ci::app::TouchEvent &event );
    bool            deepTouchesEnded( ci::app::TouchEvent &event );
    bool            deepMouseDown( ci::app::MouseEvent &event );
    bool            deepMouseDrag( ci::app::MouseEvent &event );
    bool            deepMouseUp( ci::app::MouseEvent &event );

    // Child Manipulation
    //! add a Node as a child; will receive connect/disconnect events and have its locus parented
    void            appendChild( NodeRef element );
    void            insertChildAt( NodeRef element, size_t pos );
    size_t          numChildren() const { return mChildren.size(); }
    NodeRef         getChildAt( size_t index ){ return mChildren.at( index ); }
    void            setChildIndex( NodeRef child, size_t index );

    void            deepDraw();
    //! draw content to screen, called after model matrix is transformed by locus
    virtual void    draw() {}
    //! called in deepdraw before drawing children
    virtual void    preChildDraw() {}
    //! called in deepdraw after drawing children
    virtual void    postChildDraw() {}
    //! block all stored signals, stopping UI events from entering tree
    void            blockRoot() { mConnectionManager.block(); }
    //! unblock all stored signals, stopping UI events from entering tree
    void            unblockRoot() { mConnectionManager.resume(); }
    //! connects to mouse and touch events
    void            connectRoot( ci::app::WindowRef window );
    //! disconnects all stored connections
    void            disconnectRoot();
    //! stop whatever event-related tracking this object was doing
    virtual void    cancelInteractions() {}
    void            deepCancelInteractions();
    //! set top-left of element
    void            setPosition( const ci::Vec2f &pos ){ mLocus.position = pos; }
    ci::Vec2f       getPosition() const { return mLocus.getPosition(); }
    //! set xy scale of element
    void            setScale( const ci::Vec2f &scale ){ mLocus.scale = scale; }
    ci::Vec2f       getScale() const { return mLocus.getScale(); }
    //! set rotation around z-axis
    void            setRotation( float radians ){ mLocus.rotation = radians; }
    void            setRegistrationPoint( const ci::Vec2f &loc ){ mLocus.registration_point = loc; }
    pk::Locus2D&    getLocus(){ return mLocus; }
    //! returns this node's parent transform, as transformed by its parents
    ci::MatrixAffine2f  getFullTransform() const;

    //! called when a child is added to this Node
    virtual void    childAdded( NodeRef element ){}
    void            removeChild( NodeRef element );
    Node*           getParent(){ return mParent; }

    //! return child vector, allowing manipulation of each child, but not the vector
    const std::vector<NodeRef>& getChildren() const { return mChildren; }
  protected:
    // noop default implementations of interaction events
    // return true to indicate you handled the event and stop propagation
    // in general, only begin/end events will be captured by a single object
    virtual bool    touchesBegan( ci::app::TouchEvent &event ) { return false; }
    virtual bool    touchesMoved( ci::app::TouchEvent &event ) { return false; }
    virtual bool    touchesEnded( ci::app::TouchEvent &event ) { return false; }
    virtual bool    mouseDown( ci::app::MouseEvent &event ) { return false; }
    virtual bool    mouseDrag( ci::app::MouseEvent &event ) { return false; }
    virtual bool    mouseUp( ci::app::MouseEvent &event ) { return false; }
    //! store a connection so it can be blocked/unblocked/disconnected later
    void            storeConnection( const ci::signals::connection &connection ){ mConnectionManager.store( connection ); }
  private:
    pk::Locus2D             mLocus;
    Node*                   mParent;
    std::vector<NodeRef>    mChildren;
    ConnectionManager       mConnectionManager;
    void setParent( Node *parent );
  };
  } // cobweb::
} // pockets::
