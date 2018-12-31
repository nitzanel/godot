#include "language_protocol.h"
#include "core/ustring.h"
#include "core/vector.h"
#include "core/print_string.h"

namespace gdls {
    Message read_message(const String& message_string) {
        Message message{};
        Vector<String> split = message_string.split("\r\n\r\n", true, 1);
        if (split.size() != 2) {
            // TODO: message parse failure
            ERR_PRINT("[LanguageServer] Message failed to parse");
            print_line(message_string);
            return message;
        }
        message.header = split[0];
        message.content = split[1];
        return message;
    }

    String get_header_value(const Vector<String>& header, const String& field) {
        for (size_t i = 0; i < header.size(); i++) {
            if (header[i].begins_with(field)) {
                return header[i].split(": ")[1];
            }
        }
        ERR_PRINT("[LanguageProtocol] Failed to find header field: ");
        print_line(field);
        return "";
    }

    Vector<String> split_header_to_fields(const String& header) {
        return header.split(header_seperator);
    }

    int get_content_length(const Vector<String>& header) {
        return get_header_value(header, "Content-Length").to_int();
    }

    String get_content_type(const Vector<String>& header) {
        return get_header_value(header, "Content-Type");
    }
}
