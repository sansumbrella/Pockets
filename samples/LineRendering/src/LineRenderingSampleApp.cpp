#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "pockets/gl/LineRenderer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LineRenderingSampleApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void update() override;
    void draw() override;
private:
    pockets::LineRenderer mLineRenderer;
    std::deque<vec3>      mMousePositions;
    int                   mMaxMousePositions = 128;
};

void LineRenderingSampleApp::setup()
{
    // Draw a hard-edged figure with mitered joints.
    vector<vec3> figure {
        vec3(10, 10, 0), vec3(50, 10, 0), vec3(50, 50, 0), vec3(10, 50, 0)
    };
    mLineRenderer.addLine(figure, 10.0f, ColorA::white(), ColorA::white(), true);

    // Draw a smooth shape without mitering.
    vector<vec3> wiggle;
    for (auto i = 0; i < 128; i += 1)
    {
        auto t = 2 * M_PI * i / (127.0f);
        auto offset = vec3(120.0f, 10.0f + t * 60.0f, 0.0f);
        auto x = cos(2.0f * t) * 30.0f;
        auto y = sin(3.0f * t) * 30.0f;

        wiggle.push_back(vec3(x, y, 0.0f) + offset);
    }
    auto c = ColorA(CM_HSV, 0.85f, 1.0f, 0.8f, 1.0f);
    mLineRenderer.addLine(wiggle, 12.0f, c, c, false);
}

void LineRenderingSampleApp::mouseDown(MouseEvent event)
{
    mMousePositions.clear();
}

void LineRenderingSampleApp::mouseDrag(MouseEvent event)
{
    mMousePositions.push_front(vec3(event.getPos(), 0.0f));
    if (mMousePositions.size() > mMaxMousePositions)
    {
        mMousePositions.pop_back();
    }
}

void LineRenderingSampleApp::update()
{
    if (!mMousePositions.empty())
    {
        mLineRenderer.clear();
        // Add raw mouse line.
        mLineRenderer.addLine({mMousePositions.begin(), mMousePositions.end()}, 64.0f, ColorA(CM_HSV, 0.875f, 1.0f, 1.0f, 1.0f), ColorA(CM_HSV, 0.675f, 1.0f, 0.5f, 1.0f), false);

        // If we have enough points to calculate a curve,
        // generate a smoothed path from the raw mouse points.
        if (mMousePositions.size() > 3)
        {
            // Create a spline through our points
            auto spline = BSpline2f({mMousePositions.begin(), mMousePositions.end()}, 3, false, true);
            // Convert to Path2d for handy subdivide feature.
            auto path = Path2d(spline);
            auto sub = path.subdivide();
            // Convert vec2 to vec3
            vector<vec3> positions;
            positions.reserve(sub.size());
            for (auto &pos : sub) {
                positions.push_back(vec3(pos, 0));
            }
            // Add our smoothed line.
            mLineRenderer.addLine(positions, 18.0f, ColorA(CM_HSV, 0.1f, 1.0f, 1.0f, 1.0f), ColorA(CM_HSV, 0.0f, 1.0f, 1.0f, 1.0f), false);
        }
    }
}

void LineRenderingSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatricesWindowPersp(getWindowWidth(), getWindowHeight());
    mLineRenderer.draw();
}

CINDER_APP( LineRenderingSampleApp, RendererGl(RendererGl::Options().msaa(4)) )
