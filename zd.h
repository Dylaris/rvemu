/*
 * Author: Dylaris
 * Copyright (c) 2025
 * License: MIT
 * Date: 2025-05-23
 *
 * All rights reserved
 */

/*
 * Usage: Define a few macros to enable the corresponding functions
 *
 *   ZD_STATIC            Restricts the function scope to static
 *   ZD_IMPLEMENTATION    Includes function implementation when defined, otherwise only the header is included
 *
 * '+' means you can use it in 'linux' and 'windows', otherwise only 'linux'
 *
 * + ZD_BUILD             Build the c project using only c 
 * + ZD_TEST              Simple testing tool
 * + ZD_PRINT             Some special print
 * + ZD_LOG               Simple logging for information
 * + ZD_FS                Some operations about file and directory
 * + ZD_WILDCARD          Wildcard ( '*', '?' )
 *   ZD_DYNASM            A simple way to use 'dynasm' (A stupid thing :->)
 *   ZD_COROUTINE         A simple coroutine implementation (based on GNU GCC)
 * + ZD_COMMAND_LINE      Some operations about command line (option, ...)
 * + ZD_DS_DYNAMIC_ARRAY  Dynamic array
 * + ZD_DS_DYNAMIC_BUFFER Dynamic buffer
 * + ZD_DS_STRING         String
 * + ZD_DS_STACK          Stack
 * + ZD_DS_HASH           Hash table (based on linked-list)
 * + ZD_DS_TRIE           Trie or prefix tree
 * + ZD_DS_QUEUE          Queue (based on linked-list)
 * + ZD_DS_LINKED_LIST    Linked list
 *
 *
 * !!! NOTE !!!
 *
 * Since `zd.h` implements a generic data structure, the elements you push into it should be pointers. 
 * For example, if you want to push an integer, you should pass an (int *). 
 *
 * Because strings are not of fixed length, they cannot be directly used as elements of these generic data structures. 
 * Therefore, we use (char **) as the element type of the data structure, pointing to the string. 
 * Since what we actually want to push into the data structure is (char **), we need to pass (char ***).
 * If you find this cumbersome, you can simply use the implemented struct zd_string.
 */

#ifndef _ZD_H_
#define _ZD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/* handle some dependencies between macros */
#ifdef ZD_COMMAND_LINE
  #ifndef ZD_DS_DYNAMIC_ARRAY
    #define ZD_DS_DYNAMIC_ARRAY
  #endif
  #ifndef ZD_DS_STRING
    #define ZD_DS_STRING
  #endif
  #ifndef ZD_LOG
    #define ZD_LOG
  #endif
  #ifndef ZD_WILDCARD
    #define ZD_WILDCARD
  #endif
#endif

#ifdef ZD_COROUTINE
  #ifndef ZD_DS_DYNAMIC_ARRAY
    #define ZD_DS_DYNAMIC_ARRAY
  #endif
  #ifndef ZD_DS_STACK
    #define ZD_DS_STACK
  #endif
#endif

#ifdef ZD_BUILD
  #ifndef ZD_LOG
    #define ZD_LOG
  #endif
  #ifndef ZD_FS
    #define ZD_FS
  #endif
  #ifndef ZD_WILDCARD
    #define ZD_WILDCARD
  #endif
  #ifndef ZD_COMMAND_LINE
    #define ZD_COMMAND_LINE
  #endif
  #ifndef ZD_DS_STRING
    #define ZD_DS_STRING
  #endif
  #ifndef ZD_DS_DYNAMIC_ARRAY
    #define ZD_DS_DYNAMIC_ARRAY
  #endif
#endif

#ifdef ZD_DS_QUEUE
  #ifndef ZD_DS_DYNAMIC_ARRAY
    #define ZD_DS_DYNAMIC_ARRAY
  #endif
#endif

#ifdef ZD_DS_STACK
  #ifndef ZD_DS_DYNAMIC_ARRAY
    #define ZD_DS_DYNAMIC_ARRAY
  #endif
#endif

#define NOT_SUPPORT(msg)                            \
    do {                                            \
        fprintf(stderr, "'%s' not support\n", msg); \
        exit(EXIT_FAILURE);                         \
    } while (0)

#ifndef ZD_DEF
  #ifdef ZD_STATIC
    #define ZD_DEF static
  #else
    #define ZD_DEF extern
  #endif /* ZD_STATIC */
#endif /* ZD_DEF */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef ZD_WILDCARD

ZD_DEF bool zd_wildcard_match(const char *str, const char *pattern);

#endif /* ZD_WILDCARD */


#ifdef ZD_LOG

#define LOG_INFO  1
#define LOG_ERROR 2
#define LOG_GOOD  3
#define LOG_TODO  4
#define LOG_FATAL 5

ZD_DEF void zd_log(int type, const char *fmt, ...);

#endif /* ZD_LOG */


#ifdef ZD_TEST

struct zd_testsuite {
    size_t pass_count;
    size_t fail_count;
    const char *name;
};

static struct zd_testsuite *__suite_ptr__;

#ifndef zd_type_cast
  #ifdef ZD_IMPLEMENTATION
    #define zd_type_cast(obj, type) (type (obj))  
  #else
    #define zd_type_cast(obj, type)
  #endif /* ZD_IMPLEMENTATION */
#endif /* zd_type_cast */

#ifndef zd_assert
  #ifdef ZD_IMPLEMENTATION
    #define zd_assert(exp, msg)                                     \
        do {                                                        \
            if ((exp)) zd_pass(msg);                                \
            else       zd_fail(msg);                                \
        } while (0)
  #else
    #define zd_assert(exp, msg)
  #endif /* ZD_IMPLEMENTATION */
#endif /* zd_assert */

ZD_DEF void zd_pass(const char *msg);
ZD_DEF void zd_fail(const char *msg);
ZD_DEF void zd_run_test(struct zd_testsuite *suite, char *(*test)(void));
ZD_DEF void zd_test_summary(struct zd_testsuite *suite);

#endif /* ZD_TEST */

#ifdef ZD_DS_DYNAMIC_BUFFER

struct zd_dynb {
    char *base;
    size_t capacity;
};

ZD_DEF void zd_dynb_resize(struct zd_dynb *db, int size);
ZD_DEF void zd_dynb_destroy(void *arg);

#endif /* ZD_DS_DYNAMIC_BUFFER */

#ifdef ZD_DS_DYNAMIC_ARRAY

struct zd_dyna {
    void *base;
    size_t count;
    size_t capacity;
    void (*clear_item)(void *);
    size_t size;    /* size of each element */
    size_t pos;     /* iterator position */
};

ZD_DEF void zd_dyna_init(struct zd_dyna *da, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_dyna_append(struct zd_dyna *da, void *elem);
ZD_DEF void zd_dyna_insert(struct zd_dyna *da, size_t idx, void *elem);
ZD_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx);
ZD_DEF void zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem);
ZD_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx);
ZD_DEF void zd_dyna_destroy(struct zd_dyna *da);
ZD_DEF void *zd_dyna_next(struct zd_dyna *da);

#endif /* ZD_DS_DYNAMIC_ARRAY */

#ifdef ZD_DS_STRING

struct zd_string {
    char *base;
    size_t length;
    size_t capacity;
};

ZD_DEF void zd_string_append(struct zd_string *str, const char *fmt, ...);
ZD_DEF struct zd_string zd_string_sub(const char *str, size_t src, size_t dest);
ZD_DEF struct zd_string zd_string_rep(const char *str, size_t times);
ZD_DEF void zd_string_destroy(void *arg);

#endif /* ZD_DS_STRING */

#ifdef ZD_DS_STACK

struct zd_stack {
    void *base;
    int top;
    size_t capacity;
    void (*clear_item)(void *);
    size_t size;    /* size of each element */
    struct zd_dyna gc;
};

