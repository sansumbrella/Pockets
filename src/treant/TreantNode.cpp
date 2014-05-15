//
//  TreantNode.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TreantNode.h"
#include "pockets/CollectionUtilities.hpp"
#include "cinder/gl/gl.h"

using namespace std;
using namespace cinder;
using namespace pockets;

namespace treant
{

TreantNode::TreantNode( const Entity &entity ):
  mEntity( entity )
{
  mTransform = mEntity.assign<LocationComponent>();
  mSize = mEntity.assign<SizeComponent>();
}

TreantNode::~TreantNode()
{
  for( auto &child : mChildren )
  {
    child->mParent = nullptr;
  }
  mEntity.destroy();
}

void TreantNode::appendChild( TreantNodeRef element )
{
  insertChildAt( element, mChildren.size() );
}

void TreantNode::insertChildAt( TreantNodeRef child, size_t index )
{
  child->setParent( this );
  mChildren.insert( mChildren.begin() + index, child );
  childAdded( child );
}

void TreantNode::setChildIndex( TreantNodeRef child, size_t index )
{
  vector_remove( &mChildren, child );
  index = math<int32_t>::min( index, mChildren.size() );
  mChildren.insert( mChildren.begin() + index, child );
}

void TreantNode::removeChild( TreantNodeRef element )
{
  vector_remove( &mChildren, element );
  element->mParent = nullptr;
}

void TreantNode::removeChild( TreantNode *element )
{
  vector_erase_if( &mChildren, [element]( TreantNodeRef &n ){ return n.get() == element; } );
  element->mParent = nullptr;
}

void TreantNode::clearChildren()
{
	for( auto &child : mChildren ) {
		child->mParent = nullptr;
	}
	mChildren.clear();
}

void TreantNode::setParent( TreantNode *parent )
{
  if( mParent && mParent != parent )
  { mParent->removeChild( this ); }
  mParent = parent;
}

bool TreantNode::deepTouchesBegan( ci::app::TouchEvent &event )
{
  bool captured = touchesBegan( event );
  for( TreantNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepTouchesBegan( event );
  }
  return captured;
}

bool TreantNode::deepTouchesMoved( ci::app::TouchEvent &event )
{
  bool captured = touchesMoved( event );
  for( TreantNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepTouchesMoved( event );
  }
  return captured;
}

bool TreantNode::deepTouchesEnded( ci::app::TouchEvent &event )
{
  bool captured = touchesEnded( event );
  for( TreantNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepTouchesEnded( event );
  }
  return captured;
}

bool TreantNode::deepMouseDown( ci::app::MouseEvent &event )
{
  bool captured = mouseDown( event );
  for( TreantNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseDown( event );
  }
  return captured;
}

bool TreantNode::deepMouseDrag( ci::app::MouseEvent &event )
{
  bool captured = mouseDrag( event );
  for( TreantNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseDrag( event );
  }
  return captured;
}

bool TreantNode::deepMouseUp( ci::app::MouseEvent &event )
{
  bool captured = mouseUp( event );
  for( TreantNodeRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseUp( event );
  }
  return captured;
}

void TreantNode::updateTree( const ci::MatrixAffine2f &matrix )
{
  mTransform->updateMatrix( matrix );
  for( TreantNodeRef &child : mChildren ) {
    child->updateTree( mTransform->matrix );
  }
}

void TreantNode::deepCancelInteractions()
{
  cancelInteractions();
  for( TreantNodeRef &child : mChildren ) {
    child->deepCancelInteractions();
  }
}

} // treant::
