#include "neko.hpp"
#include "../types.hpp"

#include <v8.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;
extern const SharedGlobals globals;

namespace neko {
    std::string readFile(const char* name)
    {
        std::ifstream file(name);
        if (!file.good())
            return "";

        std::stringstream ss;

        ss << file.rdbuf();

        std::string out = ss.str();

        file.close();

        return out;
    }

    v8::Local<v8::Value> throwException(v8::Local<v8::Context> ctx, const char* message,
            ExceptionType type)
    {
        v8::Isolate *isolate = ctx->GetIsolate();
        v8::Local<v8::Value> err;
        v8::Local<v8::String> str = v8::String::NewFromUtf8(isolate, message).ToLocalChecked();

        switch (type) {
            using enum neko::ExceptionType;
            case RANGE:
                err = v8::Exception::RangeError(str);
                break;
            case REFERENCE:
                err = v8::Exception::ReferenceError(str);
                break;
            case SYNTAX:
                err = v8::Exception::SyntaxError(str);
                break;
            case TYPE:
                err = v8::Exception::TypeError(str);
                break;
            default:
                err = v8::Exception::Error(str);
                break;
        }

        isolate->ThrowException(err);

        return err;
    }

    void printException(v8::Local<v8::Context> ctx, v8::Local<v8::Value> exception)
    {
        v8::Isolate *isolate = ctx->GetIsolate();
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Context::Scope context_scope(ctx);

        v8::String::Utf8Value str(isolate, exception);
        const char* cstr = *str;

        if (!exception->IsNativeError()) {
            fprintf(stderr, "Error: %s\n", cstr);
            return;
        }

        v8::Local<v8::Object> obj = exception->ToObject(ctx).ToLocalChecked();
        v8::Local<v8::Value> name = obj->Get(ctx, v8::String::NewFromUtf8(isolate, "name").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value nameStr(isolate, name);

        v8::Local<v8::Message> msg = v8::Exception::CreateMessage(isolate, exception);
        int scriptId = msg->GetScriptOrigin().ScriptId();
        const auto& module = globals.modules[scriptId];

        int line = msg->GetLineNumber(ctx).FromJust();
        int col = msg->GetStartColumn(ctx).FromJust();

        fprintf(stderr, "──────────────────────────────────────\n");
        if (!module) {
            fprintf(stderr, "%s\n", cstr);
        } else {
            v8::Local<v8::Value> filename = module->GetMetaValue("url").value;
            v8::String::Utf8Value filenameStr(isolate, filename);
            auto path = fs::path(*filenameStr);
            fprintf(stderr, "[%s:%d:%d] %s\n", path.filename().c_str(), line, col, cstr);
        }

        v8::Local<v8::StackTrace> stack = msg->GetStackTrace();
        int length = stack->GetFrameCount();

        for (int i = 0; i < length; i++) {
            v8::Local<v8::StackFrame> frame = stack->GetFrame(isolate, i);
            v8::Local<v8::String> funcName = frame->GetFunctionName();
            int id = frame->GetScriptId();
            const auto& tempModule = globals.modules[id];
            if (tempModule) {
                v8::Local<v8::Value> filename = tempModule->GetMetaValue("url").value;
                v8::String::Utf8Value filenameStr(isolate, filename);
                auto path = fs::path(*filenameStr);
                if (funcName.IsEmpty()) {
                    fprintf(stderr, "    at %s [%d:%d]\n", path.filename().c_str(), frame->GetLineNumber(), frame->GetColumn());
                } else {
                    v8::String::Utf8Value funcNameStr(isolate, funcName);
                    fprintf(stderr, "    at %s() [%s:%d:%d]\n", *funcNameStr, path.filename().c_str(), frame->GetLineNumber(), frame->GetColumn());
                }
            }
        }
        // code
        if (col == -1 || !module) {
            fprintf(stderr, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
            return;
        }

        fprintf(stderr, "━━━━━━━━━━━━━━━━ code ━━━━━━━━━━━━━━━━\n");

        v8::MaybeLocal<v8::String> maybeSource = msg->GetSource(ctx);
        if (maybeSource.IsEmpty()) return;

        v8::Local<v8::String> source = maybeSource.ToLocalChecked();
        v8::String::Utf8Value sourceStr(isolate, source);
        int startLine = line - 5;
        if (startLine < 0) startLine = 0;

        std::vector<std::string> lines;
        std::stringstream f(*sourceStr);
        std::string lineStr;
        while (std::getline(f, lineStr, '\n')) {
            lines.push_back(lineStr);
        }

        char buff[12];
        sprintf(buff, "%d", line);
        size_t width = strlen(buff);

        for (int i = startLine; i < line; i++) {
            fprintf(stderr, "%*d │%s\n", (int) width, i + 1, lines[i].c_str());
        }
        fprintf(stderr, "%*c │", (int) width, ' ');
        for (int i = 0; i < col; i++) {
            fprintf(stderr, " ");
        }
        for (int i = col; i < msg->GetEndColumn(); i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");

        fprintf(stderr, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

        // if it is AggregateError, iterate and call itself with the exception
        if (!strcmp("AggregateError", *nameStr)) {
            v8::Local<v8::Value> errors = obj->Get(ctx, v8::String::NewFromUtf8(isolate, "errors").ToLocalChecked()).ToLocalChecked();
            v8::Local<v8::Array> errorsArray = v8::Local<v8::Array>::Cast(errors);
            int errorArrLength = errorsArray->Length();
            for (int i = 0; i < errorArrLength; i++) {
                v8::Local<v8::Value> error = errorsArray->Get(ctx, i).ToLocalChecked();
                printException(ctx, error);
            }
        }
    }

    void throwAndPrintException(v8::Local<v8::Context> ctx, const char* message,
            ExceptionType type)
    {
        v8::Local<v8::Value> exc = throwException(ctx, message, type);
        printException(ctx, exc);
    }

}
