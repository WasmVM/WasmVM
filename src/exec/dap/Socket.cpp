// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Socket.hpp"

#include <exception.hpp>

#include <charconv>
#include <cstring>

#ifdef _WIN32
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#  include <cerrno>
#endif

using namespace Dap;
using WasmVM::Exception::Exception;

namespace {

#ifdef _WIN32
// Winsock must be started once per process before any other call.
struct WinsockInit {
    WinsockInit(){
        WSADATA data;
        WSAStartup(MAKEWORD(2, 2), &data);
    }
    ~WinsockInit(){
        WSACleanup();
    }
};
void ensure_winsock(){
    static WinsockInit init;
}
void close_handle(socket_handle handle){
    ::closesocket((SOCKET)handle);
}
bool handle_invalid(socket_handle handle){
    return (SOCKET)handle == INVALID_SOCKET;
}
#else
void ensure_winsock(){}
void close_handle(socket_handle handle){
    ::close(handle);
}
bool handle_invalid(socket_handle handle){
    return handle < 0;
}
#endif

}

Endpoint Endpoint::parse(const std::string& spec){
    Endpoint endpoint {"127.0.0.1", 0};
    std::string port_text = spec;
    if(!spec.empty() && spec.front() == '['){
        size_t close = spec.find(']');
        if(close == std::string::npos || close + 1 >= spec.size() || spec[close + 1] != ':'){
            throw Exception("--dap: expected [address]:port, got '" + spec + "'");
        }
        endpoint.host = spec.substr(1, close - 1);
        port_text = spec.substr(close + 2);
    }else if(size_t colon = spec.rfind(':'); colon != std::string::npos){
        if(spec.find(':') != colon){
            throw Exception("--dap: an IPv6 address must be written in brackets, got '" + spec + "'");
        }
        endpoint.host = spec.substr(0, colon);
        port_text = spec.substr(colon + 1);
    }
    if(endpoint.host.empty()){
        throw Exception("--dap: empty address in '" + spec + "'");
    }
    unsigned long port = 0;
    auto [ptr, ec] = std::from_chars(port_text.data(), port_text.data() + port_text.size(), port);
    if(port_text.empty() || ec != std::errc() || ptr != port_text.data() + port_text.size() || port > 65535){
        throw Exception("--dap: invalid port in '" + spec + "'");
    }
    endpoint.port = (uint16_t)port;
    return endpoint;
}

Connection::Connection(Connection&& other) noexcept : handle(other.handle), valid(other.valid){
    other.valid = false;
}

Connection& Connection::operator=(Connection&& other) noexcept {
    if(this != &other){
        close();
        handle = other.handle;
        valid = other.valid;
        other.valid = false;
    }
    return *this;
}

Connection::~Connection(){
    close();
}

size_t Connection::read(char* buffer, size_t size){
    if(!valid){
        return 0;
    }
    while(true){
#ifdef _WIN32
        int got = ::recv((SOCKET)handle, buffer, (int)size, 0);
        if(got == SOCKET_ERROR){
            return 0;
        }
#else
        ssize_t got = ::recv(handle, buffer, size, 0);
        if(got < 0){
            if(errno == EINTR){
                continue;
            }
            return 0;
        }
#endif
        return (size_t)got;
    }
}

bool Connection::write(const std::string& data){
    if(!valid){
        return false;
    }
    size_t sent = 0;
    while(sent < data.size()){
#ifdef _WIN32
        int put = ::send((SOCKET)handle, data.data() + sent, (int)(data.size() - sent), 0);
        if(put == SOCKET_ERROR){
            return false;
        }
#else
#  ifdef MSG_NOSIGNAL
        // A closed peer must not raise SIGPIPE and kill the guest's process.
        ssize_t put = ::send(handle, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
#  else
        ssize_t put = ::send(handle, data.data() + sent, data.size() - sent, 0);
#  endif
        if(put < 0){
            if(errno == EINTR){
                continue;
            }
            return false;
        }
#endif
        sent += (size_t)put;
    }
    return true;
}

void Connection::shutdown(){
    if(valid){
#ifdef _WIN32
        ::shutdown((SOCKET)handle, SD_BOTH);
#else
        ::shutdown(handle, SHUT_RDWR);
#endif
    }
}

void Connection::close(){
    if(valid){
        close_handle(handle);
        valid = false;
    }
}

Listener::Listener(const Endpoint& endpoint){
    ensure_winsock();
    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    addrinfo* found = nullptr;
    std::string port = std::to_string(endpoint.port);
    if(int err = ::getaddrinfo(endpoint.host.c_str(), port.c_str(), &hints, &found); err != 0 || found == nullptr){
        throw Exception("--dap: cannot resolve address '" + endpoint.host + "'");
    }
    std::string error = "--dap: cannot listen on " + endpoint.host + ":" + port;
    for(addrinfo* info = found; info != nullptr; info = info->ai_next){
#ifdef _WIN32
        socket_handle candidate = (socket_handle)::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
#else
        socket_handle candidate = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
#endif
        if(handle_invalid(candidate)){
            continue;
        }
#ifndef _WIN32
        // Let a debug session restart on the same port right away.
        int reuse = 1;
        ::setsockopt(candidate, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif
#ifdef _WIN32
        bool bound = ::bind((SOCKET)candidate, info->ai_addr, (int)info->ai_addrlen) == 0
            && ::listen((SOCKET)candidate, 1) == 0;
#else
        bool bound = ::bind(candidate, info->ai_addr, info->ai_addrlen) == 0
            && ::listen(candidate, 1) == 0;
#endif
        if(bound){
            handle = candidate;
            valid = true;
            break;
        }
        close_handle(candidate);
    }
    ::freeaddrinfo(found);
    if(!valid){
        throw Exception(error);
    }
}

Listener::~Listener(){
    close();
}

void Listener::close(){
    if(valid){
        close_handle(handle);
        valid = false;
    }
}

std::string Listener::address() const {
    sockaddr_storage storage;
    socklen_t length = sizeof(storage);
#ifdef _WIN32
    if(::getsockname((SOCKET)handle, (sockaddr*)&storage, &length) != 0){
#else
    if(::getsockname(handle, (sockaddr*)&storage, &length) != 0){
#endif
        return "?";
    }
    char host[INET6_ADDRSTRLEN] = {0};
    if(storage.ss_family == AF_INET6){
        sockaddr_in6* in6 = (sockaddr_in6*)&storage;
        ::inet_ntop(AF_INET6, &in6->sin6_addr, host, sizeof(host));
        return std::string("[") + host + "]:" + std::to_string(ntohs(in6->sin6_port));
    }
    sockaddr_in* in4 = (sockaddr_in*)&storage;
    ::inet_ntop(AF_INET, &in4->sin_addr, host, sizeof(host));
    return std::string(host) + ":" + std::to_string(ntohs(in4->sin_port));
}

Connection Listener::accept(){
#ifdef _WIN32
    socket_handle client = (socket_handle)::accept((SOCKET)handle, nullptr, nullptr);
#else
    socket_handle client;
    do {
        client = ::accept(handle, nullptr, nullptr);
    } while(handle_invalid(client) && errno == EINTR);
#endif
    if(handle_invalid(client)){
        throw Exception("--dap: accept failed");
    }
#if defined(__APPLE__)
    // macOS has no MSG_NOSIGNAL; suppress SIGPIPE on the socket instead.
    int nosigpipe = 1;
    ::setsockopt(client, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe));
#endif
    return Connection(client);
}
