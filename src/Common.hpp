#include <array>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <mach-o/dyld.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <termios.h>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "Exception.hpp"

namespace xl
{
    template<typename _Ty> 
    class Array;
    class String;
    class Variant;
}

#include "Array.hpp"
#include "String.hpp"
#include "Variant.hpp"