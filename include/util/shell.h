/*
 * Copyright (c) 2015	Justin Hibbits
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef SHELL_CMD_LINE
#define SHELL_CMD_LINE	100
#endif

#ifndef SHELL_MAX_HISTORY
#define SHELL_MAX_HISTORY	10
#endif

namespace chaos {
namespace shell {

struct shell_state {
	int sh_parse_state;
	int sh_histlen;
	int sh_curhist;
	char sh_history[SHELL_MAX_HISTORY][SHELL_CMD_LINE+1];
};

int shell_init();
int get_line(const char *prompt, char *buf, int len, struct shell_state *state);
int shell(const char *prompt);

typedef char *string;

enum {
	SH_COMMAND,
	SH_FAMILY,
};

template <typename T>
struct sh_cmd {
	int type;
	const char *name;
	int (*func)(const sh_cmd<T> *, int, const char * const*);
	T *real_func;
	int num_args;
};

template <typename T> union sh_cmd_family_un;

template <typename T>
struct sh_cmd_family {
	int type;
	const char *name;
	const union sh_cmd_family_un<T> *cmd_start;
};

template <typename T>
union sh_cmd_family_un {
	struct {
		int type;
		const char *name;
	};
	struct sh_cmd_family<T> family;
	struct sh_cmd<T> cmd;
};

int sh_print_help(const char *, const union sh_cmd_family_un<void> *);

namespace {
	template <typename T>
	T extract_arg(const char *arg);

	template <>
	int extract_arg(const char *arg) {
		// All error checking done in the validator.
		return strtol(arg, nullptr, 0);
	}

	template <>
	const char *extract_arg(const char *arg) { return arg; }

	template <>
	char extract_arg(const char *arg) { return *arg; }

	template <typename T>
	bool validate_cmd_arg(const char *arg);

	template <>
	bool validate_cmd_arg<int>(const char *arg) {
		char *end;
		strtol(arg, &end, 0);
		return (end != arg && *end == '\0' && errno == 0) ? true : false;
	}

	template <>
	bool validate_cmd_arg<const char *>(const char *arg) { return true; }

	template <>
	bool validate_cmd_arg<char>(const char *arg)
	{ return (*arg != '\0') ? -1 : 0; }

	template <typename... Args>
	constexpr int count_args(int (*func)(Args...))
	{ return sizeof...(Args); }

	template<int>
	int
	validate_args(const char * const *args)
	{
		return (-1);
	}

	template <>
	int
	validate_args<0>(const char * const *args)
	{
		return -1;
	}
	template<int i, typename T, typename ...U> int
	validate_args(const char * const *args)
	{
		return validate_cmd_arg<T>(*args) ?
		    validate_args<i - 1, U...>(args + 1) :
		    i;
	}

	/*
	 * Templates for running commands.  Currently handle up to 3 arguments,
	 * but it's trivial to add more.  It would be nice to auto-generate
	 * this, but we can't.
	 */
	template <int N, typename ...T> struct cmd_runner;

	template <typename T, typename U, typename V>
	struct cmd_runner<3, T, U, V> {
		static int run(int (*f)(T, U, V), const char * const *argv)
		{
			return f(extract_arg<T>(argv[0]),
				extract_arg<U>(argv[1]),
				extract_arg<V>(argv[2]));
		}
	};

	template <typename T, typename U>
	struct cmd_runner<2, T, U> {
		static int run(int (*f)(T, U), const char * const *argv)
		{
			return f(extract_arg<T>(argv[0]),
				extract_arg<U>(argv[1]));
		}
	};

	template <typename T>
	struct cmd_runner<1, T> {
		static int run(int (*f)(T), const char * const *argv)
		{
			return f(extract_arg<T>(argv[0]));
		}
	};
	template <>
	struct cmd_runner<0> {
		static int run(int (*f)(), const char * const *argv)
		{
			return f();
		}
	};
}

/*
 * Convert this into a generator (constexpr)....
 *
 * template <typename C, typename... Args>
 * gen_shell_cmd(C (*func)(Args...)) -> int (*)()...
 * {
 * 	return a do_shell_command() with the given argument handler.  Parse function
 * 	arguments to generate reader wrappers.
 * }
 */

template<typename... Args>
int
do_shell_command(const sh_cmd<int(Args...)> *cmd, int argc, const char * const * argv)
{
	int failed = validate_args<sizeof...(Args), Args...>(argv);

	if (argc != sizeof...(Args)) {
		iprintf("Insufficient arguments for %s\n\r", cmd->name);
		iprintf("Expected %d, got %d\n\r", sizeof...(Args), argc);
		return -1;
	}
	if (failed != -1) {
		iprintf("Invalid argument %d '%s'.\n\r", failed,
		    argv[failed - 1]);
		return -1;
	}
	return cmd_runner<sizeof...(Args), Args...>::run(cmd->real_func, argv);
}

template <typename ... Args>
constexpr decltype(do_shell_command<Args...>)*
gen_shell_cmd(int (*func)(Args ...))
{
	return do_shell_command<Args...>;
}

/*
A little about the following macros:

CMD_FAMILY() defines a command family (top-level command).  At some point, I
 plan to have nested families, but for now that's not the case.

We use a number of custom section names:

.commands	- Top level command structure.
.command.<command>.A	- Starting section (linker will place immediately before all commands in a family)
.command.<command>.a.<subcommand>	- Each command is in here.  Alphabetic order
.command.<command>.z				- Command terminator.

*/

#define _CMD_FAMILY(fname) \
	const union chaos::shell::sh_cmd_family_un<void> \
	    __CONCAT(__sh_,__CONCAT(fname,__start)) \
	    __section(".command."#fname".A") __used = \
		{.cmd = { .type = chaos::shell::SH_FAMILY, .name = "" }}; \
	const union chaos::shell::sh_cmd_family_un<void> \
	    __CONCAT(__sh_,__CONCAT(fname,__end)) \
	    __section(".command."#fname".z") __used = \
		{.cmd = { .type = chaos::shell::SH_FAMILY, .name = nullptr }}; \
	const union chaos::shell::sh_cmd_family_un<void> \
	    __CONCAT(__sh_family_,fname) __section(".commands") __used = \
		{ .family = { \
		    .type = chaos::shell::SH_FAMILY,\
		    .name = #fname,\
		    .cmd_start = &__CONCAT(__sh_,__CONCAT(fname,__start))}};
#define CMD(fname,c,f)	\
	const union chaos::shell::sh_cmd_family_un<decltype(f)> \
	    __CONCAT(__sh_,__CONCAT(__CONCAT(fname,_),c)) \
	    __section(".command."#fname".a."#c) __used = \
	        {.cmd = { \
		    .type = chaos::shell::SH_COMMAND, \
		    .name = #c, \
		    .func = chaos::shell::gen_shell_cmd(f), \
		    .real_func = f, \
		    .num_args = chaos::shell::count_args(f)}}

#define	CMD_FAMILY(fname) \
	_CMD_FAMILY(fname); \
	static int __CONCAT(fname,__help)(void) \
		{ return chaos::shell::sh_print_help(#fname, \
		    &__CONCAT(__sh_,__CONCAT(fname,__start))); } \
	CMD(fname, help, __CONCAT(fname, __help))
}
}
