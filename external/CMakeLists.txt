include(FetchContent)

FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest
        GIT_TAG f25235f4c2f8a5fcf8e888114a90864ef5e4bf56 # v2.4.10
)

FetchContent_MakeAvailable(doctest)
