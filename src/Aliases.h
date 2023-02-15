#pragma once

#include <QMainWindow>

#define DECLARE_QT_CLASS(classname) \
    QT_BEGIN_NAMESPACE \
    namespace Ui { class classname; } \
    QT_END_NAMESPACE

#include <vector>
template<typename T>
using Vector = std::vector<T>;