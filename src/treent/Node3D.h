/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
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

#include "cinder/app/App.h"
#include "pockets/ConnectionManager.h"
#include "treent/LocationComponent.h"
#include "treent/SizeComponent.h"

#include "Treent.h"

namespace treent
{

typedef std::unique_ptr<class Node3D> Node3DUniqueRef;
typedef std::shared_ptr<class Node3D> Node3DRef;

/**
 Base Node3D type in a simple scene graph.

 Allows you to compose entities hierarchically.
 Entities allow you to compose components non-hierarchically.

 UI propagation happens within tree.
 Transform update happens within tree.
 Rendering happens outside of the tree, in Rendering Systems.

 Override Node3D and add components to your entity in your constructor.
 All Node3Ds have a Location and a Size component.
 Parent locations update their children's overall transform in updateTree().
 Child sizes update their parent's overall size in updateTree()'s return loop.

 Node3Ds are connected in a tree, with a single root Node3D connecting to
 */
class Node3D
{
public:
  //! construct an empty Node3D
  explicit Node3D( const Entity &entity );
  virtual ~Node3D();

  //! Call to update the entire Node3D hierarchy.
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

  Node3DRef           createChild() { return createChild<Node3D>(); }

  // Child Manipulation
  //! add a Node3D as a child; will receive connect/disconnect events and have its locus parented
  void            appendChild( Node3DRef element );
  void            insertChildAt( Node3DRef element, size_t pos );
  size_t          numChildren() const { return mChildren.size(); }
  Node3DRef       getChildAt( size_t index ){ return mChildren.at( index ); }
  void            setChildIndex( Node3DRef child, size_t index );

  void            removeChild( Node3DRef element );
  void            removeChild( Node3D *element );

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

  //! Returns this Node3D's world transform, as transformed by its parents.
  ci::MatrixAffine2f    getWorldTransform() const { return mTransform->matrix; }
  //! Returns this Node3D's transform, ignoring parent transformations.
  ci::MatrixAffine2f    getLocalTransform() const { return mTransform->calcLocalMatrix(); }

  LocationComponentRef  getTransform() const { return mTransform; }

  //! called when a child is added to this Node3D
  virtual void    childAdded( Node3DRef element ){}
  Node3D*     getParent(){ return mParent; }

  //! return child vector, allowing manipulation of each child, but not the vector
  const std::vector<Node3DRef>& getChildren() const { return mChildren; }
protected:

  Entity                      mEntity;
  Transform3DRef              mTransform;
private:
  Node3D*                     mParent = nullptr;
  std::vector<Node3DRef>      mChildren;

  //! Sets the Node3D's parent, notifying previous parent (if any)
  void            setParent( Node3D *parent );
};

//
//  MARK: - Child creation template implementations.
//

template<typename T, typename ...Args>
std::shared_ptr<T>  Node3D::createChild( Args & ... args )
{
	auto manager = mEntity.manager_.lock();
	auto child = std::make_shared<T>( manager->create(), std::forward<Args>( args ) ... );
	appendChild( child );
	return child;
}


template<typename T, typename Arg, typename ... Args>
std::shared_ptr<T>  Node3D::createChild( Arg && arg, Args && ... args )
{
	auto manager = mEntity.manager_.lock();
	auto child = std::make_shared<T>( manager->create(), arg, std::forward<Args>( args ) ... );
	appendChild( child );
	return child;
}

} // treent::
