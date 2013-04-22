//
//  Vbo.h
//  VboSample
//
//  Created by David Wicks on 12/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#if defined( CINDER_GLES )
#include <OpenGLES/ES2/gl.h>
#include <vector>
#include <string>

#include "cinder/gl/GlslProg.h"

namespace pockets { namespace gl
{
  //! Stores information for mapping attribute data on CPU to that in a shader
  //! program. Maps name to data type.
  //! Makes it easier to call VertexAttribPointer and Enable/DisableVertexAttribArray
  class Attribute
  {
  public:
  	Attribute( const std::string &name, GLint size, GLenum type, GLsizei offset=0 );
    std::string getName() const { return mName; }
    //! set the index to a value (find index with GlslProg::getAttribLocation( Attribute::getName() ))
    GLuint 			getIndex() const { return mIndex; }
    GLsizei 		getOffset() const { return mOffset; }
    GLint 			getSize() const { return mSize; }
    GLenum 			getType() const { return mType; }
    GLboolean 	isNormalized() const { return mNormalized; }
    void setIndex( GLuint index ){ mIndex = index; }
    void enable(){ glEnableVertexAttribArray( mIndex ); }
    void disable(){ glDisableVertexAttribArray( mIndex ); }
  private:
  	//! name for finding index from a shader
    std::string	mName;
    //! the index of this named attribute in a given shader
    GLuint			mIndex = 0;
    //! the number of components per vertex attribute (1, 2, 3, or 4)
    GLint				mSize = 4;
    //! the offset within our data
    GLsizei			mOffset = 0;
    //! the type of data in each component
    GLenum			mType = GL_FLOAT;
    GLboolean		mNormalized = GL_FALSE;
  };

	//! contains information about a set of attributes
  //! attributes are expected by a shader and provided by Vbo(s)
  //! makes it easier to point to interleaved data
  class AttributeSet
  {
	public:
  	AttributeSet() = default;
    explicit AttributeSet( const Attribute &attr )
    {
      mAttributes.push_back( attr );
    }
  	//! add an attribute to set
    void addAttribute( const std::string &name, GLint size, GLenum type, GLsizei offset=0 );
    GLsizei getStride() const { return mStride; }
    void setStride( GLsizei stride ){ mStride = stride; }
    void setIndices( ci::gl::GlslProg shader );
    void enableAttributes();
    void disableAttributes();
		typedef std::vector<Attribute>::iterator Iter;
    Iter begin(){ return mAttributes.begin(); }
    Iter end(){ return mAttributes.end(); }
  private:
    std::vector<Attribute>	mAttributes;
    GLsizei									mStride = 0;
  };

  typedef std::shared_ptr<class Vbo> VboRef;
	//! buffers data on the GPU
  //! keeps track of what type of data is buffered with an AttributeSet
	class Vbo
	{
	public:
  	static VboRef create( const AttributeSet &attributes ){ return VboRef( new Vbo( attributes ) ); }
    static VboRef create( const Attribute &attribute ){ return VboRef( new Vbo( AttributeSet(attribute) ) ); }
		~Vbo();
    void			bind();
    void			unbind();
    void			bufferData( size_t size, const void *data );
    void			bufferSubData( ptrdiff_t offset, size_t size, const void *data );
    GLenum		getTarget() const { return mTarget; }
    GLuint		getId() const { return mBufferId; }
    void 			draw();
    void 			setVertexAttribIndices( ci::gl::GlslProg shader );
	private:
		Vbo( const AttributeSet &attributes );
  	GLenum				mTarget;
    GLuint				mBufferId;
    AttributeSet	mAttributes;
	};

	typedef std::shared_ptr<class VboMesh> VboMeshRef;
	//! handles groups of data that should be rendered together
	class VboMesh
  {
	public:
  	static VboMeshRef create(){ return VboMeshRef( new VboMesh() ); }
    //! set attribute indices based on shader
    void setVertexAttribIndices( ci::gl::GlslProg shader );
    //! enable all our vbos
    void enable();
		void draw();
    void addBuffer( VboRef vbo ){ mVbos.push_back( vbo ); }
  private:
    std::vector<VboRef>	mVbos;
  };

}} // ns pockets::gl

#endif
