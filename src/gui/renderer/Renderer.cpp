//
// Created by tomas on 06.10.21.
//

#include <iostream>
#include <renderer/Renderer.h>
#include <geGL/geGL.h>
#include <memory>
#include <thread>
#include <csignal>
#include <config/ConfigReader.h>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QtConcurrent/QtConcurrent>
#include <glm/ext/matrix_transform.hpp>
#include <shaders_gui/Shaders.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QGuiApplication>

namespace MapGenerator {
    Renderer::Renderer(QWindow *parent) {

        initialized = false;
        context = nullptr;
        setSurfaceType(
                QWindow::OpenGLSurface); //this needs to be set otherwise makeCurrent and other gl context related functions will fail
        surfaceFormat.setVersion(4, 5);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        surfaceFormat.setDepthBufferSize(8);
        setupLib();

    }

    bool Renderer::setupLib() {
        bool hasError = false;
        //TODO: fix this - it can crash when there is no launch argument
        auto config = ConfigReader::read(QCoreApplication::arguments().at(1).toStdString(), &hasError);
        if (hasError) {
            return false;
        }
        GeneratorOptions options;

        std::vector<double> posHome{
                49.883325913713, 17.8657865524292, 49.89402618295204, 17.890548706054688
        };

        std::vector<double> posHomeL{
                49.96736286729904, 17.860572894482512,

                49.8718795233479, 17.955027618972238};

        std::vector<double> posBrno = {
                49.19256141221154, 16.594543972568715,
                49.19827707820228, 16.604973078397315
        };

        std::vector<double> posRand = {
                49.9619918488622, 17.8586352852208, 49.91684506727818, 17.94300583538308

        };

        std::vector<double> posMountains = {
                50.10588121964279, 17.198770606455174,
                50.05709781257081, 17.28661015961763

        };
        std::vector<double> posBrnoVeryLarge{
                49.23019297366651, 16.565201713369547, 49.171611576900936, 16.71542469343281
        };


        std::vector<double> posBrnoMar{
                49.22264878390983, 16.695949499486055,
                49.20984129157041, 16.72238587076677
        };

        std::vector<double> brazil{
                -22.940051163948276, -43.226979529278665,
                -22.96603878773571, -43.18380161954447
        };

        std::vector<double> moni{
                49.206206330276416, 16.693695548101253,
                49.20443981462129, 16.697611471199462
        };

        std::vector<double> nassfeld{
                46.631426377462304, 13.222294893455746,
                46.55290962338361, 13.297562841791274
        };

        auto currentPos = posHome;

        options.lat1 = currentPos[0];
        options.lon1 = currentPos[1];
        options.lat2 = currentPos[2];
        options.lon2 = currentPos[3];
        options.terrainResolution = 32;
        this->mapGenerator = std::make_shared<MapGenerator>(config, options);
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
            //Set up the camera
            camera = std::make_shared<Camera>((Renderer *) this);
            connect(this, SIGNAL(keyPressEvent(QKeyEvent * )), camera.get(), SLOT(keyEvent(QKeyEvent * )));
            connect(this, SIGNAL(keyReleaseEvent(QKeyEvent * )), camera.get(), SLOT(keyEvent(QKeyEvent * )));
            connect(this, SIGNAL(mouseMoveEvent(QMouseEvent * )), camera.get(), SLOT(mouseMoved(QMouseEvent * )));
        }
        //set up the context and get ready for rendering
        context->makeCurrent(this);
        ge::gl::init();
        gl = std::make_shared<ge::gl::Context>();
        gl->glEnable(GL_DEPTH_TEST);

        //start rendering loop
        renderTimer = std::make_unique<QTimer>(this);
        //grab the monitor refresh rate and set the timer to that
        auto screen = QGuiApplication::primaryScreen();
        refreshRate = 1000 / screen->refreshRate();
        renderTimer->setInterval((int) refreshRate);
        connect(renderTimer.get(), SIGNAL(timeout()), this, SLOT(renderNow()));
        renderTimer->start();
        initialized = true;
        //Grab the map and create the scene

        auto map = mapGenerator->generateMap();

        //Prepare gBuffer render targets
        gBuffer = std::make_shared<ge::gl::Buffer>(GL_FRAMEBUFFER);
        prepareGBufferTextures();

        //Prepare the quad that will get rendered
        float vertices[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f
        };
        this->quadBuffer = std::make_shared<ge::gl::Buffer>(12 * sizeof(float), vertices);
        this->quadVAO = std::make_shared<ge::gl::VertexArray>();
        this->quadVAO->bind();
        quadVAO->addAttrib(quadBuffer, 0, 3, GL_FLOAT, 3 * sizeof(float), 0);

        //Prepare the shaders for the lightning pass
        this->lightningVS = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, GUIShaders::getLightningVS());
        this->lightningFS = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, GUIShaders::getLightningFS());
        this->lightningProgram = std::make_shared<ge::gl::Program>(lightningVS, lightningFS);

        scene = std::make_shared<Scene3D>(map, gl, camera, gBuffer->getId());
    }

    void Renderer::prepareGBufferTextures() {
        int width = this->width();
        int height = this->height();
        gPosition = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGBA16F, 0, width, height);
        gPosition->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gPosition->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->getId(), 0);

        gNormal = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGBA16F, 0, width, height);
        gNormal->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gNormal->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal->getId(), 0);

        gAlbedo = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGBA, 0, width, height);
        gAlbedo->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gAlbedo->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo->getId(), 0);

        unsigned int att[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        gl->glDrawBuffers(3, att);

        this->rboDepth = std::make_shared<ge::gl::Renderbuffer>(GL_DEPTH_COMPONENT32F, width, height);
        rboDepth->setStorage(GL_DEPTH_COMPONENT, width, height);
        gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth->getId());
        // finally check if framebuffer is complete
        if (gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::render() {
        const qreal retinaScale = devicePixelRatio();
        clearView();
        if (scene != nullptr) {
            gl->glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->getId());
            //scene->draw(height(), width(), retinaScale);
        }
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderQuad();
        context->swapBuffers(this);
    }

    void Renderer::renderQuad() {
        gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gNormal->bind(0);
        gPosition->bind(1);
        gAlbedo->bind(2);
        lightningProgram->use();
        quadVAO->bind();
        gl->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void Renderer::clearView() {

        //PROBABLY FUCKED TODO FIX
        const qreal retinaScale = devicePixelRatio();
        gBuffer->bind(GL_FRAMEBUFFER);
        gl->glViewport(0, 0, width() * retinaScale, height() * retinaScale);
        gl->glClearColor(0, 0, 0, 1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
            case QEvent::Close:
                //TODO stop render thread
                //deleteLater();
                return QWindow::event(event);
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
