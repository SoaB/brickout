#include <stddef.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define Target "BrickOut"
#define BUILD_FOLDER "build"
#define CFLAGS \
    "-Wall", "-Wextra", "-O3", "-Wunused-function", "-std=c2x", "-m64", "-static"
#define CINCLUDE "-IC:/Users/Couga/MingW_Dev_Lib/include"
#define LINKLIB "-LC:/Users/Couga/MingW_Dev_Lib/lib"
#define LINKFLAGS "-O3", "-s", "-m64", "-lraylibdll"

static const char* src_files[] = {
    "main",
    "player",
    "ball",
    "game",
    "animframe",
    "timer",
    "explod",
    "enemy",
    "rmath",
};
bool Build()
{
    bool result = true;
    Cmd cmd = { 0 };
    File_Paths object_files = { 0 };
    Nob_Procs procs = { 0 };

    if (!mkdir_if_not_exists(BUILD_FOLDER)) {
        return_defer(false);
    }
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); ++i) {
        const char* input_path = nob_temp_sprintf("src/%s.c", src_files[i]);
        const char* output_path = nob_temp_sprintf("./%s/%s.o", BUILD_FOLDER, src_files[i]);
        nob_da_append(&object_files, output_path);
        if (nob_needs_rebuild(output_path, &input_path, 1)) {
            cmd.count = 0;
            cmd_append(&cmd, "gcc", CFLAGS, CINCLUDE);
            cmd_append(&cmd, "-c", input_path);
            cmd_append(&cmd, "-o", output_path);
            Proc proc = nob_cmd_run_async(cmd);
            nob_da_append(&procs, proc);
        }
    }
    cmd.count = 0;
    if (!nob_procs_wait(procs))
        nob_return_defer(false);

    cmd_append(&cmd, "gcc");
    cmd_append(&cmd, "-o", Target);
    for (size_t i = 0; i < object_files.count; ++i) {
        cmd_append(&cmd, object_files.items[i]);
    }
    cmd_append(&cmd, LINKFLAGS, LINKLIB);
    if (!cmd_run_async_and_reset(&cmd)) {
        return_defer(false);
    }
defer:
    cmd_free(cmd);
    da_free(object_files);
    return result;
}
int main(int argc, char** argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Build();
    return 0;
}
