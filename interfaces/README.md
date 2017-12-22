# ELL Language Bindings

ELL interface classes are the public API used by higher-level languages to interact with ELL. [*SWIG*](http://swig.org/) is used to generate projections of these interface classes to other languages such as Python, JavaScript, C# and so on.

The following is a set of design guidelines for implementing ELL interfaces, so that:
- The API classes are convenient to use from other languages
- We maximize code re-use between core code and API, and minimize the gap between API classes and their underlying counterparts 
- We minimize friction between C++, SWIG and the target language

## Shared types
Often, POD types (plain old datatypes) and enums from the core need to be projected into the API. To do so, pull in the required header from core, but tell SWIG to ignore all classes except the one you want e.g.

In predictors.i

    %rename($ignore, %$isclass) ""; // Ignore all classes by default
    %rename("%s") PaddingScheme; // Expose PaddingScheme specifically
    %include <Layer.h>

    . . . // add more core %includes for the POD types you want to expose

    // Restore environment once you're done adding the core %includes
    %rename("%s") ""; // Unignore everything

## Class conventions
### Member variables
Prefer public class members to manually specifying accessor/getter/setter methods. This allows for easier intellisense and debugging in scripting environments.
- Use "const" to create immutable members


## POD types
Sometimes a POD type is inconvenient to use as-is from a scripting language, usually because it is necessary to create an empty instance of the type, then set its members individually. Create easy to use constructors and other overrides using the *%extend* directive in SWIG e.g.

In predictors_python_post.i

    // Additional C++ code to make API more natural for Python callers
    %extend LayerShape
    {  
        LayerShape(size_t rows, size_t columns, size_t channels) 
        {
            return new ell::api::predictors::neural::LayerShape{rows, columns, channels};
        }
    };


## Enum types
Use normal enums and enum classes. These work as expected in almost all languages. For Python, SWIG will generated flattended enum values as global constants. Instead, use %pythoncode to inject a Python class that uses the flattended values as member values, and removes them from global scope e.g.

If the C++ class was:

    enum class EnumTest
    {
        Default = 0,
        One = 1,
        Two = 2,
    };


Then put this in the *interface*_python_post.i

    %pythoncode %{
    class MyEnum:
        Default = MyEnum_Default
        One = MyEnum_One
        Two = MyEnum_Two
    %}
    del MyEnum_Default
    del MyEnum_One
    del MyEnum_Two

## Naming
Use SWIG's *%rename* declaration to change classes, methods, constants and so on to names that reflect the target language's convention e.g.

In C++:

    class Test
    {
    public:

        int GetIntPlusOne(int a)
        {
            return a + 1;
        }
    };

Then put this in the *interface*_python_pre.i

    %rename (get_int_plus_one) Test::GetIntPlusOne(int);


- For Python, follow [*PEP8 conventions*](https://www.python.org/dev/peps/pep-0008/)
- For C#, follow [*.NET Framework conventions*](https://docs.microsoft.com/en-us/dotnet/standard/design-guidelines/general-naming-conventions)
- For JavaScript/Node JS, follow [*Node Style Guide*](https://www.npmjs.com/package/node-style-guide)

## Supporting Intellisense
ELL uses the *%feature("autodoc", "3")* directive from SWIG to generate appropriate intellisense for basic things like class members, method parameters and so on.

You can add additional strings that show up in intellisense using the *docstring* directive e.g.

    %feature("docstring") Foo::bar "Bar method on Foo class"


For documenting generated code, you can add/prepend docstring comments e.g.

    %pythonprepend my_function(int, string) %{
        """My function to do something
            number:Int -- a number
            text:String -- a string
        """
    %}

## Return types
Generally, prefer to return value types or new objects, not references. The reason is that the API caller can manage the results, including object lifetime, themselves. If you return a reference like: std::vector<float>& foo() {…}, then when callers do something like this:

    result = foo()
    do_something(result)
    del result   # Uh, oh

In addition, references don't always show up well in the scripting debugger.
	
### Returning vectors or other multi-value objects
SWIG has decent support for std::vector and is perfectly safe to use as a return value.  It even allows viewing of the elements in the script debugger.

For more complicated types, create a ELL interface class and project that into the other languages. Often, these classes can be immutable after construction, so you just need to expose easy methods for initialization.

