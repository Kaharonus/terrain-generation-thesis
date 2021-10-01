//
// Created by tomas on 30.09.21.
//

#include "BingApi.h"

#include <utility>

std::string BingApi::getBaseAddress() {
    return "http://dev.virtualearth.net/REST/v1/";
}

BingApi::BingApi(std::string key) : ApiBase(std::move(key)){
}

quicktype::ElevationResult* BingApi::getElevation() {
    //auto url = getBaseAddress() + "Elevation/Bounds?bounds=49.210677743172724, 16.62863105170431, 49.21309576479339, 16.625380048112635&rows=10&cols=10&key=" + this->apiKey;
    auto url = "http://www.httpbin.org/get";
    auto result = this->sendRequest<quicktype::ElevationResult*>(url);
    return nullptr;
}
