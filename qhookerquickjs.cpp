#include "qhookerquickjs.h"
#include "qhookermain.h"
#include <QFile>
#include <QStringList>

static qhookerMain *GetQmhInstance(JSContext *ctx)
{
    return static_cast<qhookerMain *>(JS_GetContextOpaque(ctx));
}

static JSValue JsQmhOpen(JSContext *ctx, JSValueConst, int argc, JSValueConst *argv)
{
    qhookerMain *qmh = GetQmhInstance(ctx);
    if(!qmh)
        return JS_ThrowInternalError(ctx, "qhookerMain instance not found");

    int32_t portNum = 0;
    if(argc < 1 || JS_ToInt32(ctx, &portNum, argv[0]))
        return JS_ThrowTypeError(ctx, "open(port): invalid port");

    qmh->OpenPort(portNum);
    return JS_UNDEFINED;
}

static JSValue JsQmhClose(JSContext *ctx, JSValueConst, int argc, JSValueConst *argv)
{
    qhookerMain *qmh = GetQmhInstance(ctx);
    if(!qmh)
        return JS_ThrowInternalError(ctx, "qhookerMain instance not found");

    int32_t portNum = 0;
    if(argc < 1 || JS_ToInt32(ctx, &portNum, argv[0]))
        return JS_ThrowTypeError(ctx, "close(port): invalid port");

    qmh->ClosePort(portNum);
    return JS_UNDEFINED;
}

static JSValue JsQmhWrite(JSContext *ctx, JSValueConst, int argc, JSValueConst *argv)
{
    qhookerMain *qmh = GetQmhInstance(ctx);
    if(!qmh)
        return JS_ThrowInternalError(ctx, "qhookerMain instance not found");

    int32_t portNum = 0;
    if(argc < 2 || JS_ToInt32(ctx, &portNum, argv[0]))
        return JS_ThrowTypeError(ctx, "write(port, data): invalid port");

    const char *data = JS_ToCString(ctx, argv[1]);
    if(!data)
        return JS_ThrowTypeError(ctx, "write(port, data): invalid data");

    qmh->WritePort(portNum, QByteArray(data));
    JS_FreeCString(ctx, data);
    return JS_UNDEFINED;
}

#ifdef Q_OS_WIN
static JSValue JsQmhXInputVibrate(JSContext *ctx, JSValueConst, int argc, JSValueConst *argv)
{
    qhookerMain *qmh = GetQmhInstance(ctx);
    if(!qmh)
        return JS_ThrowInternalError(ctx, "qhookerMain instance not found");

    int32_t userIndex = 0;
    int32_t leftMotor = 0;
    int32_t rightMotor = 0;
    if(argc < 3 ||
       JS_ToInt32(ctx, &userIndex, argv[0]) ||
       JS_ToInt32(ctx, &leftMotor, argv[1]) ||
       JS_ToInt32(ctx, &rightMotor, argv[2])) {
        return JS_ThrowTypeError(ctx, "xinputVibrate(userIndex, leftMotor, rightMotor): invalid args");
    }

    return JS_NewBool(ctx, qmh->SetXInputVibration(userIndex, leftMotor, rightMotor));
}
#endif

bool QuickJsState::Load(const QString &path, qhookerMain *owner)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("Error loading JS file at: %s\n", path.toLocal8Bit().constData());
        return false;
    }

    Clear();

    runtime = JS_NewRuntime();
    if(!runtime) {
        printf("QuickJS runtime creation failed.\n");
        return false;
    }

    context = JS_NewContext(runtime);
    if(!context) {
        printf("QuickJS context creation failed.\n");
        Clear();
        return false;
    }

    JS_SetContextOpaque(context, owner);
    JS_AddIntrinsicBaseObjects(context);
    JS_AddIntrinsicDate(context);
    JS_AddIntrinsicRegExp(context);
    JS_AddIntrinsicJSON(context);
    JS_AddIntrinsicBigInt(context);

    JSValue globalObj = JS_GetGlobalObject(context);
    JSValue qmhObj = JS_NewObject(context);
    JS_SetPropertyStr(context, qmhObj, "open", JS_NewCFunction(context, JsQmhOpen, "open", 1));
    JS_SetPropertyStr(context, qmhObj, "close", JS_NewCFunction(context, JsQmhClose, "close", 1));
    JS_SetPropertyStr(context, qmhObj, "write", JS_NewCFunction(context, JsQmhWrite, "write", 2));
#ifdef Q_OS_WIN
    JS_SetPropertyStr(context, qmhObj, "xinputVibrate", JS_NewCFunction(context, JsQmhXInputVibrate, "xinputVibrate", 3));
