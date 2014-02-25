
/*
 * Copyright (c) 2013, David Wicks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#include "Pockets.h"

namespace pockets
{

/**
 OpenGLTimer:

 Simplifies the task of asynchronously querying OpenGL for timing information.
 Call begin() and end() before and after the blocks you want to measure.
 Call getMilliseconds() to know how much time elapsed between begin() and end().
 Note that getMilliseconds() does not block, and will return the last time queried
 from OpenGL when new query results are not ready.

 The timer's begin() and end() functions won't restart timing the section of
 interest until you have retrieved the latest timing value using getMilliseconds()
 or getMillisecondsBlocking().

 Basic usage:
 timer.setup(); // initialize OpenGL QueryObject id
 ...
 timer.begin(); // start timing section
 // issue some OpenGL commands
 timer.end(); // end timing section
 double ms = timer.getMilliseconds();

 To get some information on what exactly this is abstracting, see:
 http://www.opengl.org/wiki/Query_Object
 */
class OpenGLTimer
{
public:
  OpenGLTimer();
  ~OpenGLTimer();
  //! initialize OpenGL state of timer
  void setup();

  //! mark the beginning of the OpenGL commands you want to time
  void begin();
  //! mark the end of the OpenGL commands you want to time
  void end();

  // returns true if OpenGL query result is ready
  bool available();

  //! returns latest query value in milliseconds
  //! returns zero until the first query result is ready
  //! if available(), queries timing value and reenables begin/end
  double getMilliseconds();

  //! returns milliseconds elapsed for OpenGL commands between begin/end
  //! blocks until available(), queries timing value and reenables begin/end
  double getMillisecondsBlocking();
private:
  GLuint    mId;      // our opengl query object handle
  bool      mWaiting; // are we waiting for a result
  uint64_t  mValue;   // time elapsed in nanoseconds
};

} // pockets::
