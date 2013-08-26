//
//  OptionSelector.h
//  WordShift
//
//  Created by David Wicks on 3/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "pockets/Sprite.h"
#include "Node.h"
#include <boost/any.hpp>

namespace cascade
{
  /**
   Allows user to select one of a number of options.
   Uses boost::any to allow Option values to be of arbitrary type.
   All option values within a given OptionSelector must be of the same type.

   You need to be careful when storing built-in types to make sure you cast back
   to the exact type stored, e.g.:
   addOption( 2, "two" ), getValue<int>() // works
   addOption( 2, "two" ), getValue<double>() // throws bad_any_cast exception
   If you try to cast to a different type other than exactly what is stored with
   the getValue method, it will fail with a boost::bad_any_cast exception.
   Get the value out, then convert it to the type you want either through
   assignment or casting.

   If one option's value needs to mean different things to different consumers,
   consider using a struct that contains all meanings or an int that is mapped
   to the appropriate value in each consumer.

   Use a dynamically created SpriteSheet to hold all option graphics.
   */
  typedef std::shared_ptr<class OptionSelector> OptionSelectorRef;
	class OptionSelector : public Node
	{
	public:
    enum Layout
    {
      HEADER_STACKED
      , HEADER_INLINE
    };
    struct Option
    {
      Option( boost::any value, const pk::Sprite &icon ):
      mValue( value )
      , mIcon( icon )
      {}
      bool            contains( const ci::Vec2f &point );
      boost::any      mValue;
      pk::Sprite      mIcon;
      bool            mDrawBacking = false;
    };
		OptionSelector( const pk::Sprite &label );
		~OptionSelector();

    //! returns the current selected option's value cast to desired type
    //! throws boost::bad_any_cast if T doesn't match input type exactly
    template<typename T>
    T               getValue() const { return boost::any_cast<T>( mSelection->mValue ); }
    //! set default option by index; call after adding all options
    void            setDefaultOptionIndex( uint32_t index );
    void            setSelection( std::vector<Option>::iterator selection );
    size_t          numOptions() const { return mOptions.size(); }
    //! add an option to the list; sets default option to beginning of list
    void            addOption( boost::any value, const pk::Sprite &icon, bool with_backing=false );
    //! Render selector to screen
    void            draw();
    //! layout the buttons on screen
    void            layoutHorizontallyWithPadding( float padding, float padding_y, float left_edge, float wrap_width, Layout layout_style );
    //! Connect to window events for user interaction
    void            connect( ci::app::WindowRef window );
    void            touchesBegan( ci::app::TouchEvent &event );
    void            touchesMoved( ci::app::TouchEvent &event );
    void            touchesEnded( ci::app::TouchEvent &event );
	private:
    std::vector<Option>                     mOptions;
    pk::Sprite                              mLabel;
    typename std::vector<Option>::iterator  mSelection;
    uint32_t                                mTrackedTouch = 0;
	};
}
