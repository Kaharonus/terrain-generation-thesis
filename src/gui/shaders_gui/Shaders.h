//
// Created by tomas on 30.03.22.
//
#pragma once

const char *lightning_vs =
#include <shaders_gui/include/lightning/lightning_vs.shader.out>
        ;

const char *lightning_fs =
#include <shaders_gui/include/lightning/lightning_fs.shader.out>
        ;

const char *ssao_fs =
#include <shaders_gui/include/ssao/ssao_fs.shader.out>
        ;


class GUIShaders {
public:
    static const char *getLightningVS() {
        return lightning_vs;
    }

    static const char *getLightningFS() {
        return lightning_fs;
    }
    static const char *getSSAOFS() {
        return ssao_fs;
    }
    static const char* getSSAOVS(){
        return getLightningVS();
    }
};

