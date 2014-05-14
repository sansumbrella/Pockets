
#include "ShapeNode.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/gl.h"

using namespace cinder;
using namespace pockets;
using namespace cobweb;

ShapeNode::ShapeNode( RenderMesh2DRef mesh ):
  mMesh( mesh )
{}

void ShapeNode::draw()
{
  auto context = gl::context();

  context->pushVao();
  context->getDefaultVao()->replacementBindBegin();
  gl::VboRef defaultVbo = context->getDefaultArrayVbo( sizeof(Vertex) * mMesh->vertices.size() );
  gl::ScopedBuffer bufferBindScope( defaultVbo );
  defaultVbo->bufferSubData( 0, sizeof(Vertex) * mMesh->vertices.size(), mMesh->vertices.data() );

  auto shader = context->getGlslProg();
  int posLoc = shader->getAttribSemanticLocation( geom::Attrib::POSITION );
  if( posLoc >= 0 ) {
    gl::enableVertexAttribArray( posLoc );
    gl::vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof( Vertex, position ) );
  }
  int texLoc = shader->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
  if( texLoc >= 0 ) {
    gl::enableVertexAttribArray( texLoc );
    gl::vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof( Vertex, tex_coord ) );
  }
  int colorLoc = shader->getAttribSemanticLocation( geom::Attrib::COLOR );
  if( colorLoc >= 0 ) {
    gl::enableVertexAttribArray( colorLoc );
    gl::vertexAttribPointer( colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof( Vertex, color ) );
  }

  context->getDefaultVao()->replacementBindEnd();
  context->setDefaultShaderVars();
  context->drawArrays( GL_TRIANGLE_STRIP, 0, mMesh->vertices.size() );
}
