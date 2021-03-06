#pragma once

#include "Horizon/Core/HorizonPlatform.h"
#include "Horizon/Core/HorizonDesfinitons.h"

#include "Horizon/Core/StdHeaders.h"

#include <memory>

namespace Horizon
{
	using int8 = signed char;
	using uint8 = unsigned char;
	using int16 = short;
	using uint16 = unsigned short;
	using int32 = int;
	using uint32 = unsigned int;
	using int64 = long long;
	using uint64 = unsigned long long;

	template<class T>
	using Vector = std::vector<T>;

	using String = std::string;
	using WString = std::wstring;

	/// Smart pointers
	template<class T>
	using SharedPtr = std::shared_ptr<T>;

	template<class T>
	using SharedConstPtr = std::shared_ptr<const T>;

	template<class T>
	using UniquePtr = std::unique_ptr<T>;
}
