#define NOB_IMPLEMENTATION
#include "src/nob.h"

static const char *raylib_modules[] = {
    "rcore",   "raudio", "rglfw",     "rmodels",
    "rshapes", "rtext",  "rtextures", "utils",
};

const char *compiler = "cc";

bool build_raylib(void) {
  bool result = true;
  Nob_Cmd cmd = {0};
  Nob_File_Paths object_files = {0};
  Nob_Procs procs = {0};

  if (!nob_mkdir_if_not_exists("build/raylib/"))
    nob_return_defer(false);

  for (size_t i = 0; i < NOB_ARRAY_LEN(raylib_modules); ++i) {
    const char *input_path =
        nob_temp_sprintf("raylib/src/%s.c", raylib_modules[i]);
    const char *output_path =
        nob_temp_sprintf("build/raylib/%s.o", raylib_modules[i]);

    nob_da_append(&object_files, output_path);

    if (nob_needs_rebuild(output_path, &input_path, 1)) {
      cmd.count = 0;
      nob_cmd_append(&cmd, "cc");
      nob_cmd_append(&cmd, "-ggdb", "-DPLATFORM_DESKTOP", "-fPIC");
      nob_cmd_append(&cmd, "-Iraylib/src/external/glfw/include");
      nob_cmd_append(&cmd, "-c", input_path);
      nob_cmd_append(&cmd, "-o", output_path);
      Nob_Proc proc = nob_cmd_run_async(cmd);
      nob_da_append(&procs, proc);
    }
  }
  cmd.count = 0;

  if (!nob_procs_wait(procs))
    nob_return_defer(false);

  const char *libraylib_path = nob_temp_sprintf("build/raylib/libraylib.a");

  if (nob_needs_rebuild(libraylib_path, object_files.items,
                        object_files.count)) {
    nob_cmd_append(&cmd, "ar", "-crs", libraylib_path);
    for (size_t i = 0; i < NOB_ARRAY_LEN(raylib_modules); ++i) {
      const char *input_path =
          nob_temp_sprintf("build/raylib/%s.o", raylib_modules[i]);
      nob_cmd_append(&cmd, input_path);
    }
    if (!nob_cmd_run_sync(cmd))
      nob_return_defer(false);
  }

defer:
  nob_cmd_free(cmd);
  nob_da_free(object_files);
  return result;
}

const char *input_paths[] = {"src/main.c", "src/window.c"};

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  const char *program = nob_shift_args(&argc, &argv);
  (void)program;

  nob_mkdir_if_not_exists("build");
  if (!build_raylib())
    return 1;
  if (nob_needs_rebuild("build/fake-os", input_paths,
                        NOB_ARRAY_LEN(input_paths))) {
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, compiler);
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");
    nob_cmd_append(&cmd, "-Iraylib/src/");
    nob_cmd_append(&cmd, "-o", "build/fake-os");
    nob_da_append_many(&cmd, input_paths, NOB_ARRAY_LEN(input_paths));
    nob_cmd_append(&cmd, "-Lbuild/raylib/");
    nob_cmd_append(&cmd, "-lraylib", "-lm");
    if (!nob_cmd_run_sync(cmd))
      return 1;
  } else {
    nob_log(NOB_INFO, "Executable is already up to date");
  }

  if (argc > 0) {
    const char *subcmd = nob_shift_args(&argc, &argv);

    if (strcmp(subcmd, "-r") == 0) {
      Nob_Cmd cmd = {0};
      nob_cmd_append(&cmd, "./build/fake-os");
      nob_da_append_many(&cmd, argv, argc);
      if (!nob_cmd_run_sync(cmd))
        return 1;
    } else if (strcmp(subcmd, "-d") == 0) {
      Nob_Cmd cmd = {0};
      nob_cmd_append(&cmd, "gdb");
      nob_cmd_append(&cmd, "./build/fake-os");
      nob_temp_reset();
      if (!nob_cmd_run_sync(cmd))
        return 1;
    } else {
      nob_log(NOB_ERROR, "Unknown subcommand %s", subcmd);
    }
  }

  return 0;
}
