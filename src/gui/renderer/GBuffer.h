//
// Created by tomas on 2.8.22.
//

#pragma once
#include "geGL/Texture.h"
#include <array>
#include <geGL/Buffer.h>
#include <geGL/Framebuffer.h>
#include <geGL/OpenGLContext.h>
#include <geGL/VertexArray.h>
#include <memory>
namespace MapGenerator::Renderer {

    class GBuffer {
    public:
        explicit GBuffer(const ge::gl::Context& gl);
        GBuffer() = default;

        void init(int width, int height, int defaultFbo);
        void resize(int width, int height, int defaultFbo);

        void bindBuffer();
        void copyToDefaultBuffer(int id);


        std::shared_ptr<ge::gl::Texture> positionTexture() const;
        std::shared_ptr<ge::gl::Texture> normalTexture() const;
        std::shared_ptr<ge::gl::Texture> colorTexture() const;

    private:
        ge::gl::Context gl;

        static constexpr auto vertices =
            std::array{-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
                       -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f};
        int width = 0;
        int height = 0;
        std::shared_ptr<ge::gl::Buffer> quadBuffer;
        std::shared_ptr<ge::gl::VertexArray> quadVAO;
        std::shared_ptr<ge::gl::Framebuffer> gBuffer;
        std::shared_ptr<ge::gl::Texture> gPosition;
        std::shared_ptr<ge::gl::Texture> gNormal;
        std::shared_ptr<ge::gl::Texture> gSpecular;
        std::shared_ptr<ge::gl::Renderbuffer> rboDepth;

    };

} // namespace MapGenerator::Renderer