//
//  TreantNode.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "pockets/Locus.h"
#include "pockets/ConnectionManager.h"
#include "cinder/app/App.h"

#include "entityx.h"

namespace treant
{
  using namespace entityx;
}

namespace treant
{

typedef std::shared_ptr<class TreantNode> TreantNodeRef;
typedef std::unique_ptr<class TreantNode> TreantNodeUniqueRef;
/**
 Base TreantNode type in a simple scene graph.
 
 Allows you to compose entities hierarchically.
 Entities allow you to compose components non-hierarchically.
 
 UI propagation happens within tree.
 Transform update happens within tree.
 Rendering happens outside of the tree, in Rendering Systems.

 TreantNodes are connected in a tree, with a single root TreantNode connecting to
 window UI events and propagating them to all of its children.
 */
class TreantNode
{
public:
  //! construct an empty TreantNode
  explicit TreantNode( const Entity &entity );
  virtual ~TreantNode();
  // Mouse and touch interaction
  bool            deepTouchesBegan( ci::app::TouchEvent &event );
  bool            deepTouchesMoved( ci::app::TouchEvent &event );
  bool            deepTouchesEnded( ci::app::TouchEvent &event );
  bool            deepMouseDown( ci::app::MouseEvent &event );
  bool            deepMouseDrag( ci::app::MouseEvent &event );
  bool            deepMouseUp( ci::app::MouseEvent &event );

  void            updateTree();

  template<typename T>
  std::shared_ptr<T>  createChild();
  // Child Manipulation
  //! add a TreantNode as a child; will receive connect/disconnect events and have its locus parented
  void            appendChild( TreantNodeRef element );
  void            insertChildAt( TreantNodeRef element, size_t pos );
  size_t          numChildren() const { return _children.size(); }
  TreantNodeRef   getChildAt( size_t index ){ return _children.at( index ); }
  void            setChildIndex( TreantNodeRef child, size_t index );

  //! Call to draw the entire TreantNode hierarchy.
  void            deepDraw();
  //! Draw content to screen. Called after model matrix is transformed by locus.
  virtual void    draw() {}
  //! called in deepdraw before drawing children
  virtual void    preChildDraw() {}
  //! called in deepdraw after drawing children
  virtual void    postChildDraw() {}
  //! Stop whatever event-related tracking this object was doing. Considering for removal
  virtual void    cancelInteractions() {}
  void            deepCancelInteractions();

  //! Set top-left of element.
  void            setPosition( const ci::Vec2f &pos ){ _locus.position = pos; }
  //! Get top-left of element.
  ci::Vec2f       getPosition() const { return _locus.position; }
  //! Set xy scale of element.
  void            setScale( const ci::Vec2f &scale ){ _locus.scale = scale; }
  ci::Vec2f       getScale() const { return _locus.scale; }
  //! Set element rotation around z-axis.
  void            setRotation( float radians ){ _locus.rotation = radians; }
  //! Set registration point for rotation and scaling.
  void            setRegistrationPoint( const ci::Vec2f &loc ){ _locus.registration_point = loc; }
  //! Returns this TreantNode's locus.
  pk::Locus2D&    getLocus(){ return _locus; }
  //! Returns this TreantNode's transform, as transformed by its parents.
  ci::MatrixAffine2f  getFullTransform() const;
  //! Returns this TreantNode's transform, ignoring parent transformations.
  ci::MatrixAffine2f  getLocalTransform() const { return _locus.toMatrix(); }

  //! called when a child is added to this TreantNode
  virtual void    childAdded( TreantNodeRef element ){}
  void            removeChild( TreantNodeRef element );
  void            removeChild( TreantNode *element );
  TreantNode*           getParent(){ return _parent; }

  //! return child vector, allowing manipulation of each child, but not the vector
  const std::vector<TreantNodeRef>& getChildren() const { return _children; }
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
private:
  Entity                  _entity;
  pk::Locus2D             _locus;
  TreantNode*                   _parent;
  std::vector<TreantNodeRef>    _children;
  //! Sets the TreantNode's parent, notifying previous parent (if any)
  void            setParent( TreantNode *parent );
};

template<typename T>
std::shared_ptr<T> TreantNode::createChild()
{
  auto child = std::make_shared<T>( _entity.manager_->create() );
  appendChild( child );
  return child;
}

} // treant::
