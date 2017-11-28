## ELL archive versioning

Whenever an IArchivable object updates its archive representation, it should have a new version number. Instead of
just incrementing that object's version number, increment the `ArchiveVersion::currentVersion` constant, and use
that as the version number. This way, an archive's version number is the max of the version numbers of all its
included objects.

Whenever an archive version is incremented, make a note in the `ArchiveVersionHistory.md` file with the version
number and what changed, and whether or not the change results in compatibility issues.