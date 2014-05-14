
#pragma once

#include "pockets/cobweb/Node.h"
#include "pockets/RenderMesh.h"

namespace pockets
{
  namespace cobweb
  {
	typedef std::shared_ptr<class ShapeNode> ShapeNodeRef;
	/**
	 ShapeNode:

	 A node that draws a custom shape.
	 Uses pockets::RenderMesh as shape representation,
	 allowing for optimization by replacing deepDraw()
	 with a batch renderer.
	*/
	 class ShapeNode : public Node
	 {
	 public:
	 	ShapeNode() = default;
	 	ShapeNode( RenderMesh2DRef mesh );
	 	void draw() override;
	 	RenderMesh2DRef mesh() { return mMesh; }
	 private:
	 	RenderMesh2DRef	mMesh = std::shared_ptr<RenderMesh2D>( new RenderMesh2D( 3 ) );	// make_shared doesn't compile here in VS2013
	 };
  }
}