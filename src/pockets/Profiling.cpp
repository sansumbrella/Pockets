
#include "Profiling.h"

using namespace pockets;

OpenGLTimer::OpenGLTimer():
mId( 0 ),
mWaiting( false ),
mValue( 0 )
{}
//! initialize OpenGL state of timer
void setup()
{
  glGenQueries( 1, &mId );
}
OpenGLTimer::~OpenGLTimer()
{
  glDeleteQueries( 1, &mId );
}
//! mark the beginning of the OpenGL commands you want to time
void OpenGLTimer::begin()
{
  if( !mWaiting )
  { glBeginQuery( GL_TIME_ELAPSED, mId ); }
}
//! mark the end of the OpenGL commands you want to time
void OpenGLTimer::end()
{
  if( !mWaiting )
  {
    glEndQuery( GL_TIME_ELAPSED );
    mWaiting = true;
  }
}

// returns true if OpenGL query result is ready
bool OpenGLTimer::available()
{
  int a = 0;
  glGetQueryObjectiv( mId, GL_QUERY_RESULT_AVAILABLE, &a );
  return a == GL_TRUE;
}

//! returns the latest valid timing information in milliseconds
//! updates with new data if available
double OpenGLTimer::getMilliseconds()
{
  if( available() )
  {
    glGetQueryObjectui64v( mId, GL_QUERY_RESULT, &mValue );
    mWaiting = false;
  }
  return mValue * 1.0e-6;
}

//! blocks until OpenGL timing information is ready and returns milliseconds
double OpenGLTimer::getMillisecondsBlocking()
{
  bool ready = false;
  while( !ready )
  { ready = available(); }
  mWaiting = false;
  glGetQueryObjectui64v( mId, GL_QUERY_RESULT, &mValue );
  return mValue * 1.0e-6;
}
