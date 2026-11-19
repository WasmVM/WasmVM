// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Framing.hpp"

#include <exception.hpp>

#include <cctype>
#include <charconv>

using namespace Dap;
using WasmVM::Exception::Exception;

void MessageReader::feed(const char* data, size_t size){
    buffer.append(data, size);
}

std::optional<std::string> MessageReader::next(){
    size_t header_end = buffer.find("\r\n\r\n");
    if(header_end == std::string::npos){
        return std::nullopt;
    }
    // Header fields are "Name: value" lines; only Content-Length is required
    // and the others (Content-Type) are ignored.
    std::optional<size_t> length;
    size_t line_start = 0;
    while(line_start < header_end){
        size_t line_end = buffer.find("\r\n", line_start);
        std::string line = buffer.substr(line_start, line_end - line_start);
        line_start = line_end + 2;
        size_t colon = line.find(':');
        if(colon == std::string::npos){
            throw Exception("DAP: malformed header line '" + line + "'");
        }
        std::string name = line.substr(0, colon);
        for(char& c : name){
            c = (char)std::tolower((unsigned char)c);
        }
        if(name != "content-length"){
            continue;
        }
        size_t value_start = colon + 1;
        while(value_start < line.size() && line[value_start] == ' '){
            ++value_start;
        }
        size_t value = 0;
        const char* first = line.data() + value_start;
        const char* last = line.data() + line.size();
        auto [ptr, ec] = std::from_chars(first, last, value);
        if(first == last || ec != std::errc() || ptr != last){
            throw Exception("DAP: invalid Content-Length '" + line + "'");
        }
        length = value;
    }
    if(!length){
        throw Exception("DAP: header without Content-Length");
    }
    size_t body_start = header_end + 4;
    if(buffer.size() - body_start < length.value()){
        return std::nullopt;
    }
    std::string body = buffer.substr(body_start, length.value());
    buffer.erase(0, body_start + length.value());
    return body;
}

std::string Dap::frame(const std::string& body){
    return "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
}
