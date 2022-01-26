//
// Created by tomas on 06.10.21.
//

#pragma once


#include <QExposeEvent>
#include <geGL/OpenGLContext.h>
#include <QWidget>
#include <QSurfaceFormat>
#include <QWindow>
#include <QOpenGLContext>
#include <renderer/Camera.h>
#include <MapGenerator.h>
#include <QThread>
#include <QResizeEvent>
#include <renderer/Scene3D.h>

namespace MapGenerator {
    class Camera;
    class Scene3D;
    class Renderer : public QWindow {
    Q_OBJECT
    public:
        explicit Renderer(QWindow *parent = nullptr);

        virtual void render();

        virtual void initialize();

    public slots:

        void renderNow();

    signals:

        void keyPressEvent(QKeyEvent *event) override;

        void keyReleaseEvent(QKeyEvent *event) override;
        void mouseMoveEvent(QMouseEvent *) override;
        void resizeEvent(QResizeEvent *) override;

    protected:
        bool event(QEvent *event) override;
        void renderLoop();
        void exposeEvent(QExposeEvent *event) override;

        std::shared_ptr<MapGenerator> mapGenerator;

        std::shared_ptr<Scene3D> scene;
        bool renderLoopRunning = false;

        bool initialized;
        QOpenGLContext *context;
        std::shared_ptr<ge::gl::Context> gl;
        std::shared_ptr<Camera> camera;
        QSurfaceFormat surfaceFormat;
        std::unique_ptr<QTimer> renderTimer;
        bool setupLib();

        void createTexture(const std::vector<float> &data, int width, int height);

        void handleFinished();

        void startTextureGeneration(const std::vector<double> &draw, const int &resolution);

        void clearView();
    };

}



