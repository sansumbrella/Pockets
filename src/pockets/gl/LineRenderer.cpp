//
//  LineRenderer.cpp
//

#include "LineRenderer.h"

using namespace pockets;
using namespace cinder;

namespace {

struct Vertex {

ci::vec3 position;
ci::vec3 previous;
ci::vec3 next;
float    direction; // side of line we are on

};

const auto VertexLayout = ([] {
  geom::BufferLayout layout;
  layout.append(geom::Attrib::POSITION, 3, sizeof(Vertex), offsetof(Vertex, position));
  layout.append(geom::Attrib::CUSTOM_0, 3, sizeof(Vertex), offsetof(Vertex, previous));
  layout.append(geom::Attrib::CUSTOM_1, 3, sizeof(Vertex), offsetof(Vertex, next));
  layout.append(geom::Attrib::CUSTOM_2, 1, sizeof(Vertex), offsetof(Vertex, direction));

  return layout;
}());

const auto VertexMapping = gl::Batch::AttributeMapping{
  {geom::Attrib::POSITION, "ciPosition"},
  {geom::Attrib::CUSTOM_0, "Previous"},
  {geom::Attrib::CUSTOM_1, "Next"},
  {geom::Attrib::CUSTOM_2, "Direction"}
};

const auto VertexShader = R"vs(
#version 330 core

uniform mat4 ciModelViewProjection;
uniform float AspectRatio;
uniform float LineWidth;
uniform int   Miter = 1;

in vec3 ciPosition;
in vec4 ciColor;
in vec3 Previous;
in vec3 Next;
in float Direction;

out Vertex {
  float Edge;
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

  float len = LineWidth;
  float orientation = Direction;

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
      len = LineWidth / clamp(dot(miter, normal), 0.2, 1.0);
    }
    else {
      dir = ab;
    }
  }
  vec2 normal = vec2(-dir.y, dir.x);
  normal *= len;
  normal.x /= AspectRatio;

  vec4 offset = vec4(normal * orientation, 0.0, 1.0);
  gl_Position = currentPoint + offset;
  v.Edge = Direction;
  v.Color = ciColor;
}

)vs";

const auto FragmentShader = R"fs(
#version 330 core

uniform float EdgeSoftness;

out vec4 fColor;

in Vertex {
  float Edge;
  vec4  Color;
} v;

void main() {
  float e = 1.0 - abs(v.Edge);
  float intensity = pow(e, EdgeSoftness * 2.0);
  fColor = v.Color * intensity;
}

)fs";

} // namespace

LineRenderer::LineRenderer() {
  auto points = std::vector<Vertex>(100);
  auto indices = std::vector<uint16_t>(600);
  _vertex_buffer = gl::Vbo::create(GL_ARRAY_BUFFER, points, GL_STREAM_DRAW);
  _index_buffer = gl::Vbo::create(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STREAM_DRAW);
  auto mesh = gl::VboMesh::create(points.size(), GL_TRIANGLES, {{VertexLayout, _vertex_buffer}}, indices.size(), GL_UNSIGNED_SHORT, _index_buffer);
  auto format = gl::GlslProg::Format().vertex(VertexShader).fragment(FragmentShader);
  auto shader = gl::GlslProg::create(format);

  _batch = gl::Batch::create(mesh, shader, VertexMapping);
}

void LineRenderer::drawLine(const std::vector<ci::vec3> &positions, float width, float softness, bool mitered) const {

  const auto previous = [&positions] (int index) {
    return positions[glm::clamp(index - 1, 0, index)];
  };

  const auto next = [&positions, high = positions.size() - 1] (int index) {
    return positions[glm::clamp<int>(index + 1, 0, high)];
  };

  auto vertices = std::vector<Vertex>();
  for (auto i = 0; i < positions.size(); i += 1) {
    vertices.push_back(Vertex{
      positions[i],
      previous(i),
      next(i),
      1.0f
    });
    vertices.push_back(Vertex{
      positions[i],
      previous(i),
      next(i),
      -1.0f
    });
  }

  // We could cache a big index buffer and only update past the end
  // when a line comes in that needs more than we have stored so far.
  auto indices = std::vector<uint16_t>();
  indices.reserve(positions.size() * 6);
  for (auto i = 0; i < positions.size() - 1; i += 1) {
    auto index = i * 2;
    indices.push_back(index + 0);
    indices.push_back(index + 1);
    indices.push_back(index + 2);

    indices.push_back(index + 2);
    indices.push_back(index + 1);
    indices.push_back(index + 3);
  }

  auto bytes = sizeof(Vertex) * positions.size() * 2;
  _vertex_buffer->ensureMinimumSize(bytes);
  _vertex_buffer->bufferSubData(0, bytes, vertices.data());
  _index_buffer->ensureMinimumSize(indices.size() * sizeof(uint16_t));
  _index_buffer->bufferSubData(0, indices.size() * sizeof(uint16_t), indices.data());

  _batch->getGlslProg()->uniform("LineWidth", width * 0.5f);
  _batch->getGlslProg()->uniform("EdgeSoftness", softness);
  _batch->getGlslProg()->uniform("AspectRatio", app::getWindow()->getAspectRatio());
  _batch->getGlslProg()->uniform("Miter", (mitered ? 1 : 0));
  _batch->draw(0, indices.size());
}
