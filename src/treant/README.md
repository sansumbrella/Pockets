# Treant

Scene graphs allow for the hierarchical composition of homogeneous objects. Entity systems allow for the non-hierarchical composition of hetergeneous attributes. Treant provides a hierarchical tree of entities that allows for both types of composition within a single structure, the TreantNode.

Under the hood, Treant uses entityx to manage the entity system.

The Treant object acts as a managing container for entity system concerns and initializes the root node of your tree.
```c++
treant::Treant	treant;
```

Set up whatever systems you will want to use with treant before you start adding entities. Then configure your systems. This setup is identical to that of entityx.
```c++
treant::Treant	treant;
treant.systems->add<treant::LayeredShapeRenderSystem>();
treant.systems->add<treant::TextRenderSystem>();
treant.systems->configure();
```

Create your root node. All TreantNodes have size and location components on them.
```c++
treant::TreantNodeRef	root = treant.createRoot();
root->setPosition( getWindowCenter() );
```

All nodes can create child nodes. When they do, the children are properly connected back to the Treant entity system.
```c++
auto child = root->createChild<treant::TreantNode>();
auto grandchild = child->createChild<CustomTreantSubclass>( "other", "constructor", "arguments" );
```

Update your root node to propagate matrix transforms throughout the tree.
```c++
root->updateTree( MatrixAffine2f::identity() );
treant.systems->update<treant::LayeredShapeRenderSystem>( dt );
treant.systems->update<treant::TextRenderSystem>( dt );
```

Finally, draw your content to screen.
```c++
treant.systems->system<treant::LayeredShapeRenderSystem>()->draw();
treant.systems->system<treant::TextRenderSystem>()->draw();
```

Use custom classes to make node creation easier. If you do, note that your first constructor argument must be a treant::Entity, and you will need to pass it into your parent constructor.

For more information on entities, components, and systems, see the EntityX readme:
https://github.com/sansumbrella/entityx
