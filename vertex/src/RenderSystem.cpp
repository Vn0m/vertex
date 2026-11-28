#include "vertex/RenderSystem.h"

#include "vertex/Components.h"
#include "vertex/Registry.h"
#include "vertex/Renderer.h"

namespace vertex {

void drawSprites(Registry& registry, Renderer& renderer) {
    registry.each<Transform, Sprite>([&](Entity, Transform& transform, Sprite& sprite) {
        if (sprite.texture == nullptr) {
            return;
        }
        renderer.drawSprite(*sprite.texture, transform.position, sprite.size,
                            sprite.uvRect, sprite.tint);
    });
}

}
