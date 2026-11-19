// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The custom "name" section: the spec's subsections and the
// extended-name-section proposal's. A malformed name section (repeated or
// out-of-order subsection, bad length, invalid UTF-8, indices not strictly
// increasing) is dropped as a whole with a warning; the module still loads.

#include <decode.hpp>
#include <exception.hpp>

#include <string>
#include <vector>

using namespace WasmVM;

namespace {

struct malformed {
    std::string reason;
};

// Reads from the section's bytes only, so no length in the section can make
// it read past the section or into the next one.
struct Reader {
    const std::string& bytes;
    size_t pos;
    size_t end;

    bool done() const { return pos >= end; }

    u8_t byte(){
        if(pos >= end){
            throw malformed {"unexpected end"};
        }
        return (u8_t)bytes[pos++];
    }

    u32_t u32(){
        u32_t value = 0;
        for(unsigned shift = 0; shift < 35; shift += 7){
            u8_t b = byte();
            if(shift == 28 && (b & 0x70) != 0){
                throw malformed {"integer too large"};
            }
            value |= (u32_t)(b & 0x7f) << shift;
            if((b & 0x80) == 0){
                return value;
            }
        }
        throw malformed {"integer representation too long"};
    }

    std::string name(){
        u32_t size = u32();
        if(size > end - pos){
            throw malformed {"name length out of bounds"};
        }
        std::string value = bytes.substr(pos, size);
        pos += size;
        if(!valid_utf8(value)){
            throw malformed {"invalid UTF-8 encoding"};
        }
        return value;
    }

    static bool valid_utf8(const std::string& str){
        for(size_t i = 0; i < str.size();){
            u8_t c = (u8_t)str[i];
            size_t len;
            u32_t cp;
            if(c < 0x80){
                i += 1;
                continue;
            }else if((c & 0xe0) == 0xc0){
                len = 2; cp = c & 0x1f;
            }else if((c & 0xf0) == 0xe0){
                len = 3; cp = c & 0x0f;
            }else if((c & 0xf8) == 0xf0){
                len = 4; cp = c & 0x07;
            }else{
                return false;
            }
            if(i + len > str.size()){
                return false;
            }
            for(size_t k = 1; k < len; ++k){
                u8_t cc = (u8_t)str[i + k];
                if((cc & 0xc0) != 0x80){
                    return false;
                }
                cp = (cp << 6) | (cc & 0x3f);
            }
            // Overlong forms, surrogates and values past U+10FFFF are not UTF-8.
            if((len == 2 && cp < 0x80) || (len == 3 && cp < 0x800) || (len == 4 && cp < 0x10000)
                || (cp >= 0xd800 && cp <= 0xdfff) || cp > 0x10ffff){
                return false;
            }
            i += len;
        }
        return true;
    }

    WasmNames::NameMap namemap(){
        WasmNames::NameMap map;
        u32_t count = u32();
        std::optional<index_t> last;
        for(u32_t i = 0; i < count; ++i){
            index_t idx = u32();
            if(last && idx <= last.value()){
                throw malformed {"indices not in increasing order"};
            }
            last = idx;
            map[idx] = name();
        }
        return map;
    }

    WasmNames::IndirectNameMap indirectnamemap(){
        WasmNames::IndirectNameMap map;
        u32_t count = u32();
        std::optional<index_t> last;
        for(u32_t i = 0; i < count; ++i){
            index_t idx = u32();
            if(last && idx <= last.value()){
                throw malformed {"indices not in increasing order"};
            }
            last = idx;
            map[idx] = namemap();
        }
        return map;
    }
};

WasmNames read_names(const std::string& bytes, size_t start){
    WasmNames names;
    Reader reader {bytes, start, bytes.size()};
    std::optional<u8_t> last_id;
    while(!reader.done()){
        u8_t id = reader.byte();
        if(last_id && id <= last_id.value()){
            throw malformed {(id == last_id.value()) ? "repeated subsection" : "subsection out of order"};
        }
        last_id = id;
        u32_t size = reader.u32();
        if(size > reader.end - reader.pos){
            throw malformed {"subsection length out of bounds"};
        }
        Reader sub {bytes, reader.pos, reader.pos + size};
        reader.pos += size;
        switch(id){
            case 0: names.module = sub.name(); break;
            case 1: names.funcs = sub.namemap(); break;
            case 2: names.locals = sub.indirectnamemap(); break;
            case 3: names.labels = sub.indirectnamemap(); break;
            case 4: names.types = sub.namemap(); break;
            case 5: names.tables = sub.namemap(); break;
            case 6: names.mems = sub.namemap(); break;
            case 7: names.globals = sub.namemap(); break;
            case 8: names.elems = sub.namemap(); break;
            case 9: names.datas = sub.namemap(); break;
            case 10: names.fields = sub.indirectnamemap(); break;
            case 11: names.tags = sub.namemap(); break;
            case 12: names.type_params = sub.indirectnamemap(); break;
            case 13: names.tag_params = sub.indirectnamemap(); break;
            default:
                // Unknown subsection: skipped as a whole.
                sub.pos = sub.end;
                break;
        }
        if(!sub.done()){
            throw malformed {"subsection length mismatch"};
        }
    }
    return names;
}

} // namespace

void Decode::read_custom_section(Stream& stream){
    stream.istream.get(); // section id 0x00
    u32_t size = stream.get<u32_t>();
    std::string bytes(size, '\0');
    stream.istream.read(bytes.data(), size);
    bytes.resize((size_t)stream.istream.gcount());
    bool truncated = bytes.size() < size;
    if(truncated){
        // Leave the stream where a section header can be peeked again.
        stream.istream.clear();
    }

    // The custom section's own name. One that cannot be read is not a name
    // section, and other custom sections are skipped.
    std::string section_name;
    size_t start;
    try {
        Reader reader {bytes, 0, bytes.size()};
        section_name = reader.name();
        start = reader.pos;
    }catch(malformed&){
        return;
    }
    if(section_name != "name"){
        return;
    }
    if(stream.name_section_seen){
        Exception::Warning(std::string("ignored a second name section"));
        return;
    }
    stream.name_section_seen = true;
    try {
        if(truncated){
            throw malformed {"section length out of bounds"};
        }
        stream.module.names = read_names(bytes, start);
    }catch(malformed& e){
        stream.module.names = WasmNames();
        Exception::Warning(std::string("malformed name section dropped: ") + e.reason);
    }
}