#endif
    JS_SetPropertyStr(context, globalObj, "qmh", qmhObj);

    JSValue exportsObj = JS_NewObject(context);
    JSValue moduleObj = JS_NewObject(context);
    JS_SetPropertyStr(context, moduleObj, "exports", JS_DupValue(context, exportsObj));
    JS_SetPropertyStr(context, globalObj, "module", moduleObj);
    JS_SetPropertyStr(context, globalObj, "exports", JS_DupValue(context, exportsObj));
    JS_FreeValue(context, globalObj);

    QByteArray source = file.readAll();
    QByteArray fileName = path.toLocal8Bit();
    JSValue evalResult = JS_Eval(context, source.constData(), source.size(), fileName.constData(), JS_EVAL_TYPE_GLOBAL);
    if(JS_IsException(evalResult)) {
        PrintException();
        JS_FreeValue(context, evalResult);
        Clear();
        return false;
    }
    JS_FreeValue(context, evalResult);

    JSValue globalObjAfter = JS_GetGlobalObject(context);
    JSValue moduleAfter = JS_GetPropertyStr(context, globalObjAfter, "module");
    JSValue exportsAfter = JS_GetPropertyStr(context, moduleAfter, "exports");
    exports = JS_DupValue(context, exportsAfter);
    JS_FreeValue(context, exportsAfter);
    JS_FreeValue(context, moduleAfter);
    JS_FreeValue(context, globalObjAfter);

    printf("Loading: %s\n", path.toLocal8Bit().constData());
    return true;
}

void QuickJsState::Clear()
{
    if(context) {
        if(!JS_IsUndefined(exports))
            JS_FreeValue(context, exports);
    }

    if(context) {
        JS_FreeContext(context);
        context = nullptr;
    }

    if(runtime) {
        JS_FreeRuntime(runtime);
        runtime = nullptr;
    }

    exports = JS_UNDEFINED;
}

bool QuickJsState::IsActive() const
{
    return context && !JS_IsUndefined(exports);
}

void QuickJsState::CallHandler(qhookerMain *owner, const QString &handlerName, const QString &value)
{
    if(!IsActive())
        return;

    QByteArray handlerKey = handlerName.toLocal8Bit();
    JSValue handler = JS_GetPropertyStr(context, exports, handlerKey.constData());
    if(!JS_IsFunction(context, handler)) {
        JS_FreeValue(context, handler);
        return;
    }

    JSValue contextObj = JS_NewObject(context);
    JS_SetPropertyStr(context, contextObj, "gameName", JS_NewString(context, owner->gameName.constData()));
    JS_SetPropertyStr(context, contextObj, "portCount", JS_NewInt32(context, owner->validIDs.size()));

    JSValue argv[2];
    int argc = 0;

    if(value.isEmpty()) {
        argv[0] = contextObj;
        argc = 1;
    } else {
        argv[0] = JS_NewInt32(context, value.toInt());
        argv[1] = contextObj;
        argc = 2;
    }

    JSValue result = JS_Call(context, handler, JS_UNDEFINED, argc, argv);
    JS_FreeValue(context, handler);
    JS_FreeValue(context, contextObj);
    if(argc > 1)
        JS_FreeValue(context, argv[0]);

    if(JS_IsException(result)) {
        PrintException();
        JS_FreeValue(context, result);
        return;
    }

    HandleResult(owner, result);
    JS_FreeValue(context, result);
}

void QuickJsState::HandleResult(qhookerMain *owner, JSValue value)
{
    if(JS_IsUndefined(value) || JS_IsNull(value))
        return;

    if(JS_IsString(value)) {
        const char *command = JS_ToCString(context, value);
        if(command) {
            QStringList commands = QString::fromLocal8Bit(command).split(',', Qt::SkipEmptyParts);
            for(const auto &entry : commands) {
                if(entry.contains("cmw")) {
                    int portNum = entry.at(entry.indexOf("cmw")+4).digitValue();
                    owner->WritePort(portNum, entry.mid(entry.indexOf("cmw")+6).toLocal8Bit());
                } else if(entry.contains("cmo")) {
                    int portNum = entry.at(entry.indexOf("cmo")+4).digitValue();
                    owner->OpenPort(portNum);
                } else if(entry.contains("cmc")) {
                    int portNum = entry.at(entry.indexOf("cmc")+4).digitValue();
                    owner->ClosePort(portNum);
                }
            }
            JS_FreeCString(context, command);
        }
        return;
    }

    if(JS_IsArray(value)) {
        int64_t length = 0;
        if(!JS_GetLength(context, value, &length)) {
            for(int64_t i = 0; i < length; ++i) {
                JSValue entry = JS_GetPropertyUint32(context, value, static_cast<uint32_t>(i));
                if(JS_IsString(entry)) {
                    const char *command = JS_ToCString(context, entry);
                    if(command) {
                        QStringList commands = QString::fromLocal8Bit(command).split(',', Qt::SkipEmptyParts);
                        for(const auto &cmd : commands) {
                            if(cmd.contains("cmw")) {
                                int portNum = cmd.at(cmd.indexOf("cmw")+4).digitValue();
                                owner->WritePort(portNum, cmd.mid(cmd.indexOf("cmw")+6).toLocal8Bit());
                            } else if(cmd.contains("cmo")) {
                                int portNum = cmd.at(cmd.indexOf("cmo")+4).digitValue();
                                owner->OpenPort(portNum);
                            } else if(cmd.contains("cmc")) {
                                int portNum = cmd.at(cmd.indexOf("cmc")+4).digitValue();
                                owner->ClosePort(portNum);
                            }
                        }
                        JS_FreeCString(context, command);
                    }
                }
                JS_FreeValue(context, entry);
            }
        }
    }
}

void QuickJsState::PrintException()
{
    JSValue exception = JS_GetException(context);
    const char *message = JS_ToCString(context, exception);
    if(message)
        printf("QuickJS error: %s\n", message);
    JS_FreeCString(context, message);
    JS_FreeValue(context, exception);
}
