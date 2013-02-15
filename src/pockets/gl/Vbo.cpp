//
//  Vbo.cpp
//  VboSample
//
//  Created by David Wicks on 12/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Vbo.h"

using namespace std;

namespace pockets { namespace gl {

	//
  //	Attribute
  //

  Attribute::Attribute( const string &name, GLint size, GLenum type, GLsizei offset ):
  mName( name )
  , mSize( size )
  , mType( type )
  , mOffset( offset )
  {}

  //
  //
  //

  void AttributeSet::addAttribute( const std::string &name, GLint size, GLenum type, GLsizei offset )
  {
    mAttributes.push_back( Attribute( name, size, type, offset ) );
  }

  void AttributeSet::setIndices( ci::gl::GlslProg shader )
  {
    for( auto &attr : mAttributes )
    {
      attr.setIndex( shader.getAttribLocation( attr.getName() ) );
    }
  }

  void AttributeSet::enableAttributes()
  {
    for( auto &attr : mAttributes )
    {
      attr.enable();
    }
  }

  void AttributeSet::disableAttributes()
  {
    for( auto &attr : mAttributes )
    {
      attr.disable();
    }
  }

	//
  //	Vbo
  //

  Vbo::Vbo( const AttributeSet &attributes ):
  mAttributes( attributes )
  {
    glGenBuffers( 1, &mBufferId );
  }

  Vbo::~Vbo()
  {
    glDeleteBuffers( 1, &mBufferId );
  }

	void Vbo::bufferSubData( ptrdiff_t offset, size_t size, const void *data )
  {
    glBufferSubData( mTarget, offset, size, data );
  }

  void Vbo::setVertexAttribIndices( ci::gl::GlslProg shader )
  {
  	mAttributes.setIndices( shader );
  }

  void Vbo::bind()
  {
  	for( auto &attr : mAttributes )
    {
      glVertexAttribPointer( attr.getIndex()
      											, attr.getSize()
                            , attr.getType()
                            , attr.isNormalized()
                            , mAttributes.getStride()
                            , (void*) attr.getOffset() );
      attr.enable();
    }
  }

	//
  //	VboMesh
  //

  void VboMesh::setVertexAttribIndices( ci::gl::GlslProg shader )
  {
    for( auto vbo : mVbos )
    {
			vbo->setVertexAttribIndices( shader );
    }
  }

}} // ns pockets::gl
