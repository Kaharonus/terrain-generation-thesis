//
// Created by tomas on 06.10.21.
//

#include <iostream>
#include <renderer/Renderer.h>
#include <geGL/geGL.h>
#include <thread>

#include <csignal>
#include <shaders/Shaders.h>
#include <config/ConfigReader.h>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QtConcurrent/QtConcurrent>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace MapGenerator {
    Renderer::Renderer(QWindow *parent) {
        initialized = false;
        context = nullptr;
        setSurfaceType(
                QWindow::OpenGLSurface); //this needs to be set otherwise makeCurrent and other gl context related functions will fail
        surfaceFormat.setVersion(4, 5);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        setupLib();

    }

    bool Renderer::setupLib() {

        bool hasError = false;
        auto config = ConfigReader::read(QCoreApplication::arguments().at(1).toStdString(), &hasError);
        if (hasError) {
            return false;
        }
        this->mapGenerator = std::make_shared<MapGenerator>(&config);
        return true;
    }


    void Renderer::initialize() {
        if (initialized) return;
        if (!context) {
            context = new QOpenGLContext(this);
            context->setFormat(surfaceFormat);
            bool success = context->create();
            if (!success) {
                //fail gracefully TODO:actually fail
                ::raise(SIGSEGV);
            }
            camera = std::make_shared<Camera>((Renderer *) this);
            connect(this, SIGNAL(keyPressEvent(QKeyEvent * )), camera.get(), SLOT(keyEvent(QKeyEvent * )));
            connect(this, SIGNAL(keyReleaseEvent(QKeyEvent * )), camera.get(), SLOT(keyEvent(QKeyEvent * )));
            connect(this, SIGNAL(mouseMoveEvent(QMouseEvent * )), camera.get(), SLOT(mouseMoved(QMouseEvent * )));
        }

        //let's say to the OS that we want to work with this context
        context->makeCurrent(this);
        ge::gl::init();
        gl = std::make_shared<ge::gl::Context>();

        const qreal retinaScale = devicePixelRatio();

        gl->glViewport(0, 0, width() * retinaScale, height() * retinaScale);
        gl->glClearColor(0.0, 0, 0, 1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        context->swapBuffers(this);

        std::vector<double> posHome{
                49.883325913713, 17.8657865524292, 49.89402618295204, 17.890548706054688
        };

        std::vector<double> posBrno = {
                49.2280826330813, 16.59357713886698,
                49.22626684720634, 16.600258817898766
        };

        std::vector<double> posMoni = {
                49.20637141555205, 16.69411906986399,
                49.20453830945842, 16.697963117309616
        };

        std::vector<double> posRand = {
                49.20322968539299, 16.60926962473714, 49.201543696771644, 16.61236590263251

        };

        std::vector <double> posMountains = {
                50.10588121964279, 17.198770606455174,
                50.05709781257081, 17.28661015961763

        };

        auto draw = posHome;
        auto resolution = 1024;
        auto texData = mapGenerator->getMetadata(draw[0], draw[1], draw[2], draw[3], resolution);
        texture = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, resolution, resolution);
        texture->bind(GL_TEXTURE_2D);
        texture->setData2D(texData->data(), GL_RGBA, GL_FLOAT, 0, GL_TEXTURE_2D, 0, 0, resolution, resolution);
        texture->generateMipmap();
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);


        auto data = mapGenerator->getVertices(draw[0], draw[1], draw[2], draw[3], 30);
        vertices = std::make_shared<ge::gl::Buffer>(data->vertices->size() * sizeof(float), data->vertices->data(),
                                                    GL_STATIC_DRAW);
        indices = std::make_shared<ge::gl::Buffer>(data->indices->size() * sizeof(int), data->indices->data(),
                                                   GL_STATIC_DRAW);

        vao = std::make_shared<ge::gl::VertexArray>();


        vao->addAttrib(vertices, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
        vao->addAttrib(vertices, 1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));

        //vao->addAttrib(vertices, 1, 2, GL_FLOAT, 5,3);
        vao->addElementBuffer(indices);
        drawCount = data->indices->size();

        auto vertexShader = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, VertexSource);
        auto fragmentShader = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER,
                                                               FragmentSource);

        shaderProgram = std::make_shared<ge::gl::Program>(vertexShader, fragmentShader);

        initialized = true;
        //Draw as wireframe
        //gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        gl->glActiveTexture(GL_TEXTURE0);
    }

    void Renderer::render() {
        const qreal retinaScale = devicePixelRatio();
        gl->glViewport(0, 0, width() * retinaScale, height() * retinaScale);
        gl->glClearColor(0.0, 0, 0, 1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto view = camera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) width() / (float) height(), 0.005f,
                                                100.0f);

        shaderProgram->setMatrix4fv("view", glm::value_ptr(view));
        shaderProgram->setMatrix4fv("projection", glm::value_ptr(projection));

        texture->bind(0);
        shaderProgram->use();
        vao->bind();
        gl->glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, nullptr);
        context->swapBuffers(this);

    }

    void Renderer::renderNow() {
        if (!isExposed()) return;
        if (!initialized) {
            initialize();
        };
        render();
    }

    bool Renderer::event(QEvent *event) {
        switch (event->type()) {
            case QEvent::UpdateRequest:
                renderNow();
                return true;
            default:
                return QWindow::event(event);

        }
    }

    void Renderer::exposeEvent(QExposeEvent *event) {
        if (isExposed()) {
            renderNow();
        }
    }

}
