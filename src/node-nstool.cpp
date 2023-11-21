#include <iostream>
#include <string>
#include <napi.h>
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include "nstool/main.h"

#pragma message("Importing the Node API wrapper")

std::vector<std::string> BuildParameters(const Napi::CallbackInfo& info) {
    std::vector<std::string> parameters = {};

    for (int i = 0; i < info.Length(); i = i + 1) {
        parameters.push_back(info[i].ToString().Utf8Value());
    }

    return parameters;
}

Napi::String Run(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), start(BuildParameters(info), info.Env()));
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    exports.Set("run", Napi::Function::New(env, Run));

    return exports;
}

NODE_API_MODULE(addon, InitAll);
