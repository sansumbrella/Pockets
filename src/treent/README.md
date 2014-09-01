# Treent

Scene graphs allow for the hierarchical composition of homogeneous objects. Entity systems allow for the non-hierarchical composition of hetergeneous attributes. Treent provides a hierarchical tree of entities that allows for both types of composition within a single structure, the TreentNode.

Under the hood, Treent uses entityx to manage the entity system.

The Treent object acts as a managing container for entity system concerns and initializes the root node of your tree.
```c++
treent::Treent	treent;
```

Set up whatever systems you will want to use with treent before you start adding entities. Then configure your systems. This setup is identical to that of entityx.
```c++
treent::Treent	treent;
treent.systems->add<treent::LayeredShapeRenderSystem>();
treent.systems->add<treent::TextRenderSystem>();
treent.systems->configure();
```

Create your root node. All TreentNodes have size and location components on them.
```c++
treent::TreentNodeRef	root = treent.createRoot();
root->setPosition( getWindowCenter() );
```

All nodes can create child nodes. When they do, the children are properly connected back to the Treent entity system.
```c++
auto child = root->createChild<treent::TreentNode>();
auto grandchild = child->createChild<CustomTreentSubclass>( "other", "constructor", "arguments" );
```

Update your root node to propagate matrix transforms throughout the tree.
```c++
root->updateTree( mat4::identity() );
treent.systems->update<treent::LayeredShapeRenderSystem>( dt );
treent.systems->update<treent::TextRenderSystem>( dt );
```

Finally, draw your content to screen.
```c++
treent.systems->system<treent::LayeredShapeRenderSystem>()->draw();
treent.systems->system<treent::TextRenderSystem>()->draw();
```

Use custom classes to make node creation easier. If you do, note that your first constructor argument must be a treent::Entity, and you will need to pass it into your parent constructor.

For more information on entities, components, and systems, see the EntityX readme:
https://github.com/sansumbrella/entityx
