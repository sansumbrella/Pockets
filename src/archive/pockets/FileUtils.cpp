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

#include "FileUtils.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace std;

std::string pockets::getNextNumberedPath( const fs::path &path, int padding, bool numberFirstFile )
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
