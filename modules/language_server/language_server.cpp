#include "language_server.h"
#include "language_protocol.h"
#include <core/ustring.h>
#include <core/print_string.h>
#include <core/io/tcp_server.h>
#include <core/error_macros.h>


namespace gdls {
    String LanguageServer::recieve_header(const Vector<char>& message_bytes) {
        if (message_bytes.size() == 0) {
            return "";
        }
        String message_string = String::utf8(reinterpret_cast<const char*>(message_bytes.ptr()));
        if (message_string.find(header_content_seperator) > 0) {
            return message_string.split(header_content_seperator, true, 1)[0];
        }
        return "";
    }


    String LanguageServer::recieve_content(LanguageServer* pLanguageServer, int content_length, int header_length) {
        int message_content_left_to_read = content_length - (pLanguageServer->read_offset - header_length);
        if (message_content_left_to_read > 0) {
            Vector<char> buffer;
            buffer.resize(message_content_left_to_read);
            pLanguageServer->client_connection->get_data(reinterpret_cast<uint8_t*>(buffer.ptrw()), message_content_left_to_read);
            pLanguageServer->buffer.append_array(buffer);
        }
        const uint8_t *content_start = reinterpret_cast<const uint8_t*>(pLanguageServer->buffer.ptr() + (sizeof(uint8_t) * header_length));
        return String::utf8(reinterpret_cast<const char*>(content_start));
    }


    void LanguageServer::remove_message_from_buffer(Vector<char>& buffer, int message_length) {
        if (buffer.size() < message_length) {
            message_length = buffer.size();
        }
        Vector<char> tmp;
        size_t to_copy = buffer.size() - message_length;
        tmp.resize(to_copy);
        memcpy(tmp.ptrw(), buffer.ptr(), to_copy); 
        buffer.clear();
        buffer.append_array(tmp);
    }


    int LanguageServer::recieve_bytes(LanguageServer* pLanguageServer) {
        int to_read = pLanguageServer->client_connection->get_available_bytes();
        if (to_read == 0) {
            return 0;
        }
        Vector<char> buffer;
        buffer.resize(to_read);
        pLanguageServer->client_connection->get_data(reinterpret_cast<uint8_t*>(buffer.ptrw()), to_read);
        pLanguageServer->buffer.append_array(buffer);
        return to_read;
    }


    Message LanguageServer::recieve_message(LanguageServer* pLanguageServer) {
        Message message;
        String header;
        String content;
        while (header.length() == 0) {
            pLanguageServer->read_offset += recieve_bytes(pLanguageServer);
            header = recieve_header(pLanguageServer->buffer);
        }
        int header_length = header.length() + header_content_seperator.length();
        Vector<String> header_fields = split_header_to_fields(header);
        int content_length = get_content_length(header_fields);
        content = recieve_content(pLanguageServer, content_length, header_length);
        remove_message_from_buffer(pLanguageServer->buffer, header_length + content_length);
        pLanguageServer->read_offset -= (header_length + content_length);
        if (content.length() == 0) {
            ERR_PRINT("[LanguageServer] Failed to read message content\n");
        }
        print_line("header: " + header);
        print_line("content: " + content);
        message.header = header;
        message.content = content;
        return message;
    }

    void LanguageServer::handle_connection(void *pLanguageServer) {
        LanguageServer *languageServer = static_cast<LanguageServer*>(pLanguageServer);
        // languageServer->connection->set_no_delay()
        while (languageServer->cmd == ServerCommand::HANDLE_CONNECTION) {
            while (languageServer->client_connection->get_status() == StreamPeerTCP::Status::STATUS_CONNECTED) {
                recieve_message(languageServer);
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
