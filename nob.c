#define NOB_IMPLEMENTATION
#include "nob.h"

const char *compiler = "cc";
const char *input_paths[] = {"main.c"};
const char *output = "fake-os";

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  const char *program = nob_shift_args(&argc, &argv);
  (void)program;

  if (nob_needs_rebuild(output, input_paths, NOB_ARRAY_LEN(input_paths))) {
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, compiler);
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");
    nob_cmd_append(&cmd, "-o", output);
    nob_da_append_many(&cmd, input_paths, NOB_ARRAY_LEN(input_paths));
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
      nob_cmd_append(&cmd, nob_temp_sprintf("./%s", output));
      nob_temp_reset();
      nob_da_append_many(&cmd, argv, argc);
      if (!nob_cmd_run_sync(cmd))
        return 1;
    } else if (strcmp(subcmd, "-d") == 0) {
      Nob_Cmd cmd = {0};
      nob_cmd_append(&cmd, "gdb");
      nob_cmd_append(&cmd, nob_temp_sprintf("./%s", output));
      nob_temp_reset();
      if (!nob_cmd_run_sync(cmd))
        return 1;
    } else {
      nob_log(NOB_ERROR, "Unknown subcommand %s", subcmd);
    }
  }

  return 0;
}
