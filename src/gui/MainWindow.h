//
// Created by tomas on 02.10.21.
//

#pragma once

#include <QMainWindow>
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include "renderer/Renderer.h"
#include <ui/rendererlayout.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Renderer;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    void drawMap(glm::vec4 area);

private:
    Ui::MainWindow *ui;
    //Renderer* renderer;
    QLayout *baseGridLayout;


    void onMapLoaded();

    bool isRendering;
    RendererLayout *rendererLayout;
};


