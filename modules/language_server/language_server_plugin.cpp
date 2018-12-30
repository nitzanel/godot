#include <core/print_string.h>
#include <editor/editor_node.h>
#include "language_server_plugin.h"

namespace gdls {
    LanguageServerPlugin::LanguageServerPlugin(EditorNode* pEditor): editor(pEditor) {
        server = memnew(LanguageServer);

    }

    LanguageServerPlugin::~LanguageServerPlugin() {
        memdelete(server);
        // Empty
    }

    void LanguageServerPlugin::_notification(int p_what) {
        switch (p_what) {
            case NOTIFICATION_ENTER_TREE:
                print_line(String("LanguageServerPlugin Entered Tree\n"));
                server->start(32311);
                break;
            case NOTIFICATION_EXIT_TREE:
                print_line(String("LanguageServerPlugin Exited Tree\n"));
                break;
            default:
                break;
        }
    }

    void LanguageServerPlugin::_bind_methods() {
        ClassDB::bind_method(D_METHOD("_notification", "p_what"), &LanguageServerPlugin::_notification);
    }


}
