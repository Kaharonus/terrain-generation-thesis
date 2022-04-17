//
// Created by tomas on 29.09.21.
//

#pragma once


#include <string>
#include <utility>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace MapGenerator {
    class ApiBase {
    public:
        ApiBase(std::string apiKey);

        template<class T>
        std::shared_ptr<T> sendRequest(std::string uri, int* size);

        template<class T>
        std::shared_ptr<T> postRequest(std::string uri, const std::string &bodyType, const std::string& body);

    private:
        virtual std::string getBaseAddress() = 0;
        std::map<uint32_t , std::shared_ptr<void>> requestCache;

    protected:
        std::string apiKey;

        std::string encodeUrl(const std::string &url);

        template<class T>std::shared_ptr<T> readData(std::string path);

        static uint32_t hash(const std::string &str);
    };

    void getMetadata();
};

#include "ApiBase_impl.h"