ZD_DEF void zd_stack_init(struct zd_stack *stk, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_stack_push(struct zd_stack *stk, void *elem);
ZD_DEF void *zd_stack_pop(struct zd_stack *stk);
ZD_DEF void *zd_stack_top(struct zd_stack *stk);
ZD_DEF void zd_stack_destroy(struct zd_stack *stk);

#endif /* ZD_DS_STACK */

#ifdef ZD_DS_TRIE

#ifndef ZD_TRIE_SIZE
  #define ZD_TRIE_SIZE   26
#endif /* ZD_TRIE_SIZE */

struct zd_trie_node {
    struct zd_trie_node *children[ZD_TRIE_SIZE];
    bool is_end;
    size_t count;
};

ZD_DEF struct zd_trie_node *zd_trie_create_node(void);
ZD_DEF void zd_trie_insert(struct zd_trie_node *root, const char *word);
ZD_DEF int zd_trie_search(struct zd_trie_node *root, const char *word);
ZD_DEF void zd_trie_destroy(struct zd_trie_node *root);

#endif /* ZD_DS_TRIE */

#ifdef ZD_DS_QUEUE

struct zd_queue_node {
    void *data;
    struct zd_queue_node *next;
};

struct zd_queue {
    struct zd_queue_node head;  /* dummy */
    struct zd_queue_node *front, *rear;
    size_t count;
    size_t size;    /* size of each element */
    struct zd_dyna gc;
    void (*clear_item)(void *);
};

#ifndef zd_queue_isempty
  #ifdef ZD_IMPLEMENTATION
    #define zd_queue_isempty(qp) ((qp)->head.next == NULL)
  #else
    #define zd_queue_isempty(qp)
  #endif /* ZD_IMPLEMENTATION */
#endif /* zd_queue_isempty */

ZD_DEF void zd_queue_init(struct zd_queue *qp, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_queue_destroy(struct zd_queue *qp);
ZD_DEF void *zd_queue_front(struct zd_queue *qp);
ZD_DEF void *zd_queue_rear(struct zd_queue *qp);
ZD_DEF void zd_queue_push(struct zd_queue *qp, void *elem);
ZD_DEF void *zd_queue_pop(struct zd_queue *qp);

#endif /* ZD_DS_QUEUE */

#ifdef ZD_DS_HASH

#define LOAD_TH_UPPER 0.75
#define LOAD_TH_LOWER 0.25

#define HASH_EXPAND 1
#define HASH_SHRINK 0

struct zd_hash_node {
    void *key;
    void *val;
    void *next;
};

struct zd_hash_tbl {
    struct zd_hash_node **buckets;  /* dymmy head of bucket list */
    float load;
    size_t count;
    size_t capacity;
    size_t key_size;
    size_t val_size;
    bool (*key_cmp)(void *, void *);
    size_t (*hash_func)(void *);
    void (*key_free)(void *); 
    void (*val_free)(void *); 
};

ZD_DEF void zd_htbl_init(struct zd_hash_tbl *htbl, size_t ksz, size_t vsz,
        bool (*key_cmp)(void *k1, void *k2), size_t (*hash_func)(void *),
        void (*key_free)(void *), void (*val_free)(void *));
ZD_DEF void zd_htbl_destroy(struct zd_hash_tbl *htbl);
ZD_DEF void zd_htbl_insert(struct zd_hash_tbl *htbl, void *key, void *val);
ZD_DEF void zd_htbl_remove(struct zd_hash_tbl *htbl, void *key);
ZD_DEF bool zd_htbl_search(struct zd_hash_tbl *htbl, void *key);
ZD_DEF void *zd_htbl_get(struct zd_hash_tbl *htbl, void *key);
ZD_DEF void zd_htbl_set(struct zd_hash_tbl *htbl, void *key, void *val);
ZD_DEF void zd_htbl_resize(struct zd_hash_tbl *htbl, int mode);

static inline size_t DJB_hash(const char *key, size_t key_len);
static inline size_t SDBM_hash(const char *key, size_t key_len);
static inline size_t int_hash(int key);
static inline size_t float_hash(float key);

#define STRING_HASH(key) DJB_hash((key), strlen(key))
#define INT_HASH(key) int_hash(key)

#endif /* ZD_DS_HASH */

#ifdef ZD_DS_LINKED_LIST

struct zd_list_node {
    void *data;
    struct zd_list_node *next;
};

struct zd_list {
    struct zd_list_node head;   /* dummy */
    size_t count;   /* element count */
    size_t size;    /* the size of each element */
    void (*clear_item)(void *);
};

ZD_DEF void zd_list_init(struct zd_list *list, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_list_destroy(struct zd_list *list);
ZD_DEF void zd_list_append(struct zd_list *list, void *elem);
ZD_DEF void zd_list_insert(struct zd_list *list, size_t idx, void *elem);
ZD_DEF void zd_list_remove(struct zd_list *list, size_t idx);
ZD_DEF void zd_list_reverse(struct zd_list *list);
ZD_DEF void *zd_list_get(struct zd_list *list, size_t idx);
ZD_DEF void zd_list_set(struct zd_list *list, size_t idx, void *elem);

#endif /* ZD_DS_LINKED_LIST */

#ifdef ZD_FS

#if defined(_WIN32)
  #include <windows.h>
  #include <direct.h>
  #include <fcntl.h>
  #include <errno.h>
#elif defined(__linux__)
  #include <unistd.h>
  #include <dirent.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <time.h>
  #include <sys/stat.h>
  #include <sys/types.h>
#endif /* platform */

#define FT_NOET 0    /* not exist */
#define FT_REG  1    /* regular file */
#define FT_DIR  2    /* directory */

#define MAX_PATH_SIZE 1024

struct zd_meta_file {
    char *name;
    char *content;
    int type;
    size_t size;
    size_t line;
};

struct zd_meta_dir {
    char *name;
    char **files;
    char **dirs;
    size_t f_cnt;
    size_t d_cnt;
    size_t count;
};


ZD_DEF unsigned long long zd_fs_get_timestamp(const char *pathname);
ZD_DEF char *zd_fs_getname(const char *pathname);
ZD_DEF bool zd_fs_pwd(char *buf, size_t buf_size);
ZD_DEF bool zd_fs_cd(const char *pathname);
ZD_DEF bool zd_fs_move(const char *src, const char *dest, bool is_binary);
ZD_DEF bool zd_fs_copy(const char *src, const char *dest, bool is_binary);
ZD_DEF bool zd_fs_mkdir(const char *pathname);
ZD_DEF bool zd_fs_touch(const char *pathname);
ZD_DEF bool zd_fs_remove(const char *pathname);
ZD_DEF int zd_fs_typeof(const char *pathname);
ZD_DEF bool zd_fs_loadf(const char *filename,
        struct zd_meta_file *res, bool is_binary);
ZD_DEF bool zd_fs_loadd(const char *dirname, struct zd_meta_dir *res);
ZD_DEF bool zd_fs_dumpf(const char *dest_file,
        const char *src_buf, size_t size, bool is_binary);
ZD_DEF void zd_fs_destroy_mf(struct zd_meta_file *mf);
ZD_DEF void zd_fs_destroy_md(struct zd_meta_dir *md);

#endif /* ZD_FS */

#ifdef ZD_COMMAND_LINE

/* option type */
#define OPTT_NO_ARG     0
#define OPTT_SINGLE_ARG 1
#define OPTT_MULTI_ARG  2

struct zd_cmdlopt {
    int type;
    bool is_defined;        /* is it a rule */
    struct zd_string name;  /* used in command line */
    struct zd_dyna vals;    /* target values, each element is 'struct zd_string' */
    struct zd_dyna pargs;   /* positional args, each element is 'struct zd_string' */

    /* for rule */
    struct zd_string lname; /* long name (use --) */
    struct zd_string sname; /* short name (use -) */
    struct zd_string description;
};

struct zd_cmdl {
    struct zd_string program;
    struct zd_dyna pargs;   /* positional args, each element is 'struct zd_string' */
    struct zd_dyna opts;    /* each element is 'struct zd_cmdlopt' */
    struct zd_dyna rules;   /* each element is 'struct zd_cmdlopt' */

    /* config */
    bool merge_opt;     /* merge options if they are the same */
};

ZD_DEF void zd_cmdl_init(struct zd_cmdl *cmdl, bool merge_opt);
ZD_DEF void zd_cmdl_define(struct zd_cmdl *cmdl, int type,
        const char *lname, const char *sname, const char *description);
ZD_DEF void zd_cmdl_build(struct zd_cmdl *cmdl, int argc, char **argv);
ZD_DEF void zd_cmdl_usage(struct zd_cmdl *cmdl);
ZD_DEF bool zd_cmdl_isuse(struct zd_cmdl *cmdl, const char *optname);
ZD_DEF struct zd_cmdlopt *zd_cmdl_get_opt(struct zd_cmdl *cmdl, 
        const char *optname);
ZD_DEF void zd_cmdl_destroy(void *arg);
ZD_DEF void zd_cmdl_dump(struct zd_cmdl *cmdl);
ZD_DEF void zd_cmdlopt_init(struct zd_cmdlopt *opt);
ZD_DEF void zd_cmdlopt_dump(struct zd_cmdlopt *opt, size_t level);
ZD_DEF void zd_cmdlopt_destroy(void *arg);

#endif /* ZD_COMMAND_LINE */

#ifdef ZD_BUILD

#if defined(__linux__)
  #include <unistd.h>
  #include <sys/wait.h>
  #include <threads.h>
#elif defined(_WIN32)
#else
#endif /* platform */

#define EXEC_UNDO   0
#define EXEC_OK     1
#define EXEC_ERROR  2

struct zd_cmd {
    struct zd_dyna args;    /* each element is struct zd_string */
    size_t count;
};

struct zd_builder {
    struct zd_dyna cmds;    /* each element is struct zd_cmd */
    size_t count;
    char *bd_src;
    char *bd_exe;
#if defined(__linux__)
    int *cmds_status;
    size_t cmds_exec_count;
    mtx_t cmds_exec_count_mutex;
#endif
};

static void _cmd_append_arg(struct zd_cmd *cmd, ...);
ZD_DEF void zd_cmd_init(struct zd_cmd *cmd);
ZD_DEF void zd_cmd_print(struct zd_cmd *cmd);
ZD_DEF int zd_cmd_run(struct zd_cmd *cmd);
ZD_DEF void zd_cmd_destroy(struct zd_cmd *cmd);

#ifndef zd_cmd_append_arg
  #ifdef ZD_IMPLEMENTATION
    #define zd_cmd_append_arg(cmd, ...) \
        _cmd_append_arg((cmd), __VA_ARGS__, NULL)
  #else
    #define zd_cmd_append_arg(cmd, ...)
  #endif /* ZD_IMPLEMENTATION */
#endif /* zd_cmd_append_arg */

#define BUILD_SRC "build.c"
#define BUILD_EXE "build"

static void _build_append_cmd(struct zd_builder *builder, ...);
ZD_DEF void zd_build_init(struct zd_builder *builder);
ZD_DEF void zd_build_destroy(struct zd_builder *builder);
ZD_DEF void zd_build_print(struct zd_builder *builder);
ZD_DEF int zd_build_run_sync(struct zd_builder *builder);
ZD_DEF int zd_build_run_async(struct zd_builder *builder);

#ifndef zd_build_append_cmd
  #ifdef ZD_IMPLEMENTATION
    #define zd_build_append_cmd(builder, ...) \
        _build_append_cmd((builder), ##__VA_ARGS__, NULL)
  #else
    #define zd_build_append_cmd(builder, ...)
  #endif /* ZD_IMPLEMENTATION */
#endif /* zd_build_append_cmd */

#endif /* ZD_BUILD */

#ifdef ZD_DYNASM

#if defined(__linux__)
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/wait.h>
  #include <sys/mman.h>
#elif defined(_WIN32)
#else
#endif

#define ZD_PAGE_SIZE 4096
#define ZD_ASSEMBLER "fasm"

ZD_DEF void *zd_dynasm_map(size_t size);
ZD_DEF void *zd_dynasm_do(char *code, void *addr);
ZD_DEF void zd_dynasm_free(void *addr);

#endif /* ZD_DYNASM */

#ifdef ZD_PRINT

#include <math.h>

#define OPT_COLOR 1
#define OPT_S_TBL 2
#define OPT_D_TBL 3

ZD_DEF void zd_print(int opt, ...);

#endif /* ZD_PRINT */

#ifdef ZD_COROUTINE

#if defined(__linux__) && defined(__GNUC__)

#define COROUTINE_STACK_SIZE (16*1024)
#define PROTECTION_REGION    64

// 64bit (context.regs[14])
// low
//      | regs[0]:  r15 |
//      | regs[1]:  r14 |
//      | regs[2]:  r13 |
//      | regs[3]:  r12 |
//      | regs[4]:  r9  |
//      | regs[5]:  r8  |
//      | regs[6]:  rbp |
//      | regs[7]:  rdi |
//      | regs[8]:  rsi |
//      | regs[9]:  ret |   // return address
//      | regs[10]: rdx |
//      | regs[11]: rcx |
//      | regs[12]: rbx |
//      | regs[13]: rsp |
// high

enum ctx_reg_idx {
    CTX_R15, CTX_R14, CTX_R13, CTX_R12, CTX_R9,  CTX_R8,  CTX_RBP,
    CTX_RDI, CTX_RSI, CTX_RET, CTX_RDX, CTX_RCX, CTX_RBX, CTX_RSP
};

#define ST_READY   0
#define ST_RUNNING 1
#define ST_SUSPEND 2
#define ST_DEAD    3

struct zd_coctx {
    void *regs[14];
    size_t stack_size;
    void *stack_base;
    int status;
    int id;
};

struct zd_colib {
    struct zd_dyna coctxs;
    struct zd_stack back_stk;   /* record the back address to resume() invoke */
    struct zd_coctx *cur_coctx;
};

static void __attribute__((naked)) zd_coctx_swap(struct zd_coctx *cur,
        struct zd_coctx *next);
ZD_DEF struct zd_colib *zd_colib_init(void);
ZD_DEF void zd_colib_destroy(struct zd_colib *colib);
ZD_DEF struct zd_coctx *zd_coctx_create(struct zd_colib *colib,
        void (*task)(void *), void *arg);
ZD_DEF void zd_coctx_resume(struct zd_colib *colib, struct zd_coctx *next);
ZD_DEF void zd_coctx_yield(struct zd_colib *colib);
ZD_DEF void zd_coctx_finish(struct zd_colib *colib);
ZD_DEF void zd_coctx_collect(struct zd_colib *colib, struct zd_coctx *co);
ZD_DEF size_t zd_coctx_alive(struct zd_colib *colib);
ZD_DEF int zd_coctx_workid(struct zd_colib *colib);

#endif /* platform */

#endif /* ZD_COROUTINE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ZD_H_ */

#ifdef ZD_IMPLEMENTATION

#ifdef ZD_COMMAND_LINE

ZD_DEF void zd_cmdl_dump(struct zd_cmdl *cmdl)
{
    fprintf(stderr, "<program>: %s\n", cmdl->program.base);

    if (cmdl->pargs.count > 0) {
        fprintf(stderr, "<pargs>:\n");
        for (size_t i = 0; i < cmdl->pargs.count; i++) {
            struct zd_string *arg = zd_dyna_get(&cmdl->pargs, i);
            fprintf(stderr, "\tpargs[%zu]: %s\n", i, arg->base);
        }
    } else {
        fprintf(stderr, "<pargs>: EMPTY\n");
    }

    if (cmdl->opts.count > 0) {
        fprintf(stderr, "<opts>:\n");
        for (size_t i = 0; i < cmdl->opts.count; i++) {
            fprintf(stderr, "\topts[%zu]:\n", i);
            struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->opts, i);
            zd_cmdlopt_dump(opt, 2);
        }
    } else {
        fprintf(stderr, "<opts>: EMPTY\n");
    }
}

static inline void _optt_to_string(int type, char *buf, size_t n)
{
    switch (type) {
    case OPTT_NO_ARG:
        snprintf(buf, n, "NO ARG");
        break;
    case OPTT_SINGLE_ARG:
        snprintf(buf, n, "SINGLE ARG");
        break;
    case OPTT_MULTI_ARG:
        snprintf(buf, n, "MULTI ARG");
        break;
    default:
        snprintf(buf, n, "UNKNOWN");
        break;
    }
}

ZD_DEF void zd_cmdlopt_dump(struct zd_cmdlopt *opt, size_t level)
{
    struct zd_string indent = zd_string_rep("\t", level);

    char type_str[20];
    _optt_to_string(opt->type, type_str, sizeof(type_str));

    fprintf(stderr, "%s<name>: %s\n", indent.base, opt->name.base);
    fprintf(stderr, "%s<type>: %s\n", indent.base, type_str);
    fprintf(stderr, "%s<rule>: %s\n", indent.base, opt->is_defined ? "true" : "false");

    if (opt->vals.count > 0) {
        fprintf(stderr, "%s<vals>:\n", indent.base);
        for (size_t i = 0; i < opt->vals.count; i++) {
            struct zd_string *val = zd_dyna_get(&opt->vals, i);
            fprintf(stderr, "%s\tvals[%zu]: %s\n", indent.base, i, val->base);
        }
    } else {
        fprintf(stderr, "%s<vals>: EMPTY\n", indent.base);
    }

    if (opt->pargs.count > 0) {
        fprintf(stderr, "%s<pargs>:\n", indent.base);
        for (size_t i = 0; i < opt->pargs.count; i++) {
            struct zd_string *arg = zd_dyna_get(&opt->pargs, i);
            fprintf(stderr, "%s\tpargs[%zu]: %s\n", indent.base, i, arg->base);
        }
    } else {
        fprintf(stderr, "%s<pargs>: EMPTY\n", indent.base);
    }

    zd_string_destroy(&indent);
}

ZD_DEF void zd_cmdlopt_init(struct zd_cmdlopt *opt)
{
    opt->type = OPTT_NO_ARG;
    opt->is_defined = false;
    opt->name = (struct zd_string) {0};
    opt->lname = (struct zd_string) {0};
    opt->sname = (struct zd_string) {0};
    zd_dyna_init(&opt->vals, sizeof(struct zd_string), zd_string_destroy);
    zd_dyna_init(&opt->pargs, sizeof(struct zd_string), zd_string_destroy);
    opt->description = (struct zd_string) {0};
}

ZD_DEF void zd_cmdl_init(struct zd_cmdl *cmdl, bool merge_opt)
{
    cmdl->program = (struct zd_string) {0};
    zd_dyna_init(&cmdl->pargs, sizeof(struct zd_string), zd_string_destroy);
    zd_dyna_init(&cmdl->opts, sizeof(struct zd_cmdlopt), zd_cmdlopt_destroy);
    zd_dyna_init(&cmdl->rules, sizeof(struct zd_cmdlopt), zd_cmdlopt_destroy);
    cmdl->merge_opt = merge_opt;
}

ZD_DEF void zd_cmdl_define(struct zd_cmdl *cmdl, int type,
        const char *lname, const char *sname, const char *description)
{
    if ((type < 0 || type > 2) || (!lname && !sname))
        return;

    struct zd_cmdlopt opt = {0};
    zd_cmdlopt_init(&opt);

    opt.type = type;
    zd_string_append(&opt.lname, lname ? lname : "@@");
    zd_string_append(&opt.sname, sname ? sname : "@@");
    zd_string_append(&opt.description, description ? description : "@@");

    zd_dyna_append(&cmdl->rules, &opt);
}

static inline char *_skip_dash(char *arg)
{
    for (int i = 0; i < (int) strlen(arg); i++) {
        if (arg[i] == '-')
            continue;
        return arg + i;
    }

    return NULL;
}

static inline void _check_opt(const char *optname,
        bool one_dash, bool is_short)
{
    if (is_short != one_dash) {
        if (one_dash) {
            zd_log(LOG_FATAL, "'-%s' is invalid", optname);
        } else {
            zd_log(LOG_FATAL, "'--%s' is invalid", optname);
        }
    }
}

static inline struct zd_cmdlopt *_get_rule(struct zd_cmdl *cmdl,
        const char *optname, bool *is_short)
{
    struct zd_string s;
    struct zd_cmdlopt *best_match = NULL;
    size_t max_match_len = 0;

    for (size_t i = 0; i < cmdl->rules.count; i++) {
        struct zd_cmdlopt *rule = zd_dyna_get(&cmdl->rules, i);

        s = (struct zd_string) {0};
        zd_string_append(&s, "%s*", rule->lname.base);
        if (zd_wildcard_match(optname, s.base)) {
            size_t match_len = strlen(rule->lname.base);
            if (match_len > max_match_len) {
                max_match_len = match_len;
                best_match = rule;
                if (is_short) *is_short = false;
            }
        }
        zd_string_destroy(&s);

        s = (struct zd_string) {0};
        zd_string_append(&s, "%s*", rule->sname.base);
        if (zd_wildcard_match(optname, s.base)) {
            size_t match_len = strlen(rule->sname.base);
            if (match_len > max_match_len) {
                max_match_len = match_len;
                best_match = rule;
                if (is_short) *is_short = true;
            }
        }
        zd_string_destroy(&s);
    }

    return best_match;
}

ZD_DEF void zd_cmdl_build(struct zd_cmdl *cmdl, int argc, char **argv)
{
    int pos = 0;     /* track the arg */

    zd_string_append(&cmdl->program, argv[pos++]);

    /* handle the positional args for program without options */
    while (pos < argc) {
        if (argv[pos][0] == '-')
            break;
        struct zd_string arg = {0};
        zd_string_append(&arg, argv[pos]);
        zd_dyna_append(&cmdl->pargs, &arg);
        pos++;
    }

    /* handle the remaining options */
    while (pos < argc) {
        bool one_dash = false;
        if (argv[pos][0] == '-' && argv[pos][1] == '-')
            one_dash = false;
        else
            one_dash = true;

        const char *p1 = _skip_dash(argv[pos]);
        if (!p1) {
            pos++;
            continue;
        }
        const char *p2 = strchr(p1, '=');
        bool has_equal = (p2 != NULL);


        struct zd_cmdlopt opt = {0};
        zd_cmdlopt_init(&opt);

        bool is_short = false;
        struct zd_cmdlopt *rule = _get_rule(cmdl, p1, &is_short);
        if (rule)
            _check_opt(p1, one_dash, is_short);

        /* add target values for this option according to its type */
        if (has_equal) {
            if (rule) {
                opt.is_defined = true;
                opt.type = rule->type;
                if (!is_short)
                    zd_string_append(&opt.name, rule->lname.base);
                else
                    zd_string_append(&opt.name, rule->sname.base);
            } else {
                opt.name = zd_string_sub(p1, 0, p2 - p1);
                opt.is_defined = false;
                opt.type = OPTT_SINGLE_ARG;
            }

            struct zd_string val = {0};
            if (*(p2 + 1)) {
                zd_string_append(&val, p2 + 1);
                zd_dyna_append(&opt.vals, &val);
            }

            pos++;
        } else { 
            if (rule) {
                opt.is_defined = true;
                opt.type = rule->type;
                if (!is_short)
                    zd_string_append(&opt.name, rule->lname.base);
                else
                    zd_string_append(&opt.name, rule->sname.base);

                p2 = p1 + opt.name.length;

                struct zd_string val = {0};
                if (*p2) {
                    zd_string_append(&val, p2);
                    zd_dyna_append(&opt.vals, &val);
                }
            } else {
                zd_string_append(&opt.name, p1);
                opt.is_defined = false;
                opt.type = OPTT_MULTI_ARG;
            }

            pos++;

            struct zd_string val;
            switch (opt.type) {
            case OPTT_NO_ARG:
                break;

            case OPTT_SINGLE_ARG:
                if (opt.vals.count == 1)
                    break;
                if (pos >= argc)
                    zd_log(LOG_FATAL, "option '%s' should receive one argument",
                            opt.name.base);
                if (argv[pos][0] == '-')
                    zd_log(LOG_FATAL, "'%s' should be an argument of option '%s'",
                            argv[pos], opt.name.base);
                val = (struct zd_string) {0};
                zd_string_append(&val, argv[pos]);
                zd_dyna_append(&opt.vals, &val);
                pos++;
                break;

            case OPTT_MULTI_ARG:
                while (pos < argc) {
                    if (argv[pos][0] == '-')
                        break;
                    val = (struct zd_string) {0};
                    zd_string_append(&val, argv[pos]);
                    zd_dyna_append(&opt.vals, &val);
                    pos++;
                }
                break;

            default:
                zd_log(LOG_FATAL, "invalid option type");
                break;
            }
        }

        /* add positional args for this option */
        while (pos < argc) {
            if (argv[pos][0] == '-')
                break;
            struct zd_string arg = {0};
            zd_string_append(&arg, argv[pos]);
            zd_dyna_append(&opt.pargs, &arg);
            pos++;
        }

        if (cmdl->merge_opt) {
            /* append all stuff if the option had been used instead of adding */
            bool is_use = zd_cmdl_isuse(cmdl, opt.name.base);
            if (is_use) {
                struct zd_cmdlopt *saved_opt;
                for (size_t i = 0; i < cmdl->opts.count; i++) {
                    saved_opt = zd_dyna_get(&cmdl->opts, i);
                    if (strcmp(saved_opt->name.base, opt.name.base) == 0)
                        break;
                }

                struct zd_string *val;
                for (size_t i = 0; i < opt.vals.count; i++) {
                    val = zd_dyna_get(&opt.vals, i);
                    zd_dyna_append(&saved_opt->vals, val);
                }
                struct zd_string *arg;
                for (size_t i = 0; i < opt.pargs.count; i++) {
                    arg = zd_dyna_get(&opt.pargs, i);
                    zd_dyna_append(&saved_opt->pargs, arg);
                }

                if (opt.vals.base)
                    free(opt.vals.base);
                if (opt.pargs.base)
                    free(opt.pargs.base);
                zd_string_destroy(&opt.name);
            } else {
                zd_dyna_append(&cmdl->opts, &opt);
            }
        } else {
            /* directly append it */
            zd_dyna_append(&cmdl->opts, &opt);
        }
    }
}

ZD_DEF void zd_cmdl_usage(struct zd_cmdl *cmdl)
{
    fprintf(stderr, "Usage: %s ...\n", cmdl->program.base);
    for (size_t i = 0; i < cmdl->rules.count; i++) {
        struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->rules, i);
        struct zd_string name = {0};
        zd_string_append(&name, "-%s --%s", opt->sname.base, opt->lname.base);
        fprintf(stderr, "  %-20s %s\n", name.base, opt->description.base);
        zd_string_destroy(&name);
    }
}

ZD_DEF bool zd_cmdl_isuse(struct zd_cmdl *cmdl, const char *optname)
{
    if (!optname)
        return false;

    for (size_t i = 0; i < cmdl->opts.count; i++) {
        struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->opts, i);
        if (opt->is_defined) {
            struct zd_cmdlopt *rule = _get_rule(cmdl, opt->name.base, NULL);
            if ((rule->lname.base && strcmp(rule->lname.base, optname) == 0) ||
                (rule->sname.base && strcmp(rule->sname.base, optname) == 0))
                return true;
        } else {
            if (opt->name.base && strcmp(opt->name.base, optname) == 0)
                return true;
        }
    }
    return false;
}

ZD_DEF struct zd_cmdlopt *zd_cmdl_get_opt(struct zd_cmdl *cmdl, 
        const char *optname)
{
    if (!optname)
        return false;

    for (size_t i = 0; i < cmdl->opts.count; i++) {
        struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->opts, i);
        if (opt->is_defined) {
            struct zd_cmdlopt *rule = _get_rule(cmdl, opt->name.base, NULL);
            if ((rule->lname.base && strcmp(rule->lname.base, optname) == 0) ||
                (rule->sname.base && strcmp(rule->sname.base, optname) == 0))
                return opt;
        } else {
            if (opt->name.base && strcmp(opt->name.base, optname) == 0)
                return opt;
        }
    }

    return NULL;
}

