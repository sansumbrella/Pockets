//
//  TreentNode.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TreentNode.h"
#include "pockets/CollectionUtilities.hpp"
#include "cinder/gl/gl.h"

using namespace std;
using namespace cinder;
using namespace pockets;

namespace treent
{

TreentNode::TreentNode( const Entity &entity ):
  mEntity( entity )
{
  mTransform = mEntity.assign<LocationComponent>();
  mSize = mEntity.assign<SizeComponent>();
}

TreentNode::~TreentNode()
{
  for( auto &child : mChildren )
  {
    child->mParent = nullptr;
  }
  mEntity.destroy();
}

void TreentNode::appendChild( TreentNodeRef element )
{
  insertChildAt( element, mChildren.size() );
}

void TreentNode::insertChildAt( TreentNodeRef child, size_t index )
{
  child->setParent( this );
  mChildren.insert( mChildren.begin() + index, child );
  childAdded( child );
}

void TreentNode::setChildIndex( TreentNodeRef child, size_t index )
{
  vector_remove( &mChildren, child );
  index = math<int32_t>::min( index, mChildren.size() );
  mChildren.insert( mChildren.begin() + index, child );
}

void TreentNode::removeChild( TreentNodeRef element )
{
  vector_remove( &mChildren, element );
  element->mParent = nullptr;
}

void TreentNode::removeChild( TreentNode *element )
{
  vector_erase_if( &mChildren, [element]( TreentNodeRef &n ){ return n.get() == element; } );
  element->mParent = nullptr;
}

void TreentNode::clearChildren()
{
	for( auto &child : mChildren ) {
		child->mParent = nullptr;
	}
	mChildren.clear();
}

void TreentNode::setParent( TreentNode *parent )
{
  if( mParent && mParent != parent )
  { mParent->removeChild( this ); }
  mParent = parent;
}

bool TreentNode::deepTouchesBegan( ci::app::TouchEvent &event )
{
  bool captured = touchesBegan( event );
  for( TreentNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepTouchesBegan( event );
  }
  return captured;
}

bool TreentNode::deepTouchesMoved( ci::app::TouchEvent &event )
{
  bool captured = touchesMoved( event );
  for( TreentNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepTouchesMoved( event );
  }
  return captured;
}

bool TreentNode::deepTouchesEnded( ci::app::TouchEvent &event )
{
  bool captured = touchesEnded( event );
  for( TreentNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepTouchesEnded( event );
  }
  return captured;
}

bool TreentNode::deepMouseDown( ci::app::MouseEvent &event )
{
  bool captured = mouseDown( event );
  for( TreentNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseDown( event );
  }
  return captured;
}

bool TreentNode::deepMouseDrag( ci::app::MouseEvent &event )
{
  bool captured = mouseDrag( event );
  for( TreentNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseDrag( event );
  }
  return captured;
}

bool TreentNode::deepMouseUp( ci::app::MouseEvent &event )
{
  bool captured = mouseUp( event );
  for( TreentNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseUp( event );
  }
  return captured;
}

void TreentNode::updateTree( const ci::MatrixAffine2f &matrix )
{
  mTransform->updateMatrix( matrix );
  for( TreentNodeRef &child : mChildren ) {
    child->updateTree( mTransform->matrix );
  }
}

void TreentNode::deepCancelInteractions()
{
  cancelInteractions();
  for( TreentNodeRef &child : mChildren ) {
    child->deepCancelInteractions();
  }
}

} // treent::
