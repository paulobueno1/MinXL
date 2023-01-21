# MinXL - Extending Excel for Mac with C++

<br>

## Introduction

Excel for Windows has good support for interoperability with C++/C#; unfortunately the same cannot be said about Excel for Mac. MinXL aims to bridge that gap by allowing you to write functions in C++ and call them from VBA with a very intuitive API.

~~Although not header-only, MinXL is very simple to integrate. If you already have your project set up, just clone the repository into your dependencies folder and add it to your CMakeLists.txt file.~~ MinXL is now header-only! Just add the ```include``` folder to your include paths, then ```#include <MinXL/MinXL.hpp>``` wherever you want to use it.

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
// Alternatively, if you like suffering, you can import the function by its mangled name...
extern "C"
{
    // I strongly recommend receiving data using mxl::Variant&; a few extra bytes won't really matter
    // Primitive numeric types will work, but be aware of VBA's somewhat counter-intuitive naming
    // (e.g Longs have 4 bytes, Integers (and Booleans!!!) have 2 bytes, etc)
    mxl::Variant IncrementArrayBy(mxl::Variant& arg, double value)
    {
        try
        {
            // MinXL will automatically check if this conversion is valid
            mxl::Array<mxl::Variant> array = std::move(arg);

            // mxl::Variant has convenient operator overloads to make your life easier
            for (auto& i : array)
                i += value;

            // Implicitly converts back to mxl::Variant
            return std::move(array);
        }
        catch (mxl::Exception& e)
        {
            // If anything within the try block fails, MinXL will throw an exception with a
            // nice understandable message that can be returned as a string to Excel
            return mxl::String{e.what()};
        }
    }
}
```

<br>

### On VBA
```VB
Private Declare PtrSafe Function IncrementArrayBy _
Lib "/Library/Application Support/Microsoft/YourLibrary.dylib" _
(ByRef v as Variant, ByVal d as Double) As Variant      ' Always pass Variants ByRef!

Public Function CallLibFunction(r as Range) as Variant
    CallLibFunction = IncrementArrayBy(r.Value, 2.5)    ' Extract the Range's values first!!!
End Function
```

<br>

### On Excel
![](docs/img/usage.png?raw=true)

**_In this example, not a single copy was performed inside C++ :D_**


<br>

---

## Contributions, questions and feedback

Contributions are welcome! Fork it, hack it and open a PR :)

This is still an early version and some limitations will be addressed over time. 

If you have troubles and/or doubts while using MinXL, feel free to open an issue or message me directly at paulo.buenov@gmail.com.