ZD_DEF void zd_cmdl_destroy(void *arg)
{
    struct zd_cmdl *cmdl = (struct zd_cmdl *) arg;
    zd_string_destroy(&cmdl->program);
    zd_dyna_destroy(&cmdl->pargs);
    zd_dyna_destroy(&cmdl->opts);
    zd_dyna_destroy(&cmdl->rules);
    cmdl->merge_opt = false;
}

ZD_DEF void zd_cmdlopt_destroy(void *arg)
{
    struct zd_cmdlopt *opt = (struct zd_cmdlopt *) arg;
    opt->type = OPTT_NO_ARG;
    opt->is_defined = false;
    zd_string_destroy(&opt->name);
    zd_string_destroy(&opt->lname);
    zd_string_destroy(&opt->sname);
    zd_dyna_destroy(&opt->vals);
    zd_dyna_destroy(&opt->pargs);
    zd_string_destroy(&opt->description);
}

#endif /* ZD_COMMAND_LINE */

#ifdef ZD_FS

static inline int count_line(char *buf, size_t size)
{
    if (!buf)
        return 0;

    int count = 0;
    for (size_t i = 0; i < size; i++)
        if (buf[i] == '\n')
            count++;

    if (size > 0 && buf[size - 1] != '\n')
        count++;

    return count;
}

