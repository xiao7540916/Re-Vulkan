#pragma once

namespace Horizon
{

#define HORIZON_NULL_HANDLE 0

#define HORIZON_ARRAYSIZE(a) ((int)(sizeof(a) / sizeof(*(a))))

#define HORIZON_INVALID_ENUM_VALUE() assert(false)

#define HORIZON_ENUM_CLASS_OPERATORS(_enum_class_) inline _enum_class_ operator& (_enum_class_ a, _enum_class_ b) { return static_cast<_enum_class_>(static_cast<int>(a) & static_cast<int>(b)); } \
												   inline _enum_class_ operator| (_enum_class_ a, _enum_class_ b) { return static_cast<_enum_class_>(static_cast<int>(a) | static_cast<int>(b)); } \
												   inline _enum_class_& operator|= (_enum_class_& a, _enum_class_ b) { a = a | b; return a; } \
												   inline _enum_class_& operator&= (_enum_class_& a, _enum_class_ b) { a = a & b; return a; }

#define HORIZON_BIND_FUNCTION(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args) > (args)...); }

}