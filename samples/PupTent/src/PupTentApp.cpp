#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Json.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/Easing.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/tags/TagsComponent.h"

#include "pockets/AnimationUtils.h"

#include "puptent/RenderSystem.h"
#include "puptent/TextureAtlas.h"
#include "puptent/SpriteSystem.h"
#include "puptent/ParticleSystem.h"
#include "puptent/ExpiresSystem.h"
#include "puptent/ScriptSystem.h"
#include "puptent/ParticleBehaviorSystems.h"
#include "KeyboardInput.h"

/**
 Sample app used to develop features of PupTent.
 Learning about component systems and building my own components.
 While everything is dependent on the underlying component machinery,
 it is wonderfully decoupled from other systems and components.
 */

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace puptent;

class PupTentApp : public AppNative
{
public:
  void prepareSettings( Settings *settings );
	void setup();
	void update() override;
	void draw() override;
  Entity createPlayer();
  Entity createTreasure();
  Entity createRibbon();
  Entity createLine();
  Entity createShip();
  Entity createPlanet();
private:
  shared_ptr<EventManager>  mEvents;
  shared_ptr<EntityManager> mEntities;
  shared_ptr<SystemManager> mSystemManager;
  SpriteAnimationSystemRef  mSpriteSystem;
  double                    mAverageUpdateTime = 1.0;
  double                    mAverageRenderTime = 1.0;
  Timer                     mTimer;
  TextureAtlas              mTextureAtlas;
};

void PupTentApp::prepareSettings( Settings *settings )
{
  settings->disableFrameRate();
  settings->setWindowSize( 1024, 768 );
  //  settings->setFullScreen();
}

void PupTentApp::setup()
{
  gl::enableVerticalSync();
  Surface sprite_surf{ loadImage( loadAsset( "spritesheet.png" ) ) };
  if( !sprite_surf.isPremultiplied() )
  {
    ip::premultiply( &sprite_surf );
  }
  TextureAtlasRef atlas = TextureAtlas::create( sprite_surf, JsonTree( loadAsset( "spritesheet.json" ) ) );
  JsonTree animations{ loadAsset( "animations.json" ) };

  // Set up entity manager and systems
  mEvents = EventManager::make();
  mEntities = EntityManager::make(mEvents);
  mSystemManager = SystemManager::make( mEntities, mEvents );
  mSystemManager->add<ExpiresSystem>();
  mSystemManager->add<ParticleSystem>();
  mSystemManager->add<ScriptSystem>();
  mSpriteSystem = mSystemManager->add<SpriteAnimationSystem>( atlas, animations );
  auto renderer = mSystemManager->add<RenderSystem>();
  renderer->setTexture( atlas->getTexture() );
  mSystemManager->configure();

  createPlayer();
  for( int i = 0; i < 1000; ++i )
  {
    createTreasure();
  }
  renderer->checkOrdering();

  createRibbon();
  createLine();
  createShip();
  createPlanet();
  mTimer.start();
}

Entity PupTentApp::createPlanet()
{
  Entity planet = mEntities->create();

  // TODO: create a planet rendering system
  // For now: draw stuff with the existing system
  float planet_size = 200.0f;
  auto loc = planet.assign<Locus>();
  auto mesh = planet.assign<RenderMesh>();
  mesh->setAsCircle( Vec2f{ planet_size, planet_size } );
  mesh->setColor( Color::gray( 0.8f ) );
  planet.assign<RenderData>( mesh, loc, 0 );
  loc->position = Vec2f{ 200.0f, 400.0f };

  for( int i = 0; i < 10; ++i )
  {
    // compound shape setup to avoid entity explosion?
    // entities are cheap, though, so it's not a real problem
    Entity e = mEntities->create();
    auto mesh = e.assign<RenderMesh>( 3 );
    mesh->vertices[0].position = Vec2f{ 0.0f, 0.0f };
    mesh->vertices[1].position = Vec2f{ 30.0f, 45.0f };
    mesh->vertices[2].position = Vec2f{ -30.0f, 45.0f };
    mesh->setColor( Color( CM_HSV, 0.25f, 1.0f, 1.0f ) );

    auto locus = e.assign<Locus>();
    locus->parent = loc;
    locus->position = Vec2f{ Rand::randFloat( -1.0f, 1.0f ), Rand::randFloat( -1.0f, 1.0f ) } * planet_size * 0.5f;
    locus->registration_point = Vec2f{ 0.0f, 20.0f };
    locus->rotation = Rand::randFloat( M_PI );
    locus->scale = Rand::randFloat( 0.5f, 4.0f );
    e.assign<RenderData>( mesh, locus, 1, RenderPass::eMultiplyPass );
  }

  return planet;
}

