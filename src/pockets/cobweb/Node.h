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
    //! Block UI signals. Only works on root node.
    void            blockRoot() { mConnectionManager.block(); }
    //! Resume receiving UI signals. Only works on root node.
    void            unblockRoot() { mConnectionManager.resume(); }
    //! Connects tree to mouse and touch events. Only call on your root node.
    void            connectRoot( ci::app::WindowRef window );
    //! Disconnects tree from mouse and touch events. Only call on your root node.
    void            disconnectRoot();
    //! Stop whatever event-related tracking this object was doing. Considering for removal
    virtual void    cancelInteractions() {}
    void            deepCancelInteractions();
    //! Set top-left of element.
    void            setPosition( const ci::Vec2f &pos ){ mLocus.position = pos; }
    //! Get top-left of element.
    ci::Vec2f       getPosition() const { return mLocus.getPosition(); }
    //! Set xy scale of element.
    void            setScale( const ci::Vec2f &scale ){ mLocus.scale = scale; }
    ci::Vec2f       getScale() const { return mLocus.getScale(); }
    //! Set element rotation around z-axis.
    void            setRotation( float radians ){ mLocus.rotation = radians; }
    //! Set registration point for rotation and scaling.
    void            setRegistrationPoint( const ci::Vec2f &loc ){ mLocus.registration_point = loc; }
    //! Returns this node's locus for.
    pk::Locus2D&    getLocus(){ return mLocus; }
    //! Returns this node's transform, as transformed by its parents.
    ci::MatrixAffine2f  getFullTransform() const;
    //! Returns this node's transform, ignoring parent transformations.
    ci::MatrixAffine2f  getLocalTransform() const { return mLocus.toMatrix(); }

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
