# Application Template 1.0

## Dependencies

This project is intended to build with Microsoft Visual Studio 2019, using the build.sln solution file.  The following are also required for several of the projects.

    https://nodejs.org/
    https://developer.android.com/studio

## Notes

The std::filesystem is missing from the NDK.
  Write an alternative drop in replacement.
Data read from disk does not need to persist... it will be loaded almost directly into the GPU.
  Identify what data needs to remain on the heap and what should be streamed in/out.

  emplace, emplace_back and emplace_front
  map try_emplace