Entity PupTentApp::createShip()
{
  Entity ship = mEntities->create();
  auto loc = ship.assign<Locus>();
  loc->rotation = M_PI * 0.33f;
  loc->scale = 1.0f;
  loc->position = getWindowCenter() - Vec2f{ 0.0f, 100.0f };
  auto verlet = ship.assign<Particle>( loc );
  verlet->friction = 0.9f;
  verlet->rotation_friction = 0.0f;

  Entity left_wing = mEntities->create();
  { // left wing
    auto locus = left_wing.assign<Locus>();
    auto mesh = left_wing.assign<RenderMesh>( 3 );
    mesh->setAsTriangle( Vec2f{ 0.0f, 0.0f }, Vec2f{ -20.0f, 40.0f }, Vec2f{ 0.0f, 40.0f } );
    mesh->setColor( Color( CM_HSV, 0.55f, 1.0f, 1.0f ) );
    locus->parent = loc;
    locus->rotation = M_PI * 0.05f;
    locus->position = Vec2f{ -1.0f, 0.0f };
    left_wing.assign<RenderData>( mesh, locus, 5 );
  }

  Entity right_wing = mEntities->create();
  { // right wing
    auto locus = right_wing.assign<Locus>();
    auto mesh = right_wing.assign<RenderMesh>( 3 );
    mesh->setAsTriangle( Vec2f{ 0.0f, 0.0f }, Vec2f{ 20.0f, 40.0f }, Vec2f{ 0.0f, 40.0f } );
    mesh->setColor( Color( CM_HSV, 0.65f, 1.0f, 1.0f ) );
    locus->parent = loc;
    locus->rotation = -M_PI * 0.05f;
    locus->position = Vec2f{ 1.0f, 0.0f };
    right_wing.assign<RenderData>( mesh, locus, 5 );
  }

  Entity trailing_ribbon = mEntities->create();
  { // smoke trail or something (use ship locus directly to plot path)
    auto locus = trailing_ribbon.assign<Locus>();
    std::deque<Vec2f> ribbon_vertices;
    ribbon_vertices.assign( 11, loc->toMatrix().transformPoint( { 0.0f, 40.0f } ) );
    auto mesh = trailing_ribbon.assign<RenderMesh>( 20 );
    trailing_ribbon.assign<RenderData>( mesh, locus, 4 );
    trailing_ribbon.assign<CppScriptComponent>([=]( Entity self, double dt ) mutable
                                            { // use the ship locus to update out vertices
                                              auto locus = self.component<Locus>();
                                              auto front = loc->toMatrix().transformPoint( { 0.0f, 40.0f } );
                                              ribbon_vertices.emplace_front( front );
                                              ribbon_vertices.pop_back();
                                              auto mesh = self.component<RenderMesh>();
                                              mesh->setAsRibbon( ribbon_vertices, 4.0f );
                                            } );
  }

  auto input = KeyboardInput::create();
  input->connect( getWindow() );
  ship.assign<CppScriptComponent>( [=]( Entity self, double dt ) mutable
                               {
                                 auto locus = self.component<Locus>();
                                 auto verlet = self.component<Particle>();
                                 Vec2f delta = locus->position - verlet->p_position;
                                 locus->position += input->getForce() * dt * 100.0f;
                                 if( delta.lengthSquared() > EPSILON_VALUE )
                                 {
                                   locus->rotation = wrapLerp( locus->rotation, (float)M_PI * 0.5f + math<float>::atan2( delta.y, delta.x ), (float)M_PI * 2, 0.2f );
                                 }

                                 if( input->getKeyPressed( KeyEvent::KEY_d ) )
                                 {
                                   // break off children
                                   auto l_loc = left_wing.component<Locus>();
                                   auto r_loc = right_wing.component<Locus>();
                                   auto p = left_wing.assign<Particle>( l_loc );
                                   p->rotation_friction = 0.99f;
                                   p->friction = 0.99f;
                                   p = right_wing.assign<Particle>( r_loc );
                                   p->rotation_friction = 0.99f;
                                   p->friction = 0.99f;

                                   l_loc->detachFromParent();
                                   r_loc->detachFromParent();
                                   l_loc->position += input->getForce() * dt * 100.0f;
                                   r_loc->position += input->getForce() * dt * 100.0f;
                                   l_loc->position += Rand::randVec2f() * 10.0f;
                                   r_loc->position += Rand::randVec2f() * 10.0f;
                                   l_loc->rotation += Rand::randFloat( M_PI * 0.1f );
                                   r_loc->rotation += Rand::randFloat( M_PI * 0.1f );
                                 }
                               } );
  return ship;
}

