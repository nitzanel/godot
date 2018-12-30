#include "register_types.h"
#include "language_server_plugin.h"
#include "language_server.h"
using namespace gdls;

void register_language_server_types() {
#ifdef TOOLS_ENABLED
    EditorPlugins::add_by_type<LanguageServerPlugin>();
#endif
}

void unregister_language_server_types() {

}
