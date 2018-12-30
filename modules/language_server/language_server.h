#ifndef LANGUAGE_SERVER_H
#define LANGUAGE_SERVER_H

#include <core/object.h>
#include <core/io/tcp_server.h>
#include <core/variant.h>
#include <core/reference.h>
#include <core/hash_map.h>
#include <core/os/thread.h>
#include "language_protocol.h"

namespace gdls {

enum class ServerCommand {
    START,
    WAIT_FOR_CONNECTION,
    HANDLE_CONNECTION,
    STOP,
    NONE,
};

class LanguageServer: public Object {
    GDCLASS(LanguageServer, Object);

    Ref<TCP_Server> server;
    Ref<StreamPeerTCP> client_connection;
    ServerCommand cmd = ServerCommand::NONE;
    // TODO: better names then active and running
    bool active = false;
    bool running = false;
    Thread *thread;
    int port = 0;
    Vector<String> supportedFeatures = {};

    private:
    static void server_thread_function(void *pLanguageServer);
    static void handle_connection(void *pLanguageServer); 


    
    protected:

    public:
    void start(int port);
    void stop();
    bool is_active() const {return active;}
    int get_port() const {return port;}
    LanguageServer();
    ~LanguageServer();


};

}

#endif // LANGUAGE_SERVER_H
