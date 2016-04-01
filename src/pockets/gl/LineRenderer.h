//
//  LineRenderer.h
//

#pragma once

namespace pockets
{

///
/// Draws 3d lines expanded to a constant thickness in screen-space.
/// Must be drawn with a perspective view matrix to behave properly.
///
class LineRenderer {
public:
    struct Vertex {
        ci::vec3 position;
        ci::vec3 previous;
        ci::vec3 next;
        ci::ColorA color;
        float offset; // signed half-width
        int mitered;
    };

    LineRenderer();
    void clear();
    void addLine(const std::vector<ci::vec3> &positions, float width, const ci::ColorA &frontColor, const ci::ColorA &backColor, bool mitered);
    void draw();
private:
    ci::gl::VboRef          _vertex_buffer;
    ci::gl::VboRef          _index_buffer;
    std::vector<uint32_t>   _indices;
    std::vector<Vertex>     _vertices;
    ci::gl::BatchRef        _batch;

    bool _needs_buffering = true;
    void bufferData();
};

} // namespace pockets

namespace pk = pockets;
