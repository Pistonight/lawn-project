# Guidelines for modding the framework

To ensure we can easily take framework upgrades in the future,
**all changes** must be made as mix-ins, following these rules:
- Any change to the files from the framework must be guarded by macros
- Only these types of changes to framework is allowed:
  - Patching: Minor changes to a function (such as adding a call or removing a block; major change is not allowed)
  - Replacing: Replace the implementation of a symbol with our own implementation
  - Mixin: Adding **one** member to a class in the header, also can add many `#include`s

Mixin-code (in `src-mixin`) are code owned by us and can be modified freely.

## Scopes
The framework includes:
 - Lawn
 - "LawnApp" (source code in ResoddedFramework not in any subdirectory)
 - Sexy.TodLib
 - SexyAppFramework

ImageLib and PakLib are considered the same level as other libraries and
**cannot be modified**. Any changes must be made upstream

## Macros

Use the following macros when modifying the framework:

### `PISTON_PATCH_UPSTREAM`
A temporary patch that can/should/makes sense to be fixed in the upstream framework

Example
```cpp
void SomeFunction(const char* aString)
{
#ifdef PISTON_PATCH_UPSTREAM // nullptr crash
    if (aString == nullptr)
    {
        return;
    }
#endif
    printf("Got: %s\n", aString);
    if (aString == nullptr)
    {
        return;
    }
    // ...
}
```

### `PISTON_PATCH`
A permanent patch/hook to the framework to patch-in functionality from our mod
Example
```cpp
void SomeFunction(const char* aString)
{
#ifdef PISTON_PATCH
    Piston::SomeFunctionHook(aString);
#endif
    // ...
}
```

### `PISTON_MIXIN`
For replacing implementation and adding members

Example - removing implementation of a function so we can implement it ourselves
```cpp
#ifndef PISTON_MIXIN
void SomeFunction(const char* aString)
{
    // ...
}
#endif
```

Example - adding a member
```cpp
#ifdef PISTON_MIXIN
#include <Piston/MyType.h>
#endif

class SomeClass
{
    // ...
#ifdef PISTON_MIXIN
public:
    Piston::MyType mMyField;
#endif
}
```
