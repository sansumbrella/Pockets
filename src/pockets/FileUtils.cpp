
#include "FileUtils.h"

std::string getUniquePath( const fs::path &path, const std::string &sep, int padding, bool numberFirstFile )
{
	fs::path p( path );
	string extension = p.extension().string();
	string stem = p.stem().string();
	fs::path parent_path = p.parent_path();

	int count = 0;
	if( numberFirstFile )
		p = parent_path / ( stem + sep + leftPaddedString( toString(count++), padding ) + extension );

	while ( fs::exists( p ) )
	{
		p = parent_path / ( stem + sep + leftPaddedString( toString(count++), padding ) + extension );
	}

	return p.generic_string();
}

std::string leftPaddedString( const std::string &input, int minSize, const std::string pad )
{
	std::string output(input);
	while ( output.size() < minSize )
	{
		output = pad + output;
	}
	return output;
}
