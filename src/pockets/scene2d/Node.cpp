//
//  Node.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Node.h"
#include "CollectionUtilities.hpp"

using namespace cinder;
using namespace pockets;
using namespace std;

Node::Node():
mSize( 0, 0 )
{}

Node::Node( const Vec2f &size ):
mSize( size )
{}

void Node::appendChild(NodeRef element)
{
  NodeRef former_parent = element->getParent();
  if( former_parent ){ former_parent->removeChild( element ); }
  element->setParent( shared_from_this() );
  mChildren.push_back( element );
  childAdded( element );
}

void Node::removeChild(NodeRef element)
{
  vector_remove( &mChildren, element );
  element->setParent( NodeWeakRef() );
}

void Node::setParent(NodeWeakRef parent)
{
  mParent = parent;
  NodeRef p = mParent.lock();
  if( p )
  {
    mLocus->setParent( p->getLocus() );
  }
  else
  {
    mLocus->unsetParent();
  }
}

void Node::disconnect()
{
  for( signals::connection &connect : mConnections )
  {
    connect.disconnect();
  }
  mConnections.clear();
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
  for( signals::connection &connect : mConnections )
  {
    mBlocks.push_back( signals::shared_connection_block( connect ) );
  }
}

void Node::unblock()
{
  mBlocks.clear();
  customUnblock();
}

void Node::setChildIndex(NodeRef child, size_t index)
{
  vector_remove( &mChildren, child );
  index = math<int32_t>::min( index, mChildren.size() );
  mChildren.insert( mChildren.begin() + index, child );
}

int Node::totalHeight() const
{
  return math<int>::max( getHeight(), childHeight() );
}

int Node::childHeight() const
{
  int top = 0;
  int bottom = 0;
  for( auto &child : mChildren )
  {
    int y = child->getLoc().y;
    top = math<int>::min( y, top );
    bottom = math<int>::max( y + child->getHeight(), bottom );
  }

  return bottom - top;
}
















