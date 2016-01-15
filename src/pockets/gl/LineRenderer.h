//
//  LineRenderer.h
//

#pragma once

namespace pockets
{

///
/// Draws a 3d line expanded to a constant thickness in screen-space.
/// Must be drawn with a perspective view matrix to behave properly.
///
class LineRenderer {
public:
  LineRenderer();
  void drawLine(const std::vector<ci::vec3> &positions, float width, float softness, bool mitered) const;
private:
  ci::gl::VboRef    _vertex_buffer;
  ci::gl::VboRef    _index_buffer;
  ci::gl::BatchRef  _batch;
};

} // namespace pocket
