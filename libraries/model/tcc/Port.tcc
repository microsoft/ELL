////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Port.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    struct unknown_t
    {
    };

    template <>
    struct PortTypeToValueType<Port::PortType::none>
    {
        typedef unknown_t value_type;
    };

    template <>
    struct PortTypeToValueType<Port::PortType::smallReal>
    {
        typedef float value_type;
    };

    template <>
    struct PortTypeToValueType<Port::PortType::real>
    {
        typedef double value_type;
    };

    template <>
    struct PortTypeToValueType<Port::PortType::integer>
    {
        typedef int value_type;
    };

    template <>
    struct PortTypeToValueType<Port::PortType::bigInt>
    {
        typedef int64_t value_type;
    };

    template <>
    struct PortTypeToValueType<Port::PortType::boolean>
    {
        typedef bool value_type;
    };
}
}
