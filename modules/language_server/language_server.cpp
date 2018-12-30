#include "language_server.h"
#include <core/ustring.h>
#include <core/print_string.h>
#include <core/io/tcp_server.h>
#include <core/error_macros.h>


namespace gdls {

    void LanguageServer::handle_connection(void *pLanguageServer) {
        LanguageServer *languageServer = static_cast<LanguageServer*>(pLanguageServer);
        while (languageServer->cmd == ServerCommand::HANDLE_CONNECTION) {
            while (languageServer->client_connection->get_status() == StreamPeerTCP::Status::STATUS_CONNECTED) {
                int available_bytes = languageServer->client_connection->get_available_bytes();
                if (available_bytes == 0) {
                    continue;
                }
                print_line("[LanguageServer] recieved packet! length: " + itos(available_bytes));

                uint8_t *data = memnew_arr(uint8_t, available_bytes);
                languageServer->client_connection->get_data(data, available_bytes);
                //print_line(String(data[0]) +  String(data[1]) + String(data[2]));
            }
            // TODO: obviously dont stop
            languageServer->cmd = ServerCommand::STOP;
        }

    }


    void LanguageServer::server_thread_function(void *pLanguageServer) {
        LanguageServer *languageServer = static_cast<LanguageServer*>(pLanguageServer);
        print_line("[LanguageServer] server thread function started");

        while (languageServer->running) {
            switch (languageServer->cmd) {
                case ServerCommand::START:
                    languageServer->server->stop();
                    if (languageServer->server->listen(languageServer->port) == OK) {
                        languageServer->active = true;
                        print_line(String("[LanguageServer] Listening on port: ") + itos(languageServer->port));
                        languageServer->cmd = ServerCommand::WAIT_FOR_CONNECTION;
                    } else {
                        ERR_PRINTS(String("[LanguageServer] Failed to listen on port: ") + itos(languageServer->port));
                    }
                    break;
                case ServerCommand::STOP:
                    languageServer->server->stop();
                    languageServer->active = false;
                    languageServer->running = false;
                    break;
                case ServerCommand::WAIT_FOR_CONNECTION:
                    if (languageServer->server->is_connection_available()) {
                        languageServer->client_connection = languageServer->server->take_connection();
                        if (!languageServer->client_connection.is_valid()) {
                            ERR_PRINT("[LanguageServer] Failed to accept connection.");
                        } else {
                            languageServer->cmd = ServerCommand::HANDLE_CONNECTION;
                        }
                    } else {
                        // TODO: decide if needed
                        OS::get_singleton()->delay_usec(50000);
                    }
                    break;
                case ServerCommand::HANDLE_CONNECTION:
                    handle_connection(pLanguageServer);
                    break;
                default:
                    break;
            }
        }
    }

    LanguageServer::LanguageServer() {
        print_line(String("[LanguageServer] instance created"));
        port = 10011;
        running = true;
        server.instance();
        thread = Thread::create(server_thread_function, this);
    }


    LanguageServer::~LanguageServer() {
    }

    void LanguageServer::start(int port) { 
        port = port;
        cmd = ServerCommand::START;
    }

    void LanguageServer::stop() {
        cmd = ServerCommand::STOP;
    }

}
