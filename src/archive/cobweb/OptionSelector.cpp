//
//  OptionSelector.cpp
//  WordShift
//
//  Created by David Wicks on 3/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "OptionSelector.h"
#include "cinder/ip/Fill.h"

using namespace pockets;
using namespace cinder;
using namespace std;

OptionSelector::OptionSelector( const pk::Sprite &label ):
mLabel( label )
{
  mLabel.setTint( getAltTextColor() );
  mLabel.getLocus().setParent( getLocus() );
}

OptionSelector::~OptionSelector()
{}

void OptionSelector::connect(ci::app::WindowRef window)
{
  storeConnection( window->getSignalTouchesBegan().connect( [this]( app::TouchEvent &event ){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this]( app::TouchEvent &event ){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this]( app::TouchEvent &event ){ touchesEnded( event ); } ) );
}

void OptionSelector::addOption( boost::any value, const pk::Sprite &icon, bool with_backing )
{
  mOptions.push_back( Option( value, icon ) );
  Option& opt = mOptions.back();
  opt.mIcon.setTint( getBackingColor() );
  opt.mIcon.getLocus().setParent( getLocus() );
  opt.mDrawBacking = with_backing;
  mSelection = mOptions.begin();
}

void OptionSelector::setDefaultOptionIndex(uint32_t index)
{
  setSelection( mOptions.begin() + ci::math<uint32_t>::min( index, mOptions.size() - 1 ) );
}

void OptionSelector::layoutHorizontallyWithPadding(float padding, float padding_y, float left_edge, float wrap_width, Layout layout_style )
{
  const float start_x = left_edge;
  float start_y = 0;
  if( layout_style == HEADER_STACKED )
  {
    start_y = mLabel.getSize().y + mOptions.front().mIcon.getRegistrationPoint().y + padding_y;
  }

  float x = start_x;
  float y = start_y;
  float bottom_y = 0;
  for( auto &option : mOptions )
  {
    float x_inc = padding + option.mIcon.getSize().x;
    if( x + x_inc >= wrap_width )
    {
      x = start_x;
      y = bottom_y + padding_y; // bottom_y is the bottom edge of the current lowest element
    }
    option.mIcon.setLoc( vec2( x, y ) );
    x += x_inc;
    bottom_y = math<float>::max( y + option.mIcon.getSize().y - option.mIcon.getRegistrationPoint().y, bottom_y );
  }
  setWidth( x );
  setHeight( bottom_y );
}

bool OptionSelector::Option::contains(const ci::vec2 &point)
{
  return mIcon.contains( point );
}

void OptionSelector::touchesBegan(ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    for( auto iter = mOptions.begin(); iter != mOptions.end(); ++iter )
    {
      if( iter->contains( touch.getPos() ) )
      {
        mTrackedTouch = touch.getId();
        setSelection( iter );
        break;
      }
    }
  }
}

void OptionSelector::touchesMoved(ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    if( touch.getId() == mTrackedTouch )
    {
      for( auto iter = mOptions.begin(); iter != mOptions.end(); ++iter )
      {
        if( iter->contains( touch.getPos() ) )
        {
          setSelection( iter );
          break;
        }
      }
    }
  }
}

void OptionSelector::touchesEnded(ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    if( touch.getId() == mTrackedTouch )
    {
      mTrackedTouch = 0;
      for( auto iter = mOptions.begin(); iter != mOptions.end(); ++iter )
      {
        if( iter->contains( touch.getPos() ) )
        {
          setSelection( iter );
          break;
        }
      }
    }
  }
}

void OptionSelector::setSelection(std::vector<Option>::iterator selection)
{
  if( selection != mSelection )
  {
//    Sound().cueSound( SoundStation::eButtonSelect, 0.0f );
  }
  mSelection->mIcon.setTint( getBackingColor() );
  mSelection = selection;
  mSelection->mIcon.setTint( getBackingHighlightColor() );
}

void OptionSelector::draw()
{
  mLabel.render();
  for( Option &opt : mOptions )
  {
    gl::pushModelView();
    gl::multModelView( opt.mIcon.getLocus() );
    gl::color( opt.mIcon.getTint() );
    if( opt.mDrawBacking )
    {
      gl::disable( GL_TEXTURE_2D );
      gl::drawSolidRect( opt.mIcon.getLocalBounds() );
      gl::enable( GL_TEXTURE_2D );
      gl::color( getTextColor() );
    }
    opt.mIcon.draw();
    gl::popModelView();
  }
}

