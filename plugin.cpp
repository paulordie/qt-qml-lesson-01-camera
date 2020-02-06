#include"plugin.h"

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterType<Camera>(uri, 1, 0, "CVCamera");
}
