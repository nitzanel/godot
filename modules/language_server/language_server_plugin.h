#ifndef EDITOR_SERVER_PLUGIN_H
#define EDITOR_SERVER_PLUGIN_H

#include <editor/editor_plugin.h>
#include "language_server.h"

namespace gdls {
    class LanguageServerPlugin: public EditorPlugin {
        GDCLASS(LanguageServerPlugin, EditorPlugin);
        EditorNode *editor;
        LanguageServer *server;
        protected:
        static void _bind_methods();
        void _notification(int p_what);
        public:
        LanguageServerPlugin(EditorNode* editor);
        ~LanguageServerPlugin();
    };

}

#endif // EDITOR_SERVER_PLUGIN_H
