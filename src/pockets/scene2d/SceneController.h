//
//  SceneController.h
//  WordShift
//
//  Created by David Wicks on 2/21/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once


namespace cascade
{
  /**
   A basic application layer encapsulating input and view controls

   Renderable
   Updatable, pausable (always calls its own update on app signal unless paused)
   Listens for UI events

   Eventually, would like to make a simple scene-graph-like setup of:
   Connectable -> can connect itself to window stuff
   Renderable -> can be drawn to screen
   Maybe Runnable -> can/expects to be updated at 60Hz

   Scene graph would be useful primarily for laying out a simple GUI.
   Simple scene graph is implemented as Nodes in scene2d.
   */
  typedef std::shared_ptr<class SceneController> SceneControllerRef;
	class SceneController
	{
	public:
    typedef std::function<void ()> Callback;
    virtual ~SceneController();
    //! connect to receive user interaction events (of subclasses' choosing)
    //! Consider storing reference to the window for passing into potential subviews
    virtual void  connect( ci::app::WindowRef window ){}
    //! stop receiving window UI events
    void          disconnect(){ breakStoredConnections(); customDisconnect(); }
    //! disconnect any custom connections (non-stored ones)
    virtual void  customDisconnect(){}
    //! temporarily stop receiving UI events; useful if we want to present a different UI for a moment
    void  block();
    //! resume receiving UI events (if we were connected before block())
    void  unblock();
    //! temporarily freeze updates
    void  pause();
    virtual void customPause(){}
    //! continue receiving updates
    void  resume();
    virtual void customResume(){}
    //! update content
    virtual void  update(){}
    //! render content
    virtual void  draw(){}
    //! adds this view to the specified window; override appear to control animation
    void  show( ci::app::WindowRef window, bool useWindowBounds=true );
    //! remove this view from render sequence; override vanish to control animation
    void  hide( Callback finishFn=0 ){ vanish( vanishCompleteFn(finishFn) ); }
    //! returns the bounds of the controller in points
    ci::Area     getBounds() const { return mBounds; }
    //! set the region of screen into which we should draw this view
    void         setBounds( const ci::Area &points ){ mBounds = points; }
    //! store a connection so it can be blocked/unblocked/disconnected later
    void         storeConnection( const ci::signals::connection &connection ){ mUIConnections.push_back( connection ); }
    void         breakStoredConnections();
  protected:
    SceneController();
    //! called by show after adding view to widow
    virtual void appear(){}
    //! called by hide before removing from window; user must call removeFromDisplay when finished animating
    virtual void vanish( Callback finishFn ){ finishFn(); }
	private:
    ci::Area                                          mBounds;
    //! UI connections
    std::vector<ci::signals::connection>              mUIConnections;
    //! window update/display connection
    ci::signals::connection                           mDisplayConnection;
    ci::signals::connection                           mUpdateConnection;
    std::vector<ci::signals::shared_connection_block> mBlocks;
    ci::signals::shared_connection_block              mUpdateBlock;
    bool                                              mGameCenterPlayerValidated = false;
    Callback  vanishCompleteFn( Callback finishFn );
    void      removeFromDisplay(){ mDisplayConnection.disconnect(); }
	};
}
