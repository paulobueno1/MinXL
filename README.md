# MinXL - Extending Excel for Mac with C++

<br>

## Introduction

Excel for Windows has good support for interoperability with C++/C#; unfortunately the same cannot be said about Excel for Mac. MinXL aims to bridge that gap by allowing you to write functions in C++ and call them from VBA with a very intuitive API.

~~Although not header-only, MinXL is very simple to integrate. If you already have your project set up, just clone the repository into your dependencies folder and add it to your CMakeLists.txt file.~~ MinXL is now header-only! Just add it to your include paths, then ```#include <MinXL/MinXL.hpp>``` wherever you want to use it.

<br>

**Requires C++20 (or later)**

<!-- Future link for tutorial -->

<br>

---

## Usage

### On C++
```cpp
#include <MinXL/MinXL.hpp>

// Extern C is not mandatory, but it makes importing functions on VBA much easier
extern "C"
{
    // We recommend receiving data using mxl::Variant&
    // Primitive numeric types are ok, but be aware that on the VBA side a 
    // Long has 4 bytes (int32_t) and an Integer has 2 bytes (int16_t).
    mxl::Variant IncrementArrayBy(mxl::Variant& arg, double value)
    {
        try
        {
            // MinXL will automatically check if this conversion is valid
            mxl::Array<mxl::Variant> array = std::move(arg);

            // mxl::Variant comes with very convenient operator overloads
            for (auto& i : array)
                i += value;

            // Implicitly converts back to mxl::Variant
            return std::move(array);
        }
        catch (mxl::Exception& e)
        {
            // If the conversion fails, you can return the error message to Excel
            return mxl::String{e.what()};
        }
    }
}
```

<br>

### On VBA
```VB
Private Declare PtrSafe Function IncrementArrayBy _
Lib "/Library/Application Support/Microsoft/libexample.dylib" _
(ByRef v as Variant, ByVal d as Double) As Variant      ' Always pass Variants ByRef

Public Function CallLibFunction(r as Range) as Variant
    CallLibFunction = IncrementArrayBy(r.Value, 2.5)    ' Extract the Range's values first!!!
End Function
```

<br>

### On Excel
![](docs/img/usage.png?raw=true)

**_And not a single copy was performed :D_**

<!-- [**See more examples**](docs/README.md) -->

<br>

---

## Contribution, Questions and Feedback

Contributions are welcome! Prefer pull requests and, please, stick to the code style.

This is still an early version and its limitations will be addressed over time. 

If you have any trouble or doubts using MinXL, feel free to open an issue or message me directly at paulo.buenov@gmail.com.