/* return zero if error */
ZD_DEF unsigned long long zd_fs_get_timestamp(const char *pathname)
{
    unsigned long long timestamp = 0;

#if defined(_WIN32)
    HANDLE hFile = CreateFile(pathname, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR: unable to open file\n");
        return timestamp;
    }

    FILETIME ftCreate, ftAccess, ftWrite;
    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
        
        ULONGLONG ft = (((ULONGLONG) ftWrite.dwHighDateTime) << 32) + ftWrite.dwLowDateTime;
        timestamp = (unsigned long long)((ft - 116444736000000000LL) / 10000000);
    } else {
        fprintf(stderr, "ERROR: failed to get file time\n");
    }
    CloseHandle(hFile);
#elif defined(__linux__)
    struct stat sb;
    if (stat(pathname, &sb) == -1) {
        fprintf(stderr, "ERROR: unable to get file stats");
        return timestamp;
    }

    timestamp = (unsigned long long) sb.st_mtime;
#else
    NOT_SUPPORT("zd_fs_get_timestamp");
#endif /* platform */

    return timestamp;
}

ZD_DEF char *zd_fs_getname(const char *pathname)
{
    if (!pathname)
        return NULL;

#if defined(_WIN32)
    char *filename = strrchr(pathname, '\\');
#elif defined(__linux__)
    char *filename = strrchr(pathname, '/');
#else
    NOT_SUPPORT("zd_fs_getname");
#endif
    if (filename)
        return filename + 1;
    return (char *) pathname;
}

ZD_DEF bool zd_fs_pwd(char *buf, size_t buf_size)
{
    if (!buf || buf_size == 0)
        return false;

#if defined(_WIN32)
    if (GetCurrentDirectory(buf_size, buf))
        return true;
    else
        return false;
#elif defined(__linux__)
    if (getcwd(buf, buf_size))
        return true;
    else
        return false;
#else
    NOT_SUPPORT("zd_fs_pwd");
#endif /* platform */
}

ZD_DEF bool zd_fs_cd(const char *pathname)
{
    if (!pathname)
        return false;

#if defined(_WIN32)
    if (SetCurrentDirectory(pathname))
        return true;
    else
        return false;
#elif defined(__linux__)
    if (chdir(pathname) == 0)
        return true;
    else
        return false;
#else
    NOT_SUPPORT("zd_fd_cd");
#endif /* platform */
}

ZD_DEF bool zd_fs_move(const char *src, const char *dest, bool is_binary)
{
    if (zd_fs_typeof(src) != FT_REG)
        return false;

    if (!zd_fs_copy(src, dest, is_binary) || !zd_fs_remove(src))
        return false;
    return true;
}

ZD_DEF bool zd_fs_copy(const char *src, const char *dest, bool is_binary)
{
    if (zd_fs_typeof(src) != FT_REG)
        return false;

    char dest_path[MAX_PATH_SIZE];
    if (zd_fs_typeof(dest) == FT_NOET) {
        if (dest[strlen(dest) - 1] == '\\' || dest[strlen(dest) - 1] == '/')
            zd_fs_mkdir(dest);
    }

    if (zd_fs_typeof(dest) == FT_DIR)
        snprintf(dest_path, sizeof(dest_path), "%s/%s",
                dest, zd_fs_getname(src));
    else
        snprintf(dest_path, sizeof(dest_path), "%s", dest);

    struct zd_meta_file mf = {0};
    if (!zd_fs_loadf(src, &mf, is_binary))
        return false;
    if (!zd_fs_dumpf(dest_path, mf.content, mf.size, is_binary)) {
        zd_fs_destroy_mf(&mf);
        return false;
    }
    zd_fs_destroy_mf(&mf);
    return true;
}

ZD_DEF bool zd_fs_mkdir(const char *pathname)
{
    char path_copy[MAX_PATH_SIZE];
    strncpy(path_copy, pathname, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    /* process step by step in the path string */
    char *dir = path_copy;

#if defined(_WIN32)
    while ((dir = strchr(dir, '\\')) != NULL) {
        /* replace the path separator with a string terminator */
        *dir = '\0';
        /* create this directory */
        if (path_copy[0] != '\0' && _mkdir(path_copy) != 0 && errno != EEXIST)
            return false;
        /* restore the seperator and process next path */
        *dir = '\\';
        dir++;
    }
    /* create final directory */
    return (_mkdir(pathname) == 0 || errno == EEXIST);
#elif defined(__linux__)
    while ((dir = strchr(dir, '/')) != NULL) {
        /* replace the path separator with a string terminator */
        *dir = '\0';
        /* create this directory */
        if (path_copy[0] != '\0' && /* avoid path_copy to be "" in case /home/... */
                mkdir(path_copy, S_IRWXU | S_IRWXG | S_IRWXO) != 0 &&
                errno != EEXIST)
            return false;
        /* restore the seperator and process next path */
        *dir = '/';
        dir++;
    }
    return (mkdir(pathname, S_IRWXU | S_IRWXG | S_IRWXO) == 0 ||
            errno == EEXIST);
#else
        NOT_SUPPORT("zd_fs_mkdir");
#endif /* platform */
}

ZD_DEF bool zd_fs_touch(const char *pathname)
{
    FILE *fp = fopen(pathname, "a");
    if (fp == NULL)
        return false;
    fclose(fp);
    return true;
}

static bool _remove_dir_recursively(const char *dirpath)
{
#if defined(_WIN32)
    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = INVALID_HANDLE_VALUE;

    char dir[MAX_PATH_SIZE];
    snprintf(dir, sizeof(dir), "%s\\*", dirpath);

    h_find = FindFirstFile(dir, &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE)
        return false;

    do {
        if (strcmp(find_file_data.cFileName, ".") != 0 &&
                strcmp(find_file_data.cFileName, "..") != 0) {
            char filepath[MAX_PATH_SIZE];
            snprintf(filepath, sizeof(filepath), "%s\\%s",
                    dirpath, find_file_data.cFileName);

            int type = zd_fs_typeof(filepath);
            if (type == FT_NOET)
                break;

            if (type == FT_DIR)
                _remove_dir_recursively(filepath);
            else
                DeleteFile(filepath); 
        }
    } while (FindNextFile(h_find, &find_file_data) != 0);

    FindClose(h_find);
    return RemoveDirectory(dirpath) != 0;
#elif defined(__linux__)
    DIR *dp = opendir(dirpath);
    if (!dp)
        return false;

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
                strcmp(entry->d_name, "..") != 0) {
            char filepath[MAX_PATH_SIZE];
            snprintf(filepath, sizeof(filepath), "%s/%s",
                    dirpath, entry->d_name);

            int type = zd_fs_typeof(filepath);
            if (type == FT_NOET)
                break;

            if (type == FT_DIR) {
                if (!_remove_dir_recursively(filepath)) {
                    closedir(dp);
                    return false;
                }
            } else {
                if (unlink(filepath) != 0) {
                    closedir(dp);
                    return false;
                }
            }
        }
    }
    closedir(dp);
    return rmdir(dirpath) == 0;
#else
    NOT_SUPPORT("zd_fs_remove");
#endif /* platform */
}

ZD_DEF bool zd_fs_remove(const char *pathname)
{
    int type = zd_fs_typeof(pathname);
    if (type == FT_NOET) 
        return true;
    else if (type == FT_DIR)
        return _remove_dir_recursively(pathname);
    else
        return remove(pathname) == 0;
}

ZD_DEF int zd_fs_typeof(const char *pathname)
{
    if (!pathname)
        return FT_NOET;

#if defined(_WIN32)
    DWORD attr = GetFileAttributesA(pathname);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return FT_NOET;

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return FT_DIR;
    else
        return FT_REG;
#elif defined(__linux__)
    struct stat sb;
    if (stat(pathname, &sb) == -1) 
        return FT_NOET;

   switch (sb.st_mode & S_IFMT) {
   case S_IFBLK:
   case S_IFCHR:
   case S_IFIFO:
   case S_IFLNK:
   case S_IFSOCK:
   case S_IFREG:
       return FT_REG;
   case S_IFDIR:
       return FT_DIR;
   default: 
       return FT_NOET;
   }
#else
    NOT_SUPPORT("zd_fs_typeof");
#endif /* platform */
}

ZD_DEF bool zd_fs_loadd(const char *dirname, struct zd_meta_dir *res)
{
    if (!dirname)
        return false;

    if (zd_fs_typeof(dirname) != FT_DIR)
        return false;

    res->name = strdup(dirname);
    assert(res->name != NULL);

#if defined(_WIN32)
    char buf[MAX_PATH_SIZE];
    snprintf(buf, sizeof(buf), "%s\\*", dirname);
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(buf, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        free(res->name);
        res->name = NULL;
        return false;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 ||
                strcmp(findFileData.cFileName, "..") == 0)
            continue;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            res->dirs = realloc(res->dirs, sizeof(char *) * (res->d_cnt + 1));
            assert(res->dirs != NULL);
            res->dirs[res->d_cnt] = strdup(findFileData.cFileName);
            assert(res->dirs[res->d_cnt] != NULL);
            res->d_cnt++;
        } else {
            res->files = realloc(res->files, sizeof(char *) * (res->f_cnt + 1));
            assert(res->files != NULL);
            res->files[res->f_cnt] = strdup(findFileData.cFileName);
            assert(res->files[res->f_cnt] != NULL);
            res->f_cnt++;
        }

        res->count++;
    } while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
    return true;
#elif defined(__linux__)
    DIR *dp = opendir(dirname);
    if (!dp) {
        free(res->name);
        res->name = NULL;
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t len = strlen(dirname) + strlen(entry->d_name) + 2;
        char *full_path = (char *) malloc(len);
        snprintf(full_path, len, "%s/%s", dirname, entry->d_name);

        int type = zd_fs_typeof(full_path);
        if (type == FT_REG) {
            res->files = realloc(res->files, sizeof(char *) * (res->f_cnt + 1));
            assert(res->files != NULL);
            res->files[res->f_cnt] = strdup(entry->d_name);
            assert(res->files[res->f_cnt] != NULL);
            res->f_cnt++;
        } else {
            res->dirs = realloc(res->dirs, sizeof(char *) * (res->d_cnt + 1));
            assert(res->dirs != NULL);
            res->dirs[res->d_cnt] = strdup(entry->d_name);
            assert(res->dirs[res->d_cnt] != NULL);
            res->d_cnt++;
        }

        free(full_path);
        res->count++;
    }

    closedir(dp);
    return true;
#else
    NOT_SUPPORT("zd_fs_loadd");
#endif /* platform */
}

ZD_DEF bool zd_fs_loadf(const char *filename,
        struct zd_meta_file *res, bool is_binary)
{
    if (!filename)
        return false;

    res->type = zd_fs_typeof(filename);
    if (res->type != FT_REG)
        return false;

    res->name = strdup(filename);
    assert(res->name != NULL);

    char *mode = is_binary ? "rb" : "r";
    FILE *fp = fopen(filename, mode);
    if (!fp)
        return false;

    long saved_offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    res->size = (size_t) ftell(fp);
    fseek(fp, saved_offset, SEEK_SET);

    if (res->size == 0) {
        fclose(fp);
        return true;
    }

    res->content = malloc(res->size + 1);    
    assert(res->content != NULL);

#if defined(_WIN32)
    fread(res->content, 1, res->size, fp);
#else
    size_t read_size = fread(res->content, 1, res->size, fp);
    if (read_size != res->size) {
        fclose(fp);
        free(res->content);
        res->content = NULL;
        return false;
    }
#endif

    res->content[res->size] = '\0';
    res->line = count_line(res->content, res->size);

    fclose(fp);
    return true;
}

ZD_DEF void zd_fs_destroy_mf(struct zd_meta_file *mf)
{
    if (mf->name)
        free(mf->name);
    if (mf->content)
        free(mf->content);

    mf->name = NULL;
    mf->content = NULL;
    mf->type = FT_NOET;
    mf->size = 0;
    mf->line = 0;
}

ZD_DEF void zd_fs_destroy_md(struct zd_meta_dir *md)
{
    if (md->name)
        free(md->name);
    if (md->files) {
        for (size_t i = 0; i < md->f_cnt; i++) {
            if (md->files[i])
                free(md->files[i]);
            md->files[i] = NULL;
        }
        free(md->files);
    }
    if (md->dirs) {
        for (size_t i = 0; i < md->d_cnt; i++) {
            if (md->dirs[i])
                free(md->dirs[i]);
            md->dirs[i] = NULL;
        }
        free(md->dirs);
    }

    md->name = NULL;
    md->files = NULL;
    md->dirs = NULL;
    md->f_cnt = 0;
    md->d_cnt = 0;
    md->count = 0;
}

ZD_DEF bool zd_fs_dumpf(const char *dest_file,
        const char *src_buf, size_t size, bool is_binary)
{
    char *mode = is_binary ? "wb" : "w";
    FILE *fp = fopen(dest_file, mode);
    if (!fp) 
        return false;

    size_t write_size = fwrite(src_buf, 1, size, fp);
    if (write_size != size) {
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

#endif /* ZD_FS */

#ifdef ZD_TEST

#define ZD_TEST_COLOR_RESET   "\x1b[0m"
#define ZD_TEST_COLOR_RED     "\x1b[31m"
#define ZD_TEST_COLOR_GREEN   "\x1b[32m"

ZD_DEF void zd_pass(const char *msg)
{
    int case_id = __suite_ptr__->pass_count + __suite_ptr__->fail_count + 1;
    printf("<suite:%s> case %03d: [" ZD_TEST_COLOR_GREEN "pass" 
            ZD_TEST_COLOR_RESET "] %s\n", __suite_ptr__->name, case_id, 
            (msg != NULL) ? msg : ""); 
    fflush(stdout);
    __suite_ptr__->pass_count += 1;
}

ZD_DEF void zd_fail(const char *msg)
{
    int case_id = __suite_ptr__->pass_count + __suite_ptr__->fail_count + 1;
    printf("<suite:%s> case %03d: [" ZD_TEST_COLOR_RED "fail" 
            ZD_TEST_COLOR_RESET "] %s\n", __suite_ptr__->name, case_id,
            (msg != NULL) ? msg : ""); 
    fflush(stdout);
    __suite_ptr__->fail_count += 1;
}

ZD_DEF void zd_run_test(struct zd_testsuite *suite, char *(*test)(void))
{
    __suite_ptr__ = suite;
    char *msg = test();
    if (msg) 
        printf("<suite:%s> %s\n", __suite_ptr__->name, msg);
}

ZD_DEF void zd_test_summary(struct zd_testsuite *suite)
{
    printf("TEST SUITE: <%s>\n", suite->name);
    printf("      PASS: %zu\n", suite->pass_count);
    printf("      FAIL: %zu\n", suite->fail_count);
    printf("     TOTAL: %zu\n", suite->pass_count + suite->fail_count);
}

#undef ZD_TEST_COLOR_RESET
#undef ZD_TEST_COLOR_RED
#undef ZD_TEST_COLOR_GREEN

#endif /* ZD_TEST */

#ifdef ZD_DS_DYNAMIC_ARRAY

ZD_DEF void zd_dyna_init(struct zd_dyna *da, size_t size,
        void (*clear_item)(void *))
{
    da->base = NULL;
    da->count = 0;
    da->capacity = 0;
    da->clear_item = clear_item;
    da->size = size;
    da->pos = 0;
}

ZD_DEF void zd_dyna_append(struct zd_dyna *da, void *elem)
{
    if (da->capacity <= da->count) {
        #define ZD_DA_INIT_CAP 10
        da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
        #undef ZD_DA_INIT_CAP
        da->base = realloc(da->base, da->size * da->capacity);
        assert(da->base != NULL);
    }
    void *dest = zd_dyna_get(da, da->count++);
    memcpy(dest, elem, da->size);
}

ZD_DEF void zd_dyna_insert(struct zd_dyna *da, size_t idx, void *elem)
{
    if (idx > da->count) 
        return;
    else if (idx == da->count) 
        zd_dyna_append(da, elem);
    else {
        if (da->capacity <= da->count) {
            #define ZD_DA_INIT_CAP 10
            da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
            #undef ZD_DA_INIT_CAP
            da->base = realloc(da->base, da->size * da->capacity);
            assert(da->base != NULL);
        }
        void *dest = zd_dyna_get(da, idx);
        memcpy((char *) dest + da->size, dest, (da->count-idx) * da->size);
        memcpy(dest, elem, da->size);
        da->count++;
    }
}

ZD_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx)
{
    if (da->count > 0 && idx < da->count) {
        void *elem = zd_dyna_get(da, idx);
        if (da->clear_item)
            da->clear_item(elem);
        memcpy(zd_dyna_get(da, idx), zd_dyna_get(da, idx + 1),
                da->size * (da->count - idx - 1));
        da->count--;
    }
}

/* zd_dyna_set means: { clear_item(da[idx]); da[idx] = elem; } */
ZD_DEF void zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem)
{
    if (idx >= da->count) 
        return;
    void *dest = zd_dyna_get(da, idx);
    if (da->clear_item && dest) 
        da->clear_item(dest);
    memcpy(dest, elem, da->size);
}

/* zd_dyna_get means: return da[idx]; */
ZD_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx)
{
    if (idx >= da->count) return NULL;
    return (char *) da->base + da->size * idx;
}

ZD_DEF void zd_dyna_destroy(struct zd_dyna *da)
{
    if (da->clear_item) {
        for (size_t i = 0; i < da->count; i++) {
            void *item = (char *) da->base + da->size * i;
            da->clear_item(item);
        }
    }
    free(da->base);
    da->count = 0;
    da->capacity = 0;
    da->clear_item = NULL;
    da->size = 0;
    da->pos = 0;
}

/* A iterator */
ZD_DEF void *zd_dyna_next(struct zd_dyna *da)
{
    if (da->pos > da->count)
        da->pos = 0;
    return zd_dyna_get(da, da->pos++);
}

#endif /* ZD_DS_DYNAMIC_ARRAY */

#ifdef ZD_DS_STRING

ZD_DEF void zd_string_append(struct zd_string *str, const char *fmt, ...)
{
    if (!fmt)
        return;

    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);
    size_t len = vsnprintf(NULL, 0, fmt, args_copy);
    if (len <= 0) {
        va_end(args_copy);
        va_end(args);
        return;
    }
    va_end(args_copy);

    char new_str[len+1];
    vsnprintf(new_str, len + 1, fmt, args);
    new_str[len] = '\0';

    while (str->capacity <= str->length + len + 1) {
        #define ZD_STRING_INIT_CAP 128
        str->capacity = (str->capacity == 0) ? ZD_STRING_INIT_CAP 
                                             : (2 * str->capacity);
        #undef ZD_STRING_INIT_CAP
        str->base = realloc(str->base, str->capacity);
        assert(str->base != NULL);
    }
    memcpy(str->base + str->length, new_str, len);
    str->length += len;
    str->base[str->length] = '\0';

    va_end(args);
}

ZD_DEF struct zd_string zd_string_rep(const char *str, size_t times)
{
    struct zd_string res = {0};

    while (times--)
        zd_string_append(&res, str);

    return res;
}

/* [src, dest) */
ZD_DEF struct zd_string zd_string_sub(const char *str, size_t src, size_t dest)
{
    struct zd_string res = {0};
    if (src >= strlen(str) || dest > strlen(str) || src >= dest) 
        return res;

    char buf[dest - src + 1];
    memcpy(buf, str + src, dest - src);
    buf[dest - src] = '\0';
    zd_string_append(&res, buf);

    return res;
}

ZD_DEF void zd_string_destroy(void *arg)
{
    struct zd_string *str = (struct zd_string *) arg;
    if (str->base != NULL) free(str->base);
    str->base = NULL;
    str->length = 0;
    str->capacity = 0;
}

#endif /* ZD_DS_STRING */

#ifdef ZD_DS_DYNAMIC_BUFFER

ZD_DEF void zd_dynb_resize(struct zd_dynb *db, int size)
{
    if (size > 0) {
        db->capacity += size;
        db->base = realloc(db->base, db->capacity);
    } else {
        size_t abs_size = (size_t) -size;
        db->capacity = (db->capacity > abs_size) ? (db->capacity - abs_size) : 0;
        db->base = realloc(db->base, db->capacity);
    }
    assert((db->capacity >  0 && db->base != NULL) ||
           (db->capacity == 0 && db->base == NULL));
}

ZD_DEF void zd_dynb_destroy(void *arg)
{
    struct zd_dynb *db = (struct zd_dynb *) arg;
    if (db->base != NULL) {
        free(db->base);
        db->base = NULL;
    }
    db->capacity = 0;
}

#endif /* ZD_DS_DYNAMIC_BUFFER */

#ifdef ZD_DS_STACK

ZD_DEF void zd_stack_init(struct zd_stack *stk, size_t size,
        void (*clear_item)(void *))
{
    stk->base = NULL;
    stk->top = -1;
    stk->capacity = 0;
    stk->clear_item = clear_item;
    stk->size = size;
    if (size != 0)
        zd_dyna_init(&stk->gc, size, clear_item);
}

ZD_DEF void zd_stack_push(struct zd_stack *stk, void *elem)
{
    stk->top++;
    if (stk->capacity <= (size_t) stk->top) {
        #define ZD_STK_INIT_CAP 10
        stk->capacity = (stk->capacity == 0) ? ZD_STK_INIT_CAP : (2 * stk->capacity);
        #undef ZD_STK_INIT_CAP
        stk->base = realloc(stk->base, stk->size * stk->capacity);
        assert(stk->base != NULL);
    }
    void *dest = (char *) stk->base + stk->size * stk->top;
    memcpy(dest, elem, stk->size);
}

ZD_DEF void *zd_stack_pop(struct zd_stack *stk)
{
    if (stk->top == -1) 
        return NULL; 
    void *item = (char *) stk->base + stk->size * stk->top--;
    zd_dyna_append(&stk->gc, item);
    return item;
}

ZD_DEF void *zd_stack_top(struct zd_stack *stk)
{
    if (stk->top == -1) 
        return NULL; 
    return (char *) stk->base + stk->size * stk->top;
}

ZD_DEF void zd_stack_destroy(struct zd_stack *stk)
{
    if (stk->clear_item) {
        for (int i = 0; i <= stk->top; i++) {
            void *item = (char *) stk->base + stk->size * i;
            stk->clear_item(item);
        }
    }
    zd_dyna_destroy(&stk->gc);

    free(stk->base);
    stk->top = -1;
    stk->capacity = 0;
    stk->clear_item = NULL;
    stk->size = 0;
}

#endif /* ZD_DS_STACK */

#ifdef ZD_DYNASM

#if defined(__linux__)

ZD_DEF void *zd_dynasm_map(size_t size)
{
    if (size == 0) 
        size = ZD_PAGE_SIZE;
    void *addr = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (addr == MAP_FAILED) 
        return NULL;
    return addr;
}

ZD_DEF void *zd_dynasm_do(char *code, void *addr)
{
    if (!code || !addr) 
        return NULL;

    /* write the code into a asm file */

    char tmp_asm[] = "/tmp/tempfile_XXXXXX";

    int asm_fd = mkstemp(tmp_asm);
    if (asm_fd < 0) 
        return NULL;

    char buf[ZD_PAGE_SIZE];
    snprintf(buf, sizeof(buf), "use64\n%s", code);
    size_t asm_size = strlen(buf);

    if (write(asm_fd, buf, asm_size) != (ssize_t) asm_size) {
        close(asm_fd);
        unlink(tmp_asm);
        return NULL;
    }
    close(asm_fd);

    /* use assembler (fasm) to compile asm file, and output binary file */

    char tmp_bin[] = "/tmp/tempfile_XXXXXX";

    int bin_fd = mkstemp(tmp_bin);
    if (bin_fd < 0) 
        return NULL;

    pid_t pid = fork(); 
    if (pid == 0) {
        close(bin_fd);

        /* diable the output from child process */
        int devnull_fd = open("/dev/null", O_WRONLY);
        if (devnull_fd < 0) 
            _exit(1);

        dup2(devnull_fd, STDOUT_FILENO);
        dup2(devnull_fd, STDERR_FILENO);
        close(devnull_fd);

        execlp(ZD_ASSEMBLER, ZD_ASSEMBLER, tmp_asm, tmp_bin, NULL);
        _exit(1);
    }
    waitpid(pid, NULL, 0);

    unlink(tmp_asm);

    /* load the machine code into an mapped executable page */

    size_t read_size = read(bin_fd, buf, sizeof(buf));
    if (read_size > ZD_PAGE_SIZE) 
        read_size = ZD_PAGE_SIZE;

    memcpy(addr, buf, read_size);

    close(bin_fd);
    unlink(tmp_bin);

    return addr;
}

ZD_DEF void zd_dynasm_free(void *addr)
{
    munmap(addr, ZD_PAGE_SIZE);
}

#elif defined(_WIN32)
#else
#endif /* platform */

#endif /* ZD_DYNASM */

#ifdef ZD_DS_TRIE

#ifndef ZD_TRIE_SIZE
  #define ZD_TRIE_SIZE   26
#endif /* ZD_TRIE_SIZE */

#define ZD_TRIE_MAP(x) ((x) - 'a')

ZD_DEF struct zd_trie_node *zd_trie_create_node(void)
{
    struct zd_trie_node *node = malloc(sizeof(struct zd_trie_node));
    assert(node != NULL);

    for (int i = 0; i < ZD_TRIE_SIZE; i++)
        node->children[i] = NULL; 
    node->is_end = false;
    node->count = 0;

    return node;
}

ZD_DEF void zd_trie_insert(struct zd_trie_node *root, const char *word)
{
    if (!root) 
        return;

    struct zd_trie_node *cur = root;

    while (*word != '\0') {
        int index = ZD_TRIE_MAP(*word);
        if (cur->children[index] == NULL)
            cur->children[index] = zd_trie_create_node();
        cur = cur->children[index];
        word++;  
    }

    cur->is_end = true;
    cur->count++;
}

ZD_DEF int zd_trie_search(struct zd_trie_node *root, const char *word)
{
    if (!root) 
        return 0;

    struct zd_trie_node *cur = root;

    while (*word) {
        int index = ZD_TRIE_MAP(*word);
        if (cur->children[index] == NULL)
            cur->children[index] = zd_trie_create_node();
        cur = cur->children[index];
        word++;  
    }

    if (cur || cur->is_end)
        return cur->count;
    else
        return 0;
}

ZD_DEF void zd_trie_destroy(struct zd_trie_node *root)
{
    if (!root) 
        return;

    for (int i = 0; i < ZD_TRIE_SIZE; i++) {
        struct zd_trie_node *cur = root->children[i];
        if (cur) 
            zd_trie_destroy(cur);
    }
    free(root);
}

#endif /* ZD_DS_TRIE */

#ifdef ZD_DS_QUEUE

static void *zd_queue_create_node(void *elem, size_t size)
{
    struct zd_queue_node *np = malloc(sizeof(struct zd_queue_node));
    assert(np != NULL);

    np->data = malloc(size);
    assert(np->data != NULL);
    memcpy(np->data, elem, size);
    np->next = NULL;

    return np;
}

ZD_DEF void zd_queue_init(struct zd_queue *qp, size_t size,
        void (*clear_item)(void *))
{
    qp->head = (struct zd_queue_node) {0};
    qp->front = NULL;
    qp->rear  = NULL;
    qp->count = 0;
    qp->size  = size;
    qp->clear_item = clear_item;
    if (size != 0)
        zd_dyna_init(&qp->gc, sizeof(void *), NULL);
}

ZD_DEF void zd_queue_destroy(struct zd_queue *qp)
{
    struct zd_queue_node *np = qp->head.next, *tmp = NULL;
    while (np) {
        tmp = np->next;
        if (qp->clear_item) 
            qp->clear_item(np->data);
        free(np->data);
        free(np);
        np = tmp;
    }

    void **gc_item;
    while ((gc_item = zd_dyna_next(&qp->gc)) != NULL) {
        if (qp->clear_item)
            qp->clear_item(*gc_item);
        free(*gc_item);
    }
    zd_dyna_destroy(&qp->gc);

    zd_queue_init(qp, 0, NULL);
}

ZD_DEF void zd_queue_push(struct zd_queue *qp, void *elem)
{
    struct zd_queue_node *np = zd_queue_create_node(elem, qp->size);

    if (zd_queue_isempty(qp)) {
        qp->front = np;
        qp->rear = np;
        qp->head.next = qp->front;
    } else {
        qp->rear->next = np;
        qp->rear = qp->rear->next;
    }

    qp->count++;
}

ZD_DEF void *zd_queue_pop(struct zd_queue *qp)
{
    if (zd_queue_isempty(qp)) {
        return NULL;
    } else if (qp->front == qp->rear) {
        /* just one element */
        void *res = qp->front->data;

        zd_dyna_append(&qp->gc, &qp->front->data);
        free(qp->front);
        qp->head = (struct zd_queue_node) {0};
        qp->front = NULL;
        qp->rear  = NULL;
        qp->count = 0;

        return res;
    } else {
        void *res = qp->front->data;
        qp->head.next = qp->front->next;

        zd_dyna_append(&qp->gc, &qp->front->data);
        free(qp->front);

        qp->front = qp->head.next;
        qp->count--;

        return res;
    }
}

ZD_DEF void *zd_queue_front(struct zd_queue *qp)
{
    if (zd_queue_isempty(qp))
        return NULL;
    return qp->front->data;
}

ZD_DEF void *zd_queue_rear(struct zd_queue *qp)
{
    if (zd_queue_isempty(qp))
        return NULL;
    return qp->rear->data;
}

#endif /* ZD_DS_QUEUE */

#ifdef ZD_PRINT

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

#define _make_rgb(x, r, g, b)                               \
    do {                                                    \
        double factor = 0.3;                                \
        r = (int)(sin(factor*(double)x+0*M_PI/3)*127+128);  \
        g = (int)(sin(factor*(double)x+2*M_PI/3)*127+128);  \
        b = (int)(sin(factor*(double)x+4*M_PI/3)*127+128);  \
    } while (0)

#define _make_color(buf, pos, ch, r, g, b)              \
    do {                                                \
        char *fmt = "\x1b[38;2;%d;%d;%dm%c\x1b[0m";     \
        sprintf(buf+pos, fmt, r, g, b, ch);             \
        pos += strlen(fmt);                             \
    } while (0)

static void zd_print_color(const char *fmt, va_list args)
{
#if defined(_WIN32) || defined(__linux__)
    va_list args_copy;
    va_copy(args_copy, args);
    size_t len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    size_t raw_buf_size = len + 1;  /* extra one for '\0' */
    char *raw_buf = malloc(raw_buf_size);
    assert(raw_buf != NULL);

    /* output the format string */

    vsnprintf(raw_buf, raw_buf_size, fmt, args);

    /* generate colorful string */

    size_t res_buf_size = 2 * raw_buf_size;
    char *res_buf = malloc(res_buf_size);
    assert(res_buf != NULL);
    size_t pos = 0;

    int r, g, b;
    for (size_t i = 0; i < strlen(raw_buf); i++) {
        _make_rgb(i, r, g, b);
        _make_color(res_buf, pos, raw_buf[i], r, g, b);

        if (pos >= res_buf_size - 200) {
            res_buf_size *= 2;
            res_buf = realloc(res_buf, res_buf_size);
            assert(res_buf != NULL);
        }
    }
    res_buf[pos] = '\0';

    printf("%s", res_buf);

    free(raw_buf);
    free(res_buf);
#else
    NOT_SUPPORT("zd_print_color");
#endif /* platform */
}

#define _print_seperator(max_widths, col)                       \
    do {                                                        \
        for (size_t j = 0; j < col; j++) {                      \
            printf("+");                                        \
            for (size_t i = 0; i < max_widths[j] + 2; i++)      \
                printf("-");                                    \
        }                                                       \
        printf("+\n");                                          \
    } while (0)                                                 \


#define _find_max_width(max_widths, row, col, tbl)      \
    do {                                                \
        for (size_t i = 0; i < row; i++) {              \
            for (size_t j = 0; j < col; j++) {          \
                const char *item = tbl[i][j];           \
                size_t len = strlen(item);              \
                if (len > (size_t) max_widths[j])       \
                    max_widths[j] = len;                \
            }                                           \
        }                                               \
    } while (0)

#define zd_print_table(tbl, row, col)                                   \
    do {                                                                \
        size_t max_widths[col];                                         \
        memset(max_widths, 0, sizeof(max_widths));                      \
                                                                        \
        _find_max_width(max_widths, row, col, (tbl));                   \
                                                                        \
        for (size_t i = 0; i < row; i++) {                              \
            _print_seperator(max_widths, col);                          \
            for (size_t j = 0; j < col; j++)                            \
                printf("| %-*s ", (int) max_widths[j], (tbl)[i][j]);    \
            printf("|\n");                                              \
        }                                                               \
        _print_seperator(max_widths, col);                              \
    } while (0)

ZD_DEF void zd_print(int opt, ...)
{
    va_list args;
    va_start(args, opt);

    switch (opt) {
        case OPT_COLOR: {
            const char *fmt = va_arg(args, const char *);
            zd_print_color(fmt, args);
        } break;

        case OPT_S_TBL: {
            const char ***arg = va_arg(args, const char ***);
            size_t row  = va_arg(args, size_t);
            size_t col  = va_arg(args, size_t);
            const char *(*tbl)[col] = (const char *(*)[col]) arg;
            zd_print_table(tbl, row, col);
        } break;

        case OPT_D_TBL: {
            const char ***arg = va_arg(args, const char ***);
            size_t row = va_arg(args, size_t);
            size_t col = va_arg(args, size_t);
            const char ***tbl = arg;
            zd_print_table(tbl, row, col);
        } break;

        default:
            break;
    }

    va_end(args);
}

#undef _make_color
#undef _make_rgb
#undef _print_seperator
#undef _find_max_width
#undef zd_print_table

#endif /* ZD_PRINT */

#ifdef ZD_LOG

#define ZD_LOG_COLOR_RESET      "\x1b[0m"
#define ZD_LOG_COLOR_FG_RED     "\x1b[31m"
#define ZD_LOG_COLOR_FG_GREEN   "\x1b[32m"
#define ZD_LOG_COLOR_FG_YELLOW  "\x1b[33m"
#define ZD_LOG_COLOR_BG_RED     "\x1b[41m"
#define ZD_LOG_COLOR_BG_GREEN   "\x1b[42m"

ZD_DEF void zd_log(int type, const char *fmt, ...)
{
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);
    size_t len = vsnprintf(NULL, 0, fmt, args_copy);
    if (len <= 0) {
        va_end(args_copy);
        va_end(args);
        return;
    }
    va_end(args_copy);

    size_t buf_size = len + 100;
    char buf1[buf_size], buf2[buf_size];
    bool is_exit = false;

    switch ((type)) {
    case LOG_INFO:
        snprintf(buf1, buf_size, "[%sINFO%s] %s\n", ZD_LOG_COLOR_FG_YELLOW,
                ZD_LOG_COLOR_RESET, fmt);
        break;

    case LOG_ERROR:
        snprintf(buf1, buf_size, "[%sERROR%s] %s\n", ZD_LOG_COLOR_FG_RED,
                ZD_LOG_COLOR_RESET, fmt);
        break;

    case LOG_GOOD:
        snprintf(buf1, buf_size, "[%sGOOD%s] %s\n", ZD_LOG_COLOR_FG_GREEN,
                ZD_LOG_COLOR_RESET, fmt);
        break;

    case LOG_FATAL:
        snprintf(buf1, buf_size, "[%sFATAL%s] %s\n", ZD_LOG_COLOR_BG_RED,
                ZD_LOG_COLOR_RESET, fmt);
        is_exit = true;
        break;

    case LOG_TODO:
        snprintf(buf1, buf_size, "[%sTODO%s] %s\n", ZD_LOG_COLOR_BG_GREEN,
                ZD_LOG_COLOR_RESET, fmt);
        is_exit = true;
        break;

    default:
        break;
    }

    vsnprintf(buf2, buf_size, buf1, args);
    fprintf(stderr, "%s", buf2);

    va_end(args);

    if (is_exit)
        exit(EXIT_FAILURE);
}

#endif /* ZD_LOG */

#ifdef ZD_BUILD

static void _cmd_append_arg(struct zd_cmd *cmd, ...)
{
    va_list ap;
    va_start(ap, cmd);

    char *arg = NULL;
    while ((arg = va_arg(ap, char *)) != NULL) {
        struct zd_string str = {0};
        zd_string_append(&str, arg);
        zd_dyna_append(&cmd->args, &str);
        cmd->count++;
    }

    va_end(ap);
}

ZD_DEF void zd_cmd_print(struct zd_cmd *cmd)
{
    struct zd_string cmd_string = {0};

    struct zd_string *arg_iter = NULL;
    while ((arg_iter = zd_dyna_next(&cmd->args)) != NULL)
        zd_string_append(&cmd_string, " %s", arg_iter->base);

    zd_log(LOG_INFO, cmd_string.base);

    zd_string_destroy(&cmd_string);
}

ZD_DEF void zd_cmd_init(struct zd_cmd *cmd)
{
    zd_dyna_init(&cmd->args, sizeof(struct zd_string), zd_string_destroy);
    cmd->count = 0;
}

ZD_DEF void zd_cmd_destroy(struct zd_cmd *cmd)
{
    zd_dyna_destroy(&cmd->args);
    cmd->count = 0;
}

ZD_DEF int zd_cmd_run(struct zd_cmd *cmd)
{
    struct zd_string cmd_string = {0};

    for (size_t i = 0; i < cmd->count; i++) {
        char *arg = ((struct zd_string *) zd_dyna_get(&cmd->args, i))->base;
        zd_string_append(&cmd_string, " %s", arg);
    }
#if defined(_WIN32)
    zd_string_append(&cmd_string, " %s", "-D__USE_MINGW_ANSI_STDIO");
#endif /* platform */
    zd_string_append(&cmd_string, " ");

    int status = system(cmd_string.base);

    if (status != 0) {
        zd_log(LOG_ERROR, "run failed [%s]", cmd_string.base);
        zd_string_destroy(&cmd_string);
        return 1;
    }
    zd_log(LOG_GOOD, "run successfully [%s]", cmd_string.base);

    zd_string_destroy(&cmd_string);
    return 0;
}

ZD_DEF void zd_build_init(struct zd_builder *builder)
{
    zd_dyna_init(&builder->cmds, sizeof(struct zd_cmd), NULL);
    builder->count = 0;
    builder->bd_src = "build.c";
    builder->bd_src = "build";
#if defined(__linux__)
    builder->cmds_status = NULL;
    builder->cmds_exec_count = 0;
#endif
}

static void _build_append_cmd(struct zd_builder *builder, ...)
{
    va_list ap;
    va_start(ap, builder);

    struct zd_cmd *cmd = NULL;
    while ((cmd = va_arg(ap, struct zd_cmd *)) != NULL) {
        zd_dyna_append(&builder->cmds, cmd);
        builder->count++;
    }

    va_end(ap);
}

ZD_DEF void zd_build_print(struct zd_builder *builder)
{
    struct zd_cmd *cmd_iter = NULL;
    while ((cmd_iter = zd_dyna_next(&builder->cmds)) != NULL) {
        struct zd_string cmd_string = {0};

        struct zd_string *arg_iter = NULL;
        while ((arg_iter = zd_dyna_next(&cmd_iter->args)) != NULL)
            zd_string_append(&cmd_string, " %s", arg_iter->base);

        zd_log(LOG_INFO, cmd_string.base);

        zd_string_destroy(&cmd_string);
    }
}

ZD_DEF void zd_build_destroy(struct zd_builder *builder)
{
    struct zd_cmd *cmd_iter = NULL;
    while ((cmd_iter = zd_dyna_next(&builder->cmds)) != NULL)
        zd_cmd_destroy(cmd_iter);

    zd_dyna_destroy(&builder->cmds);

    builder->count = 0;
    builder->bd_src = NULL;
    builder->bd_exe = NULL;

#if defined(__linux__)
    builder->cmds_exec_count = 0;
    if (builder->cmds_status != NULL) {
        free(builder->cmds_status);
        builder->cmds_status = NULL;
    }
#endif
}

ZD_DEF int zd_build_run_sync(struct zd_builder *builder)
{
    for (size_t i = 0; i < builder->count; i++) {
        struct zd_cmd *cmd = zd_dyna_get(&builder->cmds, i);
        int status = zd_cmd_run(cmd);
        if (status != 0)
            return 1;
    }
    return 0;
}

#if defined(__linux__)
static int run_cmd_async(void *arg)
{
    struct zd_builder *builder = (struct zd_builder *) arg;

    while (1) {
        mtx_lock(&builder->cmds_exec_count_mutex);

        if (builder->cmds_exec_count >= builder->count) {
            mtx_unlock(&builder->cmds_exec_count_mutex);
            break;
        }

        size_t i = builder->cmds_exec_count;
        if (builder->cmds_status[i] == EXEC_UNDO) {
            struct zd_cmd *cmd = zd_dyna_get(&builder->cmds, i);
            int status = zd_cmd_run(cmd);
            if (status == 0) 
                builder->cmds_status[i] = EXEC_OK;
            else
                builder->cmds_status[i] = EXEC_ERROR;
            builder->cmds_exec_count++;
        }

        mtx_unlock(&builder->cmds_exec_count_mutex);
    }

    return 0;
}

ZD_DEF int zd_build_run_async(struct zd_builder *builder)
{
    /* initialize */

    builder->cmds_status = malloc(sizeof(int) * builder->count);
    assert(builder->cmds_status != NULL);
    for (size_t i = 0; i < builder->count; i++)
        builder->cmds_status[i] = EXEC_UNDO;

    builder->cmds_exec_count = 0;

    size_t thrd_count = (builder->count <= 10) ? builder->count : 10;
    thrd_t threads[thrd_count];

    mtx_init(&builder->cmds_exec_count_mutex, mtx_plain);

    /* start work */

    for (size_t i = 0; i < thrd_count; i++) {
        int status = thrd_create(&threads[i], run_cmd_async, builder);
        if (status != thrd_success) {
            mtx_destroy(&builder->cmds_exec_count_mutex);
            free(builder->cmds_status);
            builder->cmds_status = NULL;
            return 1;
        }
    }

    /* wait until threads done */

    for (size_t i = 0; i < thrd_count; i++)
        thrd_join(threads[i], NULL);

    /* make sure all commands have been executed  */

    for (size_t i = 0; i < builder->count; i++) {
        if (builder->cmds_status[i] == EXEC_UNDO) {
            mtx_destroy(&builder->cmds_exec_count_mutex);
            free(builder->cmds_status);
            builder->cmds_status = NULL;
            return 1;
        }
    }

    /* release sources */

    mtx_destroy(&builder->cmds_exec_count_mutex);
    free(builder->cmds_status);
    builder->cmds_status = NULL;

    return 0;
}
#endif

#endif /* ZD_BUILD */

#ifdef ZD_COROUTINE

#if defined(__linux__) && defined(__GNUC__)

static void __attribute__((naked)) zd_coctx_swap(struct zd_coctx *cur,
        struct zd_coctx *next)
{
    (void) cur;
    (void) next;
    
    __asm__ __volatile__(
        /* store current coroutine's context */
        "leaq (%rsp), %rax\n\t"
        "movq %rax, 104(%rdi)\n\t"
        "movq %rbx, 96(%rdi)\n\t"
        "movq %rcx, 88(%rdi)\n\t"
        "movq %rdx, 80(%rdi)\n\t"
        "movq 0(%rax), %rax\n\t"
        "movq %rax, 72(%rdi)\n\t"
        "movq %rsi, 64(%rdi)\n\t"
        "movq %rdi, 56(%rdi)\n\t"
        "movq %rbp, 48(%rdi)\n\t"
        "movq %r8, 40(%rdi)\n\t"
        "movq %r9, 32(%rdi)\n\t"
        "movq %r12, 24(%rdi)\n\t"
        "movq %r13, 16(%rdi)\n\t"
        "movq %r14, 8(%rdi)\n\t"
        "movq %r15, (%rdi)\n\t"
        "xorq %rax, %rax\n\t"

        /* restore next coroutine's context */
        "movq 48(%rsi), %rbp\n\t"
        "movq 104(%rsi), %rsp\n\t"
        "movq (%rsi), %r15\n\t"
        "movq 8(%rsi), %r14\n\t"
        "movq 16(%rsi), %r13\n\t"
        "movq 24(%rsi), %r12\n\t"
        "movq 32(%rsi), %r9\n\t"
        "movq 40(%rsi), %r8\n\t"
        "movq 56(%rsi), %rdi\n\t"
        "movq 80(%rsi), %rdx\n\t"
        "movq 88(%rsi), %rcx\n\t"
        "movq 96(%rsi), %rbx\n\t"
        "leaq 8(%rsp), %rsp\n\t"
        "pushq 72(%rsi)\n\t"
        "movq 64(%rsi), %rsi\n\t"
        "ret\n\t"
    );
}

static inline void clear_coroutine(void *arg)
{
    struct zd_coctx *co = (struct zd_coctx *) arg;
    memset(co->regs, 0, sizeof(co->regs));
    if (co->stack_base != NULL)
        free(co->stack_base);
    co->stack_base = NULL;
    co->stack_size = 0;
    co->status = -1;
    co->id = -1;
}

ZD_DEF struct zd_colib *zd_colib_init(void)
{
    struct zd_colib *colib = malloc(sizeof(struct zd_colib));
    assert(colib != NULL);

    zd_dyna_init(&colib->coctxs, sizeof(struct zd_coctx), clear_coroutine);
    zd_stack_init(&colib->back_stk, sizeof(struct zd_coctx *), NULL);

    /* main coroutine */
    struct zd_coctx co = {0};
    co.status = ST_RUNNING;
    co.id = colib->coctxs.count;

    zd_dyna_append(&colib->coctxs, &co);

    colib->cur_coctx = zd_dyna_get(&colib->coctxs, colib->coctxs.count - 1);

    return colib;
}

ZD_DEF void zd_colib_destroy(struct zd_colib *colib)
{
    colib->cur_coctx = NULL;
    zd_stack_destroy(&colib->back_stk);
    zd_dyna_destroy(&colib->coctxs);
    free(colib);
}

ZD_DEF struct zd_coctx *zd_coctx_create(struct zd_colib *colib,
        void (*task)(void *), void *arg)
{
    struct zd_coctx co = {0};
    co.stack_size = COROUTINE_STACK_SIZE;
    co.stack_base = malloc(COROUTINE_STACK_SIZE);
    assert(co.stack_base != NULL);
    co.regs[CTX_RET] = task;
    co.regs[CTX_RSP] = (char *) co.stack_base
        + co.stack_size - PROTECTION_REGION;     /* reserve some bytes for protection */
    co.regs[CTX_RDI] = arg;
    co.status = ST_READY;
    co.id = colib->coctxs.count;

    zd_dyna_append(&colib->coctxs, &co);

    return zd_dyna_get(&colib->coctxs, colib->coctxs.count - 1);
}

ZD_DEF void zd_coctx_resume(struct zd_colib *colib, struct zd_coctx *next)
{
    if (next->status != ST_SUSPEND && next->status != ST_READY)
        return;

    zd_stack_push(&colib->back_stk, &colib->cur_coctx);
    colib->cur_coctx->status = ST_SUSPEND;

    struct zd_coctx *cur = colib->cur_coctx;
    colib->cur_coctx = next;
    colib->cur_coctx->status = ST_RUNNING;

    zd_coctx_swap(cur, next);
}

ZD_DEF void zd_coctx_yield(struct zd_colib *colib)
{
    if (colib->cur_coctx->status != ST_RUNNING)
        return;

    colib->cur_coctx->status = ST_SUSPEND;
    struct zd_coctx *cur = colib->cur_coctx;

    struct zd_coctx *next = *(struct zd_coctx **) 
        zd_stack_pop(&colib->back_stk);
    assert(next != NULL);
    colib->cur_coctx = next;
    colib->cur_coctx->status = ST_RUNNING;

    zd_coctx_swap(cur, next);
}

ZD_DEF void zd_coctx_finish(struct zd_colib *colib)
{
    colib->cur_coctx->status = ST_DEAD;
    struct zd_coctx *cur = colib->cur_coctx;

    struct zd_coctx *next = *(struct zd_coctx **) 
        zd_stack_pop(&colib->back_stk);
    assert(next != NULL);
    colib->cur_coctx = next;
    colib->cur_coctx->status = ST_RUNNING;

    zd_coctx_swap(cur, next);
}

ZD_DEF void zd_coctx_collect(struct zd_colib *colib, struct zd_coctx *co)
{
    if (co->status != ST_DEAD)
        return;
    size_t off = co - (struct zd_coctx *) colib->coctxs.base;
    if (off >= colib->coctxs.count)
        return;
    zd_dyna_remove(&colib->coctxs, off);
}

ZD_DEF size_t zd_coctx_alive(struct zd_colib *colib)
{
    struct zd_coctx *co_iter = NULL;
    size_t alive = 0;

    while ((co_iter = zd_dyna_next(&colib->coctxs)) != NULL) {
        if (co_iter->status != ST_DEAD)
            alive += 1;
    }
    colib->coctxs.pos = 0;

    return alive;
}

ZD_DEF int zd_coctx_workid(struct zd_colib *colib)
{
    return colib->cur_coctx->id;
}

#endif /* platform */

#endif /* ZD_COROUTINE */

#ifdef ZD_DS_LINKED_LIST

