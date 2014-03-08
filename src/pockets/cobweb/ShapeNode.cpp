
#include "ShapeNode.h"

using namespace cinder;
using namespace pockets;
using namespace cobweb;

ShapeNode::ShapeNode():
mMesh( new RenderMesh2D() )
{
}

/*
void ShapeNode::draw()
{
  gl::pushModelView();
  gl::multModelView( Matrix44f( getLocus()->toMatrix() ) );
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_COLOR_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mMesh->vertices[0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mMesh->vertices[0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mMesh->vertices[0].color.r );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mMesh->vertices.size() );

  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  gl::popModelView();
}
 */
