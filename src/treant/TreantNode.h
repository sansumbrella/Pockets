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
#include "treant/LocationComponent.h"
#include "treant/SizeComponent.h"

#include "Treant.h"

namespace treant
{

typedef std::unique_ptr<class TreantNode> TreantNodeUniqueRef;
/**
 Base TreantNode type in a simple scene graph.
 
 Allows you to compose entities hierarchically.
 Entities allow you to compose components non-hierarchically.
 
 UI propagation happens within tree.
 Transform update happens within tree.
 Rendering happens outside of the tree, in Rendering Systems.
 
 Override TreantNode and add components to your entity in your constructor.
 All TreantNodes have a Location and a Size component.
 Parent locations update their children's overall transform in updateTree().
 Child sizes update their parent's overall size in updateTree()'s return loop.

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

  //! Call to update the entire TreantNode hierarchy.
  void            updateTree( const ci::MatrixAffine2f &matrix );

  //
  // Mirror Entity interface
  //

  //! Assign a component.
  template <typename C, typename ... Args>
  std::shared_ptr<C> assign(Args && ... args) { return mEntity.assign<C>( std::forward<Args>(args) ... ); }
  //! Remove a component.
  template <typename C>
  void remove() { mEntity.remove<C>(); }
  //! Get a component.
  template <typename C>
  std::shared_ptr<C> component() { return mEntity.component<C>(); }
  //! Retrieve a bunch of components at once.
  template <typename A, typename ... Args>
  void unpack(std::shared_ptr<A> &a, std::shared_ptr<Args> & ... args) { mEntity.unpack( a, std::forward<Args>(args) ... ); }

  //
  //  Child creation (automatically creates entity)
  //

  //! Create a child and add it to our hierarchy.
  //! The entity argument is automatically constructed.
  //! Pass any additional constructor parameters through as args.
  template<typename T, typename ...Args>
  std::shared_ptr<T>  createChild( Args & ... args );

  template<typename T, typename Arg, typename ...Args>
  std::shared_ptr<T>  createChild( Arg &&arg, Args && ... args );

  // Child Manipulation
  //! add a TreantNode as a child; will receive connect/disconnect events and have its locus parented
  void            appendChild( TreantNodeRef element );
  void            insertChildAt( TreantNodeRef element, size_t pos );
  size_t          numChildren() const { return mChildren.size(); }
  TreantNodeRef   getChildAt( size_t index ){ return mChildren.at( index ); }
  void            setChildIndex( TreantNodeRef child, size_t index );

  void            removeChild( TreantNodeRef element );
  void            removeChild( TreantNode *element );

  //! Removes all children.
  void            clearChildren();

  //! Stop whatever event-related tracking this object was doing. Considering for removal
  virtual void    cancelInteractions() {}
  void            deepCancelInteractions();

  //! Set top-left of element.
  void            setPosition( const ci::Vec2f &pos ){ mTransform->position = pos; }
  //! Get top-left of element.
  ci::Vec2f       getPosition() const { return mTransform->position; }
  //! Set xy scale of element.
  void            setScale( const ci::Vec2f &scale ){ mTransform->scale = scale; }
  ci::Vec2f       getScale() const { return mTransform->scale; }
  //! Set element rotation around z-axis.
  void            setRotation( float radians ){ mTransform->rotation = radians; }
  //! Set registration point for rotation and scaling.
  void            setRegistrationPoint( const ci::Vec2f &loc ){ mTransform->registration_point = loc; }

  //! Returns nominal width and height. Up to users to set correctly at this point.
  ci::Vec2f       getSize() const { return mSize->size; }
  void            setSize( const ci::Vec2f &size ) { mSize->size = size; }

  //! Returns this TreantNode's transform, as transformed by its parents.
  ci::MatrixAffine2f    getFullTransform() const { return mTransform->matrix; }
  //! Returns this TreantNode's transform, ignoring parent transformations.
  ci::MatrixAffine2f    getLocalTransform() const { return mTransform->calcLocalMatrix(); }

  LocationComponentRef  getTransform() const { return mTransform; }

  //! called when a child is added to this TreantNode
  virtual void    childAdded( TreantNodeRef element ){}
  TreantNode*     getParent(){ return mParent; }

  //! return child vector, allowing manipulation of each child, but not the vector
  const std::vector<TreantNodeRef>& getChildren() const { return mChildren; }
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

  Entity                      mEntity;
  LocationComponentRef        mTransform;
  SizeComponentRef            mSize;
private:
  TreantNode*                 mParent = nullptr;
  std::vector<TreantNodeRef>  mChildren;

  //! Sets the TreantNode's parent, notifying previous parent (if any)
  void            setParent( TreantNode *parent );
};


template<typename T, typename ...Args>
std::shared_ptr<T>  TreantNode::createChild( Args & ... args )
{
	auto manager = mEntity.manager_.lock( );
	auto child = std::make_shared<T>( manager->create( ), std::forward<Args>( args ) ... );
	appendChild( child );
	return child;
}


template<typename T, typename Arg, typename ... Args>
std::shared_ptr<T>  TreantNode::createChild( Arg && arg, Args && ... args )
{
	auto manager = mEntity.manager_.lock( );
	auto child = std::make_shared<T>( manager->create(), arg, std::forward<Args>( args ) ... );
	appendChild( child );
	return child;
}

} // treant::
