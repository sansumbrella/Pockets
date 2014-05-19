//
//  TreentNode.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/app/App.h"
#include "pockets/ConnectionManager.h"
#include "treent/LocationComponent.h"
#include "treent/SizeComponent.h"

#include "Treent.h"

namespace treent
{

typedef std::unique_ptr<class TreentNode> TreentNodeUniqueRef;

/**
 Base TreentNode type in a simple scene graph.

 Allows you to compose entities hierarchically.
 Entities allow you to compose components non-hierarchically.

 UI propagation happens within tree.
 Transform update happens within tree.
 Rendering happens outside of the tree, in Rendering Systems.

 Override TreentNode and add components to your entity in your constructor.
 All TreentNodes have a Location and a Size component.
 Parent locations update their children's overall transform in updateTree().
 Child sizes update their parent's overall size in updateTree()'s return loop.

 TreentNodes are connected in a tree, with a single root TreentNode connecting to
 window UI events and propagating them to all of its children.
 */
class TreentNode
{
public:
  //! construct an empty TreentNode
  explicit TreentNode( const Entity &entity );
  virtual ~TreentNode();
  // Mouse and touch interaction
  bool            deepTouchesBegan( ci::app::TouchEvent &event );
  bool            deepTouchesMoved( ci::app::TouchEvent &event );
  bool            deepTouchesEnded( ci::app::TouchEvent &event );
  bool            deepMouseDown( ci::app::MouseEvent &event );
  bool            deepMouseDrag( ci::app::MouseEvent &event );
  bool            deepMouseUp( ci::app::MouseEvent &event );

  //! Call to update the entire TreentNode hierarchy.
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

  TreentNodeRef       createChild() { return createChild<TreentNode>(); }

  // Child Manipulation
  //! add a TreentNode as a child; will receive connect/disconnect events and have its locus parented
  void            appendChild( TreentNodeRef element );
  void            insertChildAt( TreentNodeRef element, size_t pos );
  size_t          numChildren() const { return mChildren.size(); }
  TreentNodeRef   getChildAt( size_t index ){ return mChildren.at( index ); }
  void            setChildIndex( TreentNodeRef child, size_t index );

  void            removeChild( TreentNodeRef element );
  void            removeChild( TreentNode *element );

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

  //! Returns this TreentNode's world transform, as transformed by its parents.
  ci::MatrixAffine2f    getWorldTransform() const { return mTransform->matrix; }
  //! Returns this TreentNode's transform, ignoring parent transformations.
  ci::MatrixAffine2f    getLocalTransform() const { return mTransform->calcLocalMatrix(); }

  LocationComponentRef  getTransform() const { return mTransform; }

  //! called when a child is added to this TreentNode
  virtual void    childAdded( TreentNodeRef element ){}
  TreentNode*     getParent(){ return mParent; }

  //! return child vector, allowing manipulation of each child, but not the vector
  const std::vector<TreentNodeRef>& getChildren() const { return mChildren; }
protected:

  Entity                      mEntity;
  LocationComponentRef        mTransform;
  SizeComponentRef            mSize;
private:
  TreentNode*                 mParent = nullptr;
  std::vector<TreentNodeRef>  mChildren;

  //! Sets the TreentNode's parent, notifying previous parent (if any)
  void            setParent( TreentNode *parent );
};

class RootNode : public TreentNode
{
public:
  RootNode( Entity entity ) : TreentNode( entity ) {}

  //! Connects tree to mouse and touch events.
  void            connect( ci::app::WindowRef window );
  //! Disconnects tree from mouse and touch events.
  void            disconnect() { _connection_manager.disconnect(); }
  //! Temporarily block UI signals.
  void            block() { _connection_manager.block(); }
  //! Resume receiving UI signals.
  void            unblock() { _connection_manager.resume(); }
private:
  //! store a connection so it can be blocked/unblocked/disconnected later
  void            storeConnection( const ci::signals::connection &connection ){ _connection_manager.store( connection ); }
  pockets::ConnectionManager   _connection_manager;
};

//
//  MARK: - Child creation implementations.
//

template<typename T, typename ...Args>
std::shared_ptr<T>  TreentNode::createChild( Args & ... args )
{
	auto manager = mEntity.manager_.lock( );
	auto child = std::make_shared<T>( manager->create( ), std::forward<Args>( args ) ... );
	appendChild( child );
	return child;
}


template<typename T, typename Arg, typename ... Args>
std::shared_ptr<T>  TreentNode::createChild( Arg && arg, Args && ... args )
{
	auto manager = mEntity.manager_.lock( );
	auto child = std::make_shared<T>( manager->create(), arg, std::forward<Args>( args ) ... );
	appendChild( child );
	return child;
}

} // treent::
