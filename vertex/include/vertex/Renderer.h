#pragma once

namespace Vertex {
    class Renderer {
        public:
            Renderer();
            void Draw();
        private:
            unsigned int VAO_{0};
            unsigned int VBO_{0};
            unsigned int EBO_{0};
    };
}
