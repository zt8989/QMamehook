#ifndef QHOOKERQUICKJS_H
#define QHOOKERQUICKJS_H

#include <QString>
#include "quickjs.h"

class qhookerMain;

class QuickJsState
{
public:
    bool Load(const QString &path, qhookerMain *owner);
    void Clear();
    bool IsActive() const;
    void CallHandler(qhookerMain *owner, const QString &handlerName, const QString &value = QString());

private:
    JSRuntime *runtime = nullptr;
    JSContext *context = nullptr;
    JSValue exports = JS_UNDEFINED;

    void HandleResult(qhookerMain *owner, JSValue value);
    void PrintException();
};

#endif // QHOOKERQUICKJS_H
