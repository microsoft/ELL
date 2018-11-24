////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ArchiveVersion.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace utilities
{
    /// <summary> Enum to record verion number history </summary>
    /// Any time a class updates its archive format, it should add a new entry for its version number
    /// before "nextVersion".
    enum class ArchiveVersionNumbers : int
    {
        v0_initial = 0,
        v1 = 1,
        v2 = 2,
        v3_model_metadata = 3,
        v4_source_sink_shapes = 4,
        v5_refined_nodes = 5,
        v6_sink_triggers = 6,
        v7_binary_operation_active_regions = 7,
        v8_port_memory_layout = 8,
        v9_activation_objects = 9, // move activation from template parameters to member objects.
        v10_memory_layout_update = 10,
        nextVersion
    };

    /// <summary> Type to represent archive versions </summary>
    struct ArchiveVersion
    {
        static constexpr int currentVersion = static_cast<int>(ArchiveVersionNumbers::nextVersion) - 1;

        constexpr ArchiveVersion(int version) :
            versionNumber(version) {}
        constexpr ArchiveVersion(ArchiveVersionNumbers version) :
            versionNumber(static_cast<int>(version)) {}

        /// <summary> The version number </summary>
        int versionNumber = 0;
    };

    // Comparison operators on version numbers
    bool operator==(const ArchiveVersion& a, const ArchiveVersion& b);
    bool operator!=(const ArchiveVersion& a, const ArchiveVersion& b);
    bool operator>(const ArchiveVersion& a, const ArchiveVersion& b);
    bool operator>=(const ArchiveVersion& a, const ArchiveVersion& b);
    bool operator<(const ArchiveVersion& a, const ArchiveVersion& b);
    bool operator<=(const ArchiveVersion& a, const ArchiveVersion& b);

} // namespace utilities
} // namespace ell
