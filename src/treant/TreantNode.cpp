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
  _entity( entity )
{
  _transform = _entity.assign<Location>();
  _size = _entity.assign<SizeComponent>();
}

TreantNode::~TreantNode()
{
  for( auto &child : _children )
  {
    child->_parent = nullptr;
  }
  _entity.destroy();
}

void TreantNode::appendChild( TreantNodeRef element )
{
  insertChildAt( element, _children.size() );
}

void TreantNode::insertChildAt( TreantNodeRef child, size_t index )
{
  child->setParent( this );
  _children.insert( _children.begin() + index, child );
  childAdded( child );
}

void TreantNode::setChildIndex( TreantNodeRef child, size_t index )
{
  vector_remove( &_children, child );
  index = math<int32_t>::min( index, _children.size() );
  _children.insert( _children.begin() + index, child );
}

void TreantNode::removeChild( TreantNodeRef element )
{
  vector_remove( &_children, element );
  element->_parent = nullptr;
}

void TreantNode::removeChild( TreantNode *element )
{
  vector_erase_if( &_children, [element]( TreantNodeRef &n ){ return n.get() == element; } );
  element->_parent = nullptr;
}

void TreantNode::setParent( TreantNode *parent )
{
  if( _parent && _parent != parent )
  { _parent->removeChild( this ); }
  _parent = parent;
}

bool TreantNode::deepTouchesBegan( ci::app::TouchEvent &event )
{
  bool captured = touchesBegan( event );
  for( TreantNodeRef &child : _children )
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
  for( TreantNodeRef &child : _children )
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
  for( TreantNodeRef &child : _children )
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
  for( TreantNodeRef &child : _children )
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
  for( TreantNodeRef &child : _children )
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
  for( TreantNodeRef &child : _children )
  { // stop evaluation if event was captured by self or a child
    if( captured )
    { break; }
    captured = child->deepMouseUp( event );
  }
  return captured;
}

void TreantNode::updateTree( const ci::MatrixAffine2f &matrix )
{
  _transform->updateMatrix( matrix );
  for( TreantNodeRef &child : _children ) {
    child->updateTree( _transform->matrix );
  }
}

void TreantNode::deepCancelInteractions()
{
  cancelInteractions();
  for( TreantNodeRef &child : _children ) {
    child->deepCancelInteractions();
  }
}

} // treant::
