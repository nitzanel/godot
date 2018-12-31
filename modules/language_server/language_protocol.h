#ifndef LANGUAGE_PROTOCOL_H
#define LANGUAGE_PROTOCOL_H

#include "core/ustring.h"
#include "core/variant.h"
#include "core/vector.h"


namespace gdls {
enum JSONRPCErrorCodes {
    	// Defined by JSON RPC
	PARSE_ERROR = -32700,
	INVALID_REQUEST = -32600,
	METHOD_NOT_FOUND = -32601,
	INVALID_PARAMS = -32602,
	INTERNAL_ERROR = -32603,
	SERVER_ERROR_START = -32099,
	SERVER_ERROR_END = -32000,
	SERVER_NOT_INITIALIZED = -32002,
	UNKNOWN_ERROR_CODE = -32001,

	// DEFINED BY THE PROTOCOL.
	REQUEST_CANCELLED = -32800,
	CONTENT_MODIFIED = -32801,
};


enum MessageType {
    REQUEST,
    RESPONSE,
    NOTIFICATION,
};


/*
 * Position in a text document expressed as zero-based line and zero-based character offset.
 * A position is between two characters like an ‘insert’ cursor in a editor.
 */
struct TextPosition {
	/**
	 * Line position in a document (zero-based).
	 */
    int line;

	/**
	 * Character offset on a line in a document (zero-based). Assuming that the line is
	 * represented as a string, the `character` value represents the gap between the
	 * `character` and `character + 1`.
	 *
	 * If the character value is greater than the line length it defaults back to the
	 * line length.
	 */
    int character;
};


/*
 * A range in a text document expressed as (zero-based) start and end positions.
 * A range is comparable to a selection in an editor.
 * Therefore the end position is exclusive. 
 */
struct TextRange {
    TextPosition start;
    TextPosition End;
};

using DocumentUri = String;

/*
 * Represents a location inside a resource, such as a line inside a text file.
 */
struct Location {
    DocumentUri uri;
    TextRange range;
};


/*
 * Represents a link between a source and a target location.
 */
struct LocationLink {
    /**
	 * Span of the origin of this link.
	 *
	 * Used as the underlined span for mouse interaction. Defaults to the word range at
	 * the mouse position.
	 */
    TextRange originSelectionRange;

	/**
	 * The target resource identifier of this link.
	 */
    String targetUri;
	/**
	 * The full target range of this link.
	 */

    TextRange targetRange;
	/**
	 * The span of this link.
	 */
    TextRange targetSelectionRange;
};


enum DiagnosticSeverity {
    ERROR = 1,
    WARNING = 2,
    INFORMATION = 3,
    HINT = 4,
};

/**
 * Represents a related message and source code location for a diagnostic. This should be
 * used to point to code locations that cause or related to a diagnostics, e.g when duplicating
 * a symbol in a scope.
 */
struct DiagnosticRelatedInformation {
    /**
	 * The location of this related diagnostic information.
	 */
    Location location;

	/**
	 * The message of this related diagnostic information.
	 */
    String message;
};


struct Diagnostic {
    /**
     * The range at which the message applies.
	 */
    TextRange range;

    DiagnosticSeverity severity;

	/**
	 * The diagnostic's code, which might appear in the user interface.
	 */
    int code;

	/**
	 * A human-readable string describing the source of this
	 * diagnostic, e.g. 'godot' or 'super gd lint'.
	 */
    String source;

	/**
	 * The diagnostic's message.
	 */
    String message;

	/**
	 * An array of related diagnostic information, e.g. when symbol-names within
	 * a scope collide all definitions can be marked via this property.
	 */
	Vector<DiagnosticRelatedInformation> relatedInformation;
};


struct Command {
    /**
	 * Title of the command, like `save`.
	 */
    String title;

	/**
	 * The identifier of the actual command handler.
	 */
    String command;

	/**
	 * Arguments that the command handler should be
	 * invoked with.
	 */
    Variant arguments;
};


struct TextEdit {
	/**
	 * The range of the text document to be manipulated. To insert
	 * text into a document create a range where start === end.
	 */
	TextRange range;

	/**
	 * The string to be inserted. For delete operations use an
	 * empty string.
	 */
	String newText;
};

using VersionedTextDocumentIdentifier = int;


struct TextDocumentEdit {
	/**
	 * The text document to change.
	 */
	VersionedTextDocumentIdentifier textDocument;

	/**
	 * The edits to be applied.
	 */
	Vector<TextEdit> edits;

};

struct ResponseError {
    int code;
    String message;
    Variant data;
};

struct Message {
    String header;
    String content;
};

struct JsonRpcMessage {
    String jsonrpc;
};

struct RequestMessage: public JsonRpcMessage {
    Variant id;
    String method;
    Variant params;
};


struct ResponseMessage: public JsonRpcMessage { 
    Variant id;
    Variant result;
    ResponseError error;
};

Message read_message(const String& message_string);
int get_content_length(const Vector<String>& header);
String get_content_type(const Vector<String>& header);

String get_header_value(const Vector<String>& header, const String& field);
Vector<String> split_header_to_fields(const String& header);
const String header_content_seperator = "\r\n\r\n";
const String header_seperator = "\r\n";


}

#endif // LANGUAGE_PROTOCOL_H
