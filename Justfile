@lint:
    find src/ \
    \( -name "*.cpp" -o -name "*.cc" -o -name \
    "*.c" -o -name "*.h" -o -name "*.hpp" -o \
    -name "*.cxx" \) -exec clang-format -i {} +