Entity PupTentApp::createRibbon()
{
  Entity e = mEntities->create();
  auto loc = e.assign<Locus>();
  auto mesh = e.assign<RenderMesh>( 100 );
  e.assign<RenderData>( mesh, loc );
  vector<Vec2f> positions;
  for( int i = 0; i < 50; ++i )
  {
    positions.push_back( Vec2f{ 50.0f + i * 10.0f, 100.0f + sin( i * 0.2f ) * 50.0f } );
  }
  try {
    mesh->setAsRibbon( positions, 20.0f, false );
  } catch ( exception &exc ) {
    cout << "Error: " << exc.what() << endl;
  }
  return e;
}

Entity PupTentApp::createLine()
{
  Entity e = mEntities->create();
  auto loc = e.assign<Locus>();
  auto mesh = e.assign<RenderMesh>();
  e.assign<RenderData>( mesh, loc, 20 );
  e.assign<CppScriptComponent>( [=](Entity self, double dt){
    mesh->setAsLine( { 0, 0 }, getMousePos(), 8.0f );
  } );
  return e;
}

Entity PupTentApp::createPlayer()
{
  Rand r;
  auto player = mEntities->create();
  auto loc = shared_ptr<Locus>{ new Locus };
  // get an animation out of the sprite system
  auto anim = mSpriteSystem->createSpriteAnimation( "jellyfish" );
  // ping-pong animation
  //  anim->looping = false;
  //  anim->finish_fn = []( SpriteAnimationRef animation ){
  //    animation->rate *= -1.0f;
  //  };
  loc->position = getWindowCenter();
  loc->rotation = r.nextFloat( M_PI * 2 );
  loc->registration_point = { 20.0f, 10.0f }; // center of the mesh created below
  auto mesh = player.assign<RenderMesh>( 4 );
  player.assign( anim );
  player.assign( loc );
  player.assign<RenderData>( mesh, loc, 1 );
  auto verlet = player.assign<Particle>( loc );
  verlet->friction = 0.9f;
  verlet->rotation_friction = 0.5f;
  auto input = KeyboardInput::create();
  input->connect( getWindow() );
  // give custom behavior to the player
  auto view = tags::TagsComponent::view( mEntities->entities_with_components<Locus>(), "treasure" );
  player.assign<CppScriptComponent>( [=](Entity self, double dt){
    auto locus = self.component<Locus>();
//    locus->position += input->getForce() * dt * 100.0f;
    for( auto entity : view )
    {
      auto other_loc = entity.component<Locus>();
      if( other_loc->position.distance( locus->position ) < 50.0f )
      {
        cout << "Eating treasure" << endl;
        entity.destroy();
      }
    }
  } );
  return player;
}

Entity PupTentApp::createTreasure()
{
  auto entity = mEntities->create();
  auto loc = shared_ptr<Locus>{ new Locus };
  // get an animation out of the sprite system
  auto anim = mSpriteSystem->createSpriteAnimation( "dot" );
  anim->current_index = Rand::randInt( 0, 10 );
  loc->position = { Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) };
  loc->rotation = Rand::randFloat( M_PI * 2 );
  loc->registration_point = { 20.0f, 10.0f }; // center of the mesh created below
  auto color = Color::gray( Rand::randFloat( 0.4f, 1.0f ) );
  auto mesh = entity.assign<RenderMesh>( 4 );
  mesh->setColor( color );
  entity.assign( anim );
  entity.assign( loc );
  entity.assign<RenderData>( mesh, loc, Rand::randInt( 50 ), eNormalPass );
  // randomized expire time, weighted toward end
  entity.assign<Expires>( easeOutQuad( Rand::randFloat() ) * 5.0f + 1.0f );
  entity.assign<tags::TagsComponent>( "treasure" );
  //  entity.assign<SeekComponent>( predator_loc, Vec2f{-1.0f, -0.5f} );
  return entity;
}

void PupTentApp::update()
{
  double dt = mTimer.getSeconds();
  mTimer.start();
  Timer up;
  up.start();
  mSystemManager->update<ExpiresSystem>( dt );
  mSystemManager->update<ScriptSystem>( dt );
  mSystemManager->update<SpriteAnimationSystem>( dt );
  mSystemManager->update<ParticleSystem>( dt );
  mSystemManager->update<RenderSystem>( dt );
  double ms = up.getSeconds() * 1000;
  mAverageUpdateTime = (mAverageUpdateTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 90 == 0 )
  {
    cout << "Update: " << mAverageUpdateTime << ", " << ms << endl;
  }
}

void PupTentApp::draw()
{
	gl::clear( Color::black() );
  gl::color( Color::white() );
  Timer dr;
  dr.start();
  //  mSystemManager->system<PhysicsSystem>()->debugDraw();
  mSystemManager->system<RenderSystem>()->draw();
  double ms = dr.getSeconds() * 1000;
  mAverageRenderTime = (mAverageRenderTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 90 == 0 )
  {
    cout << "Render: " << mAverageRenderTime << ", " << ms << endl;
  }
}

CINDER_APP_NATIVE( PupTentApp, RendererGl( RendererGl::AA_MSAA_4 ) )
