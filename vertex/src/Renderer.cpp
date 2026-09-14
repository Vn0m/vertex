#include "../include/vertex/Renderer.h"
#include "../include/vertex/Window.h"
#include <glad/gl.h>
namespace Vertex {

    Renderer::Renderer() {
    // handles the mixing of color based on opacity of different overlapping images
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glGenVertexArrays(1,&VAO_);
    glGenBuffers(1,&VBO_);    
    glGenBuffers(1,&EBO_);    

    unsigned int indices[] = {
    0,1,3, // first triangle
    1,2,3 // second triangle
    };

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    glBindVertexArray(0);
    }

    void Renderer::draw() {
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,   // bottom-left
            0.5f, -0.5f, 0.0f,   // bottom-right
            0.0f,  0.5f, 0.0f,   // top
        };

        glGenVertexArrays(1,&VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER,VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
} // end namespace vertex
