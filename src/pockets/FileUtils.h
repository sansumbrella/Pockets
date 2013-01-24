

#include "Pockets.h"
#include "cinder/Filesystem.h"

#pragma once

namespace pockets
{
//! Returns next numbered pathname based on the provided path in the form some/filename-##.ext
std::string getNextNumberedPath( const ci::fs::path &path, int padding = 2, bool numberFirstFile = true );
//! Returns a left-padded string based on the input string. e.g. ("two", 4, "0") returns "0two", ("four", 4, "0") returns "four"
std::string leftPaddedString( const std::string &input, int minSize, const std::string pad="0" );

} // pockets
