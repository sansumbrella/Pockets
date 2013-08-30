//
//  SceneController.cpp
//  WordShift
//
//  Created by David Wicks on 2/21/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SceneController.h"

using namespace pockets;
using namespace cinder;

SceneController::SceneController()
{ // start updating
  mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this](){ update(); } );
}

SceneController::~SceneController()
{ // make sure nothing references us anymore
  mUpdateConnection.disconnect();
  disconnect();
  removeFromDisplay();
}

void SceneController::breakStoredConnections()
{
  for( signals::connection &connection : mUIConnections )
  {
    connection.disconnect();
  }
  mUIConnections.clear();
}

void SceneController::block()
{
  for( signals::connection &connect : mUIConnections )
  {
    mBlocks.push_back( signals::shared_connection_block( connect ) );
  }
}

void SceneController::unblock()
{
  mBlocks.clear();
}

void SceneController::pause()
{
  mUpdateBlock = signals::shared_connection_block( mUpdateConnection );
  customPause();
}

void SceneController::resume()
{
  mUpdateBlock.unblock();
  mUpdateBlock = signals::shared_connection_block();
  customResume();
}

SceneController::Callback SceneController::vanishCompleteFn( SceneController::Callback finishFn)
{
  return [this, finishFn](){ removeFromDisplay(); if( finishFn ){ finishFn(); } };
}

void SceneController::show( app::WindowRef window, bool useWindowBounds )
{
  if( mDisplayConnection.connected() ){ mDisplayConnection.disconnect(); }
  mDisplayConnection = window->getSignalDraw().connect( 1, [this](){ draw(); } );
  if( useWindowBounds ){ setBounds( window->getBounds() ); }
  appear();
}
