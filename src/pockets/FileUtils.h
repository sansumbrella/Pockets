

#include "Pockets.h"
#include "cinder/Filesystem.h"

namespace cinder { namespace pockets
{
//! Returns unique, numbered pathname based on the provided path in the form some/filename-##.ext
std::string getUniquePath( const fs::path &path, const std::string &sep = "-", int padding = 2, bool numberFirstFile = true );
//! Returns a left-padded string based on the input string. e.g. ("two", 4, "0") returns "0two", ("four", 4, "0") returns "four"
std::string leftPaddedString( const std::string &input, int minSize, const std::string pad="0" );

}} // cinder::pockets
