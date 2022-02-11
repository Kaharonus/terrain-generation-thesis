//
// Created by tomas on 08.02.22.
//


#pragma once

#include <string>
#include<memory>
#include <api/models/openstreetmap/OSMData.h>
#include <scene/Model.h>
#include <scene/Vertex.h>
#include <config/GeneratorOptions.h>
#include <api/models/bing/ElevationData.h>

namespace MapGenerator{
    class BuildingsGenerator {
    public:
        BuildingsGenerator(const std::shared_ptr<OSMData>& metadata, const std::shared_ptr<ElevationData>& heightData,GeneratorOptions options);
        std::shared_ptr<Model> generate();
    private:
        Vertex normaliseVertex(Vertex vertex);
        float scale;
        bool updateZ = false;
        std::shared_ptr<OSMData> data;
        std::shared_ptr<ElevationData> heightData;
        GeneratorOptions options;
        std::vector<Node> buildings;
    };

}



