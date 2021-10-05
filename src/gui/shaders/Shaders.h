//
// Created by tomas on 03.10.21.
//

#pragma once

//Ignore the fact that if you look at the autogenerated include/shader.vs and .fs files they are empty.
//They get filled up at compile time with data from vertex.shader and fragment.shader
const char* VertexSource =
#include "include/shader.vs"
;

const char *FragmentSource =
#include "include/shader.fs"
;