static struct zd_list_node *zd_list_create_node(void *elem, size_t size)
{
    struct zd_list_node *node = malloc(sizeof(struct zd_list_node));
    assert(node != NULL);

    node->data = malloc(size);
    assert(node->data != NULL);
    memcpy(node->data, elem, size);
    node->next = NULL;

    return node;
}

ZD_DEF void zd_list_append(struct zd_list *list, void *elem)
{
    struct zd_list_node *node = zd_list_create_node(elem, list->size);
    struct zd_list_node *prev = &list->head, *cur = list->head.next;

    for (size_t i = 0; i < list->count; i++) {
        prev = cur;
        cur = cur->next;
    }

    node->next = cur;
    prev->next = node;

    list->count++;
}

ZD_DEF void zd_list_insert(struct zd_list *list, size_t idx, void *elem)
{
    struct zd_list_node *node = zd_list_create_node(elem, list->size);
    struct zd_list_node *prev = &list->head, *cur = list->head.next;

    if (idx > list->count)
        idx = list->count;

    for (size_t i = 0; i < idx; i++) {
        prev = cur;
        cur = cur->next;
    }

    node->next = cur;
    prev->next = node;

    list->count++;
}

ZD_DEF void zd_list_remove(struct zd_list *list, size_t idx)
{
    if (idx >= list->count)
        return;

    struct zd_list_node *prev = &list->head, *cur = list->head.next;

    for (size_t i = 0; i < idx; i++) {
        prev = cur;
        cur = cur->next;
    }

    prev->next = cur->next;

    if (list->clear_item)
        list->clear_item(cur->data);
    free(cur->data);
    cur->data = NULL;
    cur->next = NULL;
    free(cur);

    list->count--;
}

ZD_DEF void *zd_list_get(struct zd_list *list, size_t idx)
{
    if (idx >= list->count)
        return NULL;

    struct zd_list_node *cur = list->head.next;

    for (size_t i = 0; i < idx; i++)
        cur = cur->next;

    return cur->data;
}

ZD_DEF void zd_list_set(struct zd_list *list, size_t idx, void *elem)
{
    if (idx >= list->count)
        return;

    struct zd_list_node *cur = list->head.next, *prev = &list->head;

    for (size_t i = 0; i < idx; i++) {
        prev = cur;
        cur = cur->next;
    }

    struct zd_list_node *node = zd_list_create_node(elem, list->size);
    prev->next = node;
    node->next = cur->next;

    if (list->clear_item)
        list->clear_item(cur->data);
    free(cur->data);
    cur->data = NULL;
    cur->next = NULL;
    free(cur);
}

ZD_DEF void zd_list_reverse(struct zd_list *list)
{
    struct zd_list_node *cur = list->head.next, *prev = NULL, *next;

    while (cur) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;  
    }
    list->head.next = prev;
}

ZD_DEF void zd_list_init(struct zd_list *list, size_t size,
        void (*clear_item)(void *))
{
    list->head = (struct zd_list_node) {
        .data = NULL,
        .next = NULL
    };
    list->count = 0;
    list->size = size;
    list->clear_item = clear_item;
}

ZD_DEF void zd_list_destroy(struct zd_list *list)
{
    struct zd_list_node *cur = list->head.next, *next = NULL;
    while (cur) {
        next = cur->next;
        if (list->clear_item)
            list->clear_item(cur->data);
        free(cur->data);
        cur->data = NULL;
        cur->next = NULL;
        free(cur);
        cur = next;
    }

    list->head = (struct zd_list_node) {0};
    list->count = 0;
    list->size = 0;
    list->clear_item = NULL;
}

#endif /* ZD_DS_LINKED_LIST */

#ifdef ZD_DS_HASH

ZD_DEF void zd_htbl_init(struct zd_hash_tbl *htbl, size_t ksz, size_t vsz,
        bool (*key_cmp)(void *k1, void *k2), size_t (*hash_func)(void *),
        void (*key_free)(void *), void (*val_free)(void *))
{

    htbl->capacity = 1 / LOAD_TH_LOWER;
    htbl->buckets = malloc(sizeof(struct zd_hash_node *) * htbl->capacity);
    assert(htbl->buckets != NULL);
    for (size_t i = 0; i < htbl->capacity; i++) {
        htbl->buckets[i] = malloc(sizeof(struct zd_hash_node));
        assert(htbl->buckets[i]);
        htbl->buckets[i]->next = NULL;
    }
    htbl->load      = 0;
    htbl->count     = 0;
    htbl->key_size  = ksz;
    htbl->val_size  = vsz;
    htbl->key_cmp   = key_cmp;
    htbl->hash_func = hash_func;
    htbl->key_free  = key_free;
    htbl->val_free  = val_free;
}

static struct zd_hash_node *zd_htbl_create_node(struct zd_hash_tbl *htbl,
        void *key, void *val)
{
    struct zd_hash_node *node = malloc(sizeof(struct zd_hash_node));
    assert(node != NULL);

    node->key = malloc(htbl->key_size);
    assert(node->key != NULL);
    memcpy(node->key, key, htbl->key_size);
    node->val = malloc(htbl->val_size);
    assert(node->val != NULL);
    memcpy(node->val, val, htbl->val_size);
    node->next = NULL;

    return node;
}

ZD_DEF void zd_htbl_resize(struct zd_hash_tbl *htbl, int mode)
{
    struct zd_hash_tbl new_ht = *htbl;
    if (mode == HASH_EXPAND)
        new_ht.capacity = htbl->capacity * 2;
    else
        new_ht.capacity = htbl->capacity / 2;
    new_ht.load = 0;
    new_ht.count = 0;
    new_ht.buckets = malloc(sizeof(struct zd_hash_node *) * new_ht.capacity);
    assert(new_ht.buckets);
    for (size_t i = 0; i < new_ht.capacity; i++) {
        new_ht.buckets[i] = malloc(sizeof(struct zd_hash_node));
        assert(new_ht.buckets[i] != NULL);
        new_ht.buckets[i]->next = NULL;
    }

    for (size_t i = 0; i < htbl->capacity; i++) {
        struct zd_hash_node *cur = htbl->buckets[i]->next;
        while (cur) {
            zd_htbl_insert(&new_ht, cur->key, cur->val);
            cur = cur->next;
        }
    }

    struct zd_hash_tbl tmp_ht = *htbl;
    *htbl = new_ht;

    tmp_ht.key_free = NULL;
    tmp_ht.val_free = NULL;
    zd_htbl_destroy(&tmp_ht);
}

ZD_DEF void zd_htbl_insert(struct zd_hash_tbl *htbl, void *key, void *val)
{
    size_t idx = htbl->hash_func(key) % htbl->capacity;
    struct zd_hash_node *cur = htbl->buckets[idx];
    while (cur && cur->next)
        cur = cur->next;

    struct zd_hash_node *node = zd_htbl_create_node(htbl, key, val);

    cur->next = node;
    htbl->count++;

    htbl->load = (float) htbl->count / (float) htbl->capacity;
    if (htbl->load > LOAD_TH_UPPER)
        zd_htbl_resize(htbl, HASH_EXPAND);
}

ZD_DEF bool zd_htbl_search(struct zd_hash_tbl *htbl, void *key)
{
    size_t idx = htbl->hash_func(key) % htbl->capacity;
    struct zd_hash_node *cur = htbl->buckets[idx]->next;

    while (cur) {
        if (htbl->key_cmp(cur->key, key))
            return true;
        cur = cur->next;
    }

    return false;
}

static void zd_htbl_destroy_node(struct zd_hash_tbl *htbl,
        struct zd_hash_node *node)
{
    if (htbl->key_free)
        htbl->key_free(node->key);
    if (htbl->val_free)
        htbl->val_free(node->val);
    free(node->key);
    free(node->val);
    node->next = NULL;
    free(node);
}

ZD_DEF void zd_htbl_remove(struct zd_hash_tbl *htbl, void *key)
{
    if (zd_htbl_search(htbl, key) == false)
        return;

    size_t idx = htbl->hash_func(key) % htbl->capacity;
    struct zd_hash_node *cur = htbl->buckets[idx]->next,
                        *prev = htbl->buckets[idx];

    while (cur) {
        if (htbl->key_cmp(cur->key, key)) {
            prev->next = cur->next;
            zd_htbl_destroy_node(htbl, cur);
            htbl->count--;
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    htbl->load = (float) htbl->count / (float) htbl->capacity;
    if (htbl->load < LOAD_TH_LOWER)
        zd_htbl_resize(htbl, HASH_SHRINK);
}

ZD_DEF void zd_htbl_destroy(struct zd_hash_tbl *htbl)
{
    for (size_t i = 0; i < htbl->capacity; i++) {
        struct zd_hash_node *cur = htbl->buckets[i]->next, *next = NULL;
        while (cur) {
            next = cur->next;
            zd_htbl_destroy_node(htbl, cur);
            cur = next;
        }
        free(htbl->buckets[i]);
    }
    if (htbl->buckets)
        free(htbl->buckets);

    htbl->buckets   = NULL;
    htbl->load      = 0;
    htbl->count     = 0;
    htbl->capacity  = 0;
    htbl->load      = 0;
    htbl->key_size  = 0;
    htbl->val_size  = 0;
    htbl->key_cmp   = NULL;
    htbl->hash_func = NULL;
    htbl->key_free  = NULL;
    htbl->val_free  = NULL;
}

ZD_DEF void *zd_htbl_get(struct zd_hash_tbl *htbl, void *key)
{
    size_t idx = htbl->hash_func(key) % htbl->capacity;
    struct zd_hash_node *cur = htbl->buckets[idx]->next;

    while (cur) {
        if (htbl->key_cmp(cur->key, key))
            return cur->val;
        cur = cur->next;
    }
    return NULL;
}

ZD_DEF void zd_htbl_set(struct zd_hash_tbl *htbl, void *key, void *val)
{
    size_t idx = htbl->hash_func(key) % htbl->capacity;
    struct zd_hash_node *cur = htbl->buckets[idx]->next;

    while (cur) {
        if (htbl->key_cmp(cur->key, key)) {
            if (htbl->val_free)
                htbl->val_free(cur->val);
            memcpy(cur->val, val, htbl->val_size);
            break;
        }
        cur = cur->next;
    }
}

static inline size_t DJB_hash(const char *key, size_t key_len)
{
    size_t hash = 5381;
    for (size_t i = 0; i < key_len; i++) {
        hash = ((hash << 5) + hash) + key[i];
    }
    return hash;
}

static inline size_t SDBM_hash(const char *key, size_t key_len)
{
    size_t hash = 0;
    for (size_t i = 0; i < key_len; i++) {
        hash = key[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

static inline size_t int_hash(int key)
{
    size_t hash = (size_t) key;
    hash = (hash + 0x7ed55d16) + (hash << 12);
    hash = (hash ^ 0xc761c23c) ^ (hash >> 19);
    hash = (hash + 0x165667b1) + (hash << 5);
    hash = (hash + 0xd3a2646c) ^ (hash << 9);
    hash = (hash + 0xfd7046c5) + (hash << 3);
    hash = (hash ^ 0xb55a4f09) ^ (hash >> 16);
    return hash;
}

static inline size_t float_hash(float key)
{
    size_t *ptr = (size_t *) &key;
    size_t hash = *ptr;
    hash = (hash + 0x7ed55d16) + (hash << 12);
    hash = (hash ^ 0xc761c23c) ^ (hash >> 19);
    hash = (hash + 0x165667b1) + (hash << 5);
    hash = (hash + 0xd3a2646c) ^ (hash << 9);
    hash = (hash + 0xfd7046c5) + (hash << 3);
    hash = (hash ^ 0xb55a4f09) ^ (hash >> 16);
    return hash;
}

#endif /* ZD_DS_HASH */

#ifdef ZD_WILDCARD

ZD_DEF bool zd_wildcard_match(const char *str, const char *pattern)
{
    if (!str || !pattern)
        return false;

    int m = strlen(str);
    int n = strlen(pattern);

    bool dp[m + 1][n + 1];
    
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++)
            dp[i][j] = false;
    }
    dp[0][0] = true;

    for (int j = 1; j <= n; j++) {
        if (pattern[j - 1] == '*') {
            dp[0][j] = dp[0][j - 1];
        }
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (pattern[j - 1] == '*')
                dp[i][j] = dp[i][j - 1] || dp[i - 1][j];
            else if (pattern[j - 1] == '?' || str[i - 1] == pattern[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
        }
    }

    return dp[m][n];
}

#endif /* ZD_WILDCARD */

#endif /* ZD_IMPLEMENTATION */
