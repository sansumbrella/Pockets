#include "GLKVbo.h"

namespace cinder { namespace gl {

using namespace std;


GLKVbo::GLKVbo(GLuint type)
: mType(type)
{
}
GLKVbo::~GLKVbo()
{
}

void GLKVbo::set(AttributeRef attr)
{
    mAttributes[attr->getName()] = attr;
}
GLKVbo::AttributeRef GLKVbo::get(const string &name)
{
    auto iter = mAttributes.find(name);
    if(iter != mAttributes.end())
        return iter->second;
    return AttributeRef();
}

void GLKVbo::assignLocations(GlslProg shader)
{
    for(auto &pair : mAttributes){
        pair.second->setLocation(shader.getAttribLocation(pair.first));
    }
}

void GLKVbo::bind()
{
    mMinBoundAttributeLength = numeric_limits<size_t>::max();
    mEnabledLocations.clear();
    mBoundIndices.reset();
    
    // Bind any vertex attributes that have data and a location
    for(auto &pair : mAttributes){
        AttributeRef attr = pair.second;
        if(attr->getTarget() == GL_ARRAY_BUFFER && attr->getLocation() >= 0 && attr->getData()){
            attr->bindAndEnable();
            mEnabledLocations.push_back(attr->getLocation());
            mMinBoundAttributeLength = math<size_t>::min(mMinBoundAttributeLength, attr->getLength());
        }
        else if(attr->getTarget() == GL_ELEMENT_ARRAY_BUFFER){
            attr->bind();
            mBoundIndices = attr;
        }
    }
}
void GLKVbo::unbind()
{
    // Disable vertex attributes and Reset bind state
    for(int location : mEnabledLocations){
        glDisableVertexAttribArray(location);
    }
    if(mBoundIndices)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    else
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLKVbo::draw()
{
    bind();
    
    // Draw if any attributes are enabled
    if(mEnabledLocations.size() > 0){
        if(mBoundIndices)
            glDrawElements(mType, mBoundIndices->getData().getDataSize() / 2, mBoundIndices->getType(), 0); // TODO(ryan): This assumes the indices are 16bit ints
        else
            glDrawArrays(mType, 0, mMinBoundAttributeLength);
    }
    
    unbind();
}
void GLKVbo::draw(GlslProg shader)
{
    assignLocations(shader);
    shader.bind();
    draw();
}


GLKVbo::Attribute::Attribute(const string &name, int size, GLenum type, GLenum usage)
: mBuffer(0)
, mName(name)
, mSize(size)
, mType(type)
, mUsage(usage)
, mLocation(-1)
, mDataDirty(false)
{
    mTarget = mName == "index" ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
}
GLKVbo::Attribute::~Attribute()
{
    if(mBuffer){
        glDeleteBuffers(1, &mBuffer);
        mBuffer = 0;
    }
}

GLKVbo::AttributeRef GLKVbo::Attribute::create(const string &name, int size, GLenum type, GLenum usage)
{
    return AttributeRef(new Attribute(name, size, type, usage));
}
GLKVbo::AttributeRef GLKVbo::Attribute::create(const char *name, int size, GLenum type, GLenum usage)
{
    return AttributeRef(new Attribute(string(name), size, type, usage));
}
GLKVbo::AttributeRef GLKVbo::Attribute::createIndex(GLenum usage)
{
    return AttributeRef(new Attribute(string("index"), 1, GL_UNSIGNED_SHORT, usage));
}

void GLKVbo::Attribute::bufferData()
{
    if(!mBuffer)
        glGenBuffers(1, &mBuffer);
    glBindBuffer(mTarget, mBuffer);
    glBufferData(mTarget, mData.getDataSize(), mData.getData(), mUsage);
    mDataDirty = false;
}

void GLKVbo::Attribute::bind()
{
    if(mDataDirty)
        bufferData();
    glBindBuffer(mTarget, mBuffer);
}

void GLKVbo::Attribute::bindAndEnable()
{
    bind();
    glVertexAttribPointer(mLocation, mSize, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(mLocation);
}

void GLKVbo::Attribute::disable()
{
    glDisableVertexAttribArray(mLocation);
}


GLKVboRef GLKVbo::create(GLenum type)
{
    return GLKVboRef(new GLKVbo(type));
}

GLKVboRef GLKVbo::createPlane(const Vec2f &p1, const Vec2f &p2)
{
    float positions[] = { p1.x, p1.y, 0, p1.x, p2.y, 0, p2.x, p1.y, 0, p2.x, p2.y, 0 };
	GLfloat colors[] = { 	1, 1, 1, 1,   1, 1, 1, 1,
							1, 1, 1, 1,   1, 1, 1, 1 };
    float texcoords[] = { 0, 0, 0, 1, 1, 0, 1, 1 };
    
    GLKVboRef vbo = GLKVbo::create(GL_TRIANGLE_STRIP);
    vbo->set(Attribute::create("position", 3)->setData(positions, 12));
	vbo->set(Attribute::create("color", 4)->setData(colors, 16 ) );
    vbo->set(Attribute::create("texcoord", 2)->setData(texcoords, 8));
    
    return vbo;
}
GLKVboRef GLKVbo::createPlane(const Vec2f &size)
{
    return createPlane(size * -0.5f, size * 0.5f);
}
GLKVboRef GLKVbo::createPlane(const Rectf &rect)
{
    return createPlane(rect.getUpperLeft(), rect.getLowerRight());
}

GLKVboRef GLKVbo::createBox(const Vec3f &p1, const Vec3f &p2)
{
    GLfloat positions[] = { p2.x,p2.y,p2.z, p2.x,p1.y,p2.z, p2.x,p1.y,p1.z, p2.x,p2.y,p1.z,   // +X
                            p2.x,p2.y,p2.z, p2.x,p2.y,p1.z, p1.x,p2.y,p1.z, p1.x,p2.y,p2.z,   // +Y
                            p2.x,p2.y,p2.z, p1.x,p2.y,p2.z, p1.x,p1.y,p2.z, p2.x,p1.y,p2.z,   // +Z
                            p1.x,p2.y,p2.z, p1.x,p2.y,p1.z, p1.x,p1.y,p1.z, p1.x,p1.y,p2.z,   // -X
                            p1.x,p1.y,p1.z, p2.x,p1.y,p1.z, p2.x,p1.y,p2.z, p1.x,p1.y,p2.z,   // -Y
                            p2.x,p1.y,p1.z, p1.x,p1.y,p1.z, p1.x,p2.y,p1.z, p2.x,p2.y,p1.z }; // -Z
    
    GLfloat normals[] = { 1, 0, 0,  1, 0, 0,  1, 0, 0,  1, 0, 0,
                          0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
                          0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1,
                         -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
                          0,-1, 0,  0,-1, 0,  0,-1, 0,  0,-1, 0,
                          0, 0,-1,  0, 0,-1,  0, 0,-1,  0, 0,-1 };

	GLfloat	colors[] = { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,
						 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,
						 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,
						 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,
						 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,
						 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1 };

    GLfloat texcoords[] = { 0,1, 1,1, 1,0, 0,0,
                            1,1, 1,0, 0,0, 0,1,
                            0,1, 1,1, 1,0, 0,0,
                            1,1, 1,0, 0,0, 0,1,
                            1,0, 0,0, 0,1, 1,1,
                            1,0, 0,0, 0,1, 1,1 };
    
    GLushort indices[] = { 0, 1, 2, 0, 2, 3,
                           4, 5, 6, 4, 6, 7,
                           8, 9,10, 8,10,11,
                          12,13,14,12,14,15,
                          16,17,18,16,18,19,
                          20,21,22,20,22,23 };
    
    GLKVboRef vbo = GLKVbo::create(GL_TRIANGLES);
    vbo->set(Attribute::create("position", 3)->setData(positions, 72));
    vbo->set(Attribute::create("normal", 3)->setData(normals, 72));
    vbo->set(Attribute::create("texcoord", 2)->setData(texcoords, 48));
	vbo->set(Attribute::create("color", 4)->setData(colors, 96) );
    vbo->set(Attribute::createIndex()->setData(indices, 36));

    return vbo;
}
GLKVboRef GLKVbo::createBox(const Vec3f &size)
{
    return createBox(size * -0.5f, size * 0.5f);
}

GLKVboRef GLKVbo::createWireBox(const Vec3f &p1, const Vec3f &p2)
{
    GLfloat positions[] = { p1.x,p1.y,p1.z, p2.x,p1.y,p1.z, p2.x,p2.y,p1.z, p1.x,p2.y,p1.z,
                            p1.x,p1.y,p2.z, p2.x,p1.y,p2.z, p2.x,p2.y,p2.z, p1.x,p2.y,p2.z };
    
    GLushort indices[] = { 0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 3,7, 2,6, 1,5 };
	GLfloat colors[] = { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,
						 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1 };

    
    GLKVboRef vbo = GLKVbo::create(GL_LINES);
    vbo->set(Attribute::create("position", 3)->setData(positions, 24));
	vbo->set(Attribute::create("color", 3)->setData( colors, 32 ) );
    vbo->set(Attribute::createIndex()->setData(indices, 24));
    
    return vbo;
}
GLKVboRef GLKVbo::createWireBox(const Vec3f &size)
{
    return createWireBox(size * -0.5f, size * 0.5f);
}


}} // namespace cinder::gl