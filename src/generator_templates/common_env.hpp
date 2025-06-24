#pragma once
#include <stdexcept>
#include "svd_types.hpp"
#include "fmt_wrapper.hpp"
#include "inja_wrapper.hpp"

namespace Generator {
inline void setCommonEnv(inja::Environment& env) {
    env.add_callback("hex", 2, [](inja::Arguments& args) {
        auto number   = args.at(0)->get<std::uint64_t>();
        auto dataType = args.at(1)->get<std::string>();

        unsigned width{};
        if(dataType == "u8" || dataType == "b") {
            width = 2;
        } else if(dataType == "u16") {
            width = 4;
        } else if(dataType == "u32") {
            width = 8;
        } else if(dataType == "u64") {
            width = 16;
        } else if(dataType == "min") {
            width = 2;
        } else {
            throw std::runtime_error("hex failed");
        }

        return fmt::format("0x{:0{}X}", number, width);
    });

    env.add_callback("dataType", 1, [](inja::Arguments& args) {
        auto dataType = args.at(0)->get<std::string>();

        if(dataType == "b") {
            return std::string{"bool"};
        }

        if(dataType != "u8" && dataType != "u16" && dataType != "u32" && dataType != "u64") {
            throw std::runtime_error(dataType + " dataType failed");
        }

        dataType.erase(0, 1);

        return fmt::format("std::uint{}_t", dataType);
    });
}
}  // namespace Generator
