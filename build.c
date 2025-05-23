#define ZD_IMPLEMENTATION
#define ZD_BUILD
#include "zd.h"

#define TARGET  "rvemu"
#define SRC_DIR "./src"

void compile(void)
{
    struct zd_builder builder = {0};
    zd_build_init(&builder);

    struct zd_cmd cmd = {0};
    zd_cmd_init(&cmd);
    zd_cmd_append_arg(&cmd, "gcc");
    zd_cmd_append_arg(&cmd, "-O3");
    zd_cmd_append_arg(&cmd, "-Wall", "-Wextra");
    zd_cmd_append_arg(&cmd, "-I", SRC_DIR);
    zd_cmd_append_arg(&cmd, "-o", TARGET);

    struct zd_meta_dir md = {0};
    zd_fs_loadd(SRC_DIR, &md);

    for (size_t i = 0; i < md.f_cnt; i++) {
        if (!zd_wildcard_match(md.files[i], "*.c"))
            continue;

        struct zd_string cfile = {0};
        zd_string_append(&cfile, "%s/%s", SRC_DIR, md.files[i]);
        zd_cmd_append_arg(&cmd, cfile.base);
        zd_string_destroy(&cfile);
    }

    zd_build_append_cmd(&builder, &cmd);
    zd_build_run_sync(&builder);

    zd_fs_destroy_md(&md);
    zd_build_destroy(&builder);
}

void clean(void)
{
    zd_fs_remove(TARGET);
}

void test(const char *input)
{
    if (!zd_wildcard_match(input, "*.c"))
        zd_log(LOG_FATAL, "%s is not a c file", input);

    struct zd_builder builder = {0};
    zd_build_init(&builder);

    struct zd_string exe = {0};
    exe = zd_string_sub(input, 0, strlen(input) - 2);

    struct zd_cmd cmd = {0};
    zd_cmd_init(&cmd);

    zd_cmd_append_arg(&cmd, "riscv64-unknown-elf-gcc");
    zd_cmd_append_arg(&cmd, "-O3");
    zd_cmd_append_arg(&cmd, "-o");
    zd_cmd_append_arg(&cmd, exe.base);
    zd_cmd_append_arg(&cmd, input);
    zd_build_append_cmd(&builder, &cmd);

    cmd = (struct zd_cmd) {0};
    zd_cmd_init(&cmd);

    zd_cmd_append_arg(&cmd, "qemu-riscv64");
    zd_cmd_append_arg(&cmd, exe.base);
    zd_build_append_cmd(&builder, &cmd);

    zd_build_run_sync(&builder);

    zd_string_destroy(&exe);
    zd_build_destroy(&builder);
}

void define_rule(struct zd_cmdl *cmdl)
{
    zd_cmdl_define(cmdl, OPTT_NO_ARG, "help", "h", "Print help information");
    zd_cmdl_define(cmdl, OPTT_NO_ARG, "compile", "c", "Compile all files");
    zd_cmdl_define(cmdl, OPTT_NO_ARG, "clean", "cl", "Clean the generated files");
    zd_cmdl_define(cmdl, OPTT_SINGLE_ARG, "test", "t", "Test emulator with an input file");
}

int main(int argc, char **argv)
{
    struct zd_cmdl cmdl = {0};
    zd_cmdl_init(&cmdl, false);

    define_rule(&cmdl);

    zd_cmdl_build(&cmdl, argc, argv);

    bool is_help = zd_cmdl_isuse(&cmdl, "help");
    bool is_clean = zd_cmdl_isuse(&cmdl, "clean");
    bool is_compile = zd_cmdl_isuse(&cmdl, "compile");
    bool is_test = zd_cmdl_isuse(&cmdl, "test");

    if (is_help)
        zd_cmdl_usage(&cmdl); 
    else if (is_compile)
        compile();
    else if (is_clean)
        clean();
    else if (is_test) {
        struct zd_cmdlopt *opt = zd_cmdl_get_opt(&cmdl, "test");
        struct zd_string *arg = zd_dyna_get(&opt->vals, 0);
        test(arg->base);
    } else
        zd_log(LOG_INFO, "unknown option");

    zd_cmdl_destroy(&cmdl);

    return 0;
}
