

#include "FileUtils.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace std;

std::string pockets::getUniquePath( const fs::path &path, int padding, bool numberFirstFile )
{
	fs::path p( path );
	string extension = p.extension().string();
	string stem = p.stem().string();
	fs::path parent_path = p.parent_path();

	int count = 0;
	if( numberFirstFile )
		p = parent_path / ( stem + leftPaddedString( toString(count++), padding ) + extension );

	while ( fs::exists( p ) )
	{
		p = parent_path / ( stem + leftPaddedString( toString(count++), padding ) + extension );
	}

	return p.generic_string();
}

std::string pockets::leftPaddedString( const std::string &input, int minSize, const std::string pad )
{
	std::string output(input);
	while ( output.size() < minSize )
	{
		output = pad + output;
	}
	return output;
}
