#include <iostream>
#include <string>
#include <napi.h>
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include "nstool/main.cpp"

#pragma message("Importing the Node API wrapper")

using namespace Napi;

std::vector<std::string> BuildParameters(const CallbackInfo& info) {
    std::vector<std::string> parameters = {};

    for (int i = 0; i < info.Length(); i = i + 1) {
        std::string str = "";

        // Copy the string to char before adding it to the parameter array.
        std::strcpy(str, info[i].ToString().Utf8Value());

        parameters[i] = info[i].ToString().Utf8Value();
    }

    return parameters;
}

String Run(const CallbackInfo& info) {
    return String::New(info.Env(), umain(BuildParameters(info), info.Env()));
}

Object InitAll(Env env, Object exports) {
    exports.Set("run", Function::New(env, Run));

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);
