//
//  Node.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Node.h"
#include "pockets/CollectionUtilities.hpp"

using namespace std;
using namespace cinder;
using namespace pockets;
using namespace cobweb;

NodeUniqueRef Node::create()
{
  return NodeUniqueRef( new Node() );
}

Node::~Node()
{
  for( auto &child : mChildren )
  {
    child->setParent( nullptr );
  }
}

void Node::appendChild(NodeRef element)
{
  Node *former_parent = element->getParent();
  if( former_parent ) // remove child from parent (but skip notifying child)
  { vector_remove( &former_parent->mChildren, element ); }
  element->setParent( this );
  mChildren.push_back( element );
  childAdded( element );
}

void Node::removeChild(NodeRef element)
{
  vector_remove( &mChildren, element );
  element->setParent( nullptr );
}

void Node::setParent( Node *parent )
{
  mParent = parent;
  if( parent )
  {
    mLocus->parent = parent->getLocus();
  }
  else
  {
    mLocus->detachFromParent();
  }
}

void Node::disconnect()
{
  mConnectionManager.disconnect();
  customDisconnect();
}

void Node::deepDraw()
{
  draw();
  preChildDraw();
  for( NodeRef &child : mChildren )
  {
    child->deepDraw();
  }
  postChildDraw();
}

void Node::deepCancelInteractions()
{
  cancelInteractions();
  for( NodeRef &child : mChildren )
  {
    child->deepCancelInteractions();
  }
}

void Node::deepConnect(ci::app::WindowRef window)
{
  connect( window );
  for( NodeRef &child : mChildren )
  {
    child->deepConnect( window );
  }
}

void Node::deepDisconnect()
{
  disconnect();
  for( NodeRef &child : mChildren )
  {
    child->deepDisconnect();
  }
}

void Node::blockChildren()
{
  for( NodeRef &child : mChildren )
  {
    child->block();
  }
}

void Node::unblockChildren()
{
  for( NodeRef &child : mChildren )
  {
    child->unblock();
  }
}

void Node::block()
{
  customBlock();
  mConnectionManager.block();
}

void Node::unblock()
{
  mConnectionManager.resume();
  customUnblock();
}

void Node::setChildIndex(NodeRef child, size_t index)
{
  vector_remove( &mChildren, child );
  index = math<int32_t>::min( index, mChildren.size() );
  mChildren.insert( mChildren.begin() + index, child );
}
