//
//  LineRenderer.cpp
//

#include "LineRenderer.h"

using namespace pockets;
using namespace cinder;

namespace {

using Vertex = LineRenderer::Vertex;

const auto VertexLayout = ([] {
    geom::BufferLayout layout;
    layout.append(geom::Attrib::POSITION, 3, sizeof(Vertex), offsetof(Vertex, position));
    layout.append(geom::Attrib::COLOR, 4, sizeof(Vertex), offsetof(Vertex, color));
    layout.append(geom::Attrib::CUSTOM_0, 3, sizeof(Vertex), offsetof(Vertex, previous));
    layout.append(geom::Attrib::CUSTOM_1, 3, sizeof(Vertex), offsetof(Vertex, next));
    layout.append(geom::Attrib::CUSTOM_2, 1, sizeof(Vertex), offsetof(Vertex, offset));
    layout.append(geom::Attrib::CUSTOM_3, 1, sizeof(Vertex), offsetof(Vertex, mitered));

    return layout;
}());

const auto VertexMapping = gl::Batch::AttributeMapping{ { geom::Attrib::POSITION, "ciPosition" },
    { geom::Attrib::COLOR, "ciColor" },
    { geom::Attrib::CUSTOM_0, "Previous" },
    { geom::Attrib::CUSTOM_1, "Next" },
    { geom::Attrib::CUSTOM_2, "EdgeOffset" },
    { geom::Attrib::CUSTOM_3, "Miter" } };

const auto VertexShader = R"vs(
#version 330 core

uniform mat4 ciModelViewProjection;
uniform float AspectRatio;

in vec3 ciPosition;
in vec4 ciColor;
in vec3 Previous;
in vec3 Next;
in float EdgeOffset;
in int Miter;

out Vertex {
	vec4 Color;
} v;

void main() {
	vec2 aspectVector = vec2(AspectRatio, 1.0);
	vec4 previousPoint = ciModelViewProjection * vec4(Previous, 1.0);
	vec4 currentPoint = ciModelViewProjection * vec4(ciPosition, 1.0);
	vec4 nextPoint = ciModelViewProjection * vec4(Next, 1.0);

	vec2 currentScreen = aspectVector * currentPoint.xy / currentPoint.w;
	vec2 previousScreen = aspectVector * previousPoint.xy / previousPoint.w;
	vec2 nextScreen = aspectVector * nextPoint.xy / nextPoint.w;

	float len = EdgeOffset;

	vec2 dir = vec2(0.0);
	if (currentScreen == previousScreen) {
		dir = normalize(nextScreen - currentScreen);
	}
	else if (currentScreen == nextScreen) {
		dir = normalize(currentScreen - previousScreen);
	}
	else {
		vec2 ab = normalize(currentScreen - previousScreen);
		if (Miter == 1) {
			vec2 bc = normalize(nextScreen - currentScreen);
			vec2 tangent = normalize(ab + bc);
			vec2 normal = vec2(-ab.y, ab.x);
			vec2 miter = vec2(-tangent.y, tangent.x);
			dir = tangent;
			len = EdgeOffset / clamp(dot(miter, normal), 0.1, 1.0);
		}
		else {
			dir = ab;
		}
	}
	vec2 normal = vec2(-dir.y, dir.x);
	normal *= len;
	normal.x /= AspectRatio;

	vec4 offset = vec4(normal, 0.0, 0.0);
	gl_Position = currentPoint + offset;
	v.Color = ciColor;
}

)vs";

const auto FragmentShader = R"fs(
#version 330 core

out vec4 fColor;

in Vertex {
	vec4  Color;
} v;

void main() {
    fColor = v.Color;
}

)fs";

} // namespace

LineRenderer::LineRenderer()
{
    auto points = std::vector<Vertex>(2 * 10 * 1000);
    auto indices = std::vector<uint16_t>(points.size() * 6);
    _vertex_buffer = gl::Vbo::create(GL_ARRAY_BUFFER, points, GL_STREAM_DRAW);
    _index_buffer = gl::Vbo::create(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STREAM_DRAW);
    auto mesh = gl::VboMesh::create(points.size(), GL_TRIANGLES, { { VertexLayout, _vertex_buffer } }, indices.size(), GL_UNSIGNED_INT, _index_buffer);
    auto format = gl::GlslProg::Format().vertex(VertexShader).fragment(FragmentShader);
    auto shader = gl::GlslProg::create(format);

    _batch = gl::Batch::create(mesh, shader, VertexMapping);
}

void LineRenderer::clear()
{
    _indices.clear();
    _vertices.clear();

    _needs_buffering = true;
}

void LineRenderer::addLine(const std::vector<ci::vec3> &positions, float width, const ColorA &frontColor, const ColorA &backColor, bool mitered)
{
    // add indices
    const auto index_offset = _vertices.size();
    for (auto i = 0; i < (positions.size() - 1); i += 1) {
        auto index = index_offset + i * 2;
        _indices.push_back(index + 0);
        _indices.push_back(index + 1);
        _indices.push_back(index + 2);

        _indices.push_back(index + 2);
        _indices.push_back(index + 1);
        _indices.push_back(index + 3);
    }

    // add positions
    const auto previous = [&positions](int index) {
        return positions[glm::clamp(index - 1, 0, index)];
    };

    const auto next = [&positions, high = positions.size() - 1](int index)
    {
        return positions[glm::clamp<int>(index + 1, 0, high)];
    };

    const auto color = [frontColor, backColor, end = positions.size()](int index)
    {
        return lerp(frontColor, backColor, (float)index / end);
    };

    auto mitered_int = mitered ? 1 : 0;
    auto half_width = width / 2.0f;
    for (auto i = 0; i < positions.size(); i += 1) {
        _vertices.push_back(Vertex{ positions[i], previous(i), next(i), color(i), half_width, mitered_int });
        _vertices.push_back(Vertex{ positions[i], previous(i), next(i), color(i), -half_width, mitered_int });
    }

    _needs_buffering = true;
}

void LineRenderer::bufferData()
{
    if (_needs_buffering)
    {
        const auto vb = sizeof(Vertex) * _vertices.size();
        _vertex_buffer->ensureMinimumSize(vb);
        _vertex_buffer->bufferSubData(0, vb, _vertices.data());

        const auto ib = sizeof(uint32_t) * _indices.size();
        _index_buffer->ensureMinimumSize(ib);
        _index_buffer->bufferSubData(0, ib, _indices.data());

        _needs_buffering = false;
    }
}

void LineRenderer::draw()
{
    bufferData();

    auto dims = vec2(gl::getViewport().second);
    _batch->getGlslProg()->uniform("AspectRatio", dims.x / dims.y);
    _batch->draw(0, _indices.size());
}
