bin := "./bin"
src := "./src/main.cpp"
out := bin + "/main.out"
cxx := "g++"
cxxflags := "--std=c++23"
libs := "-lboost_fiber -lboost_context"

@_ensure-bin:
    mkdir -p {{bin}}

@compile: _ensure-bin
    {{cxx}} {{src}} -O3 {{cxxflags}} -o {{out}} {{libs}}

@compile-tsan: _ensure-bin
    {{cxx}} {{src}} -O1 -g {{cxxflags}} -o {{out}} {{libs}} \
    -fsanitize=thread -fno-omit-frame-pointer

@compile-msan: _ensure-bin
    clang++ {{src}} -O1 -g {{cxxflags}} -o {{out}} {{libs}} \
    -fsanitize=memory -fno-omit-frame-pointer -fsanitize-memory-track-origins=2

@compile-asan: _ensure-bin
    {{cxx}} {{src}} -O1 -g {{cxxflags}} -o {{out}} {{libs}} \
    -fsanitize=address,undefined -fno-omit-frame-pointer

@compile-leak: _ensure-bin
    {{cxx}} {{src}} -O1 -g {{cxxflags}} -o {{out}} {{libs}} \
    -fsanitize=leak

@exec:
    {{out}}

@run:       compile exec
@trun: compile-tsan
    env TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" {{out}}

@mrun: compile-msan
    env MSAN_OPTIONS="halt_on_error=1" {{out}}

@arun: compile-asan
    env ASAN_OPTIONS="detect_leaks=1 halt_on_error=1" {{out}}
@lrun:      compile-leak  exec

@lint:
    find src/ \
    \( -name "*.cpp" -o -name "*.cc" -o -name \
    "*.c" -o -name "*.h" -o -name "*.hpp" -o \
    -name "*.cxx" \) -exec clang-format -i {} +

@clean:
    rm -rf {{bin}}