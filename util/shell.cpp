#if 0

state list:

command table:
	[cmd] [help] [processor] [next_state]

options list:
	[option] [help] [optarg name] [optarg type] [flags]

optarg types:
	{u,}integer
	string (quoted)
	enumeration
	float

optarg flags:
	multiple (sets array)

Processor:
	int (proc *)(argc, argv)
		- argc - number of arguments in array (at least 1)
		- argv - argument vector:
			- vec 1 - "*command*" (array)
			- rest: args, indexed by optarg. Options that can have multiple instances
			  have an array as the second parameter
#endif

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#include <sys/kernel.h>
#include <util/shell.h>

#define ESC 27

using namespace chaos::shell;

namespace chaos {
	namespace shell {
enum sh_state {
	SH_PREINIT=0,
	SH_INIT,
	SH_ESCAPE,
	SH_ESCBR
};

static char parse_escape(char ch, struct shell_state *state)
{
	switch (state->sh_parse_state) {
		default:
			break;
		case SH_INIT:
			if (ch == ESC) {
				state->sh_parse_state = SH_ESCAPE;
				ch = 0;
			}
			break;
		case SH_ESCAPE:
			switch (ch) {
				default:
					state->sh_parse_state = SH_INIT;
					ch = 0;
					break;
				case '[':
					state->sh_parse_state = SH_ESCBR;
					ch = 0;
					break;
			}
			break;
		case SH_ESCBR:
			switch (ch) {
				default:
					state->sh_parse_state = SH_INIT;
					ch = 0;
					break;
				case 0 ... 9:
					break;
				case 'A':
					state->sh_parse_state = SH_INIT;
					ch = 0;
					break;
				case 'B':
					state->sh_parse_state = SH_INIT;
					ch = 0;
					break;
				case 'C':
					state->sh_parse_state = SH_INIT;
					ch = 0;
					break;
				case 'D':
					state->sh_parse_state = SH_INIT;
					ch = 0;
					break;
			}
			break;
	}
	return (ch);
}

extern "C" {
	union sh_cmd_family_un<void> __sh_commands_start;
}

static int
sh_print_help(void)
{
	puts("Available commands:\r");
	for (const union sh_cmd_family_un<void> *commands = &__sh_commands_start; commands->name != NULL; commands++)
		if (commands->name[0] != '\0') {
			iprintf("%s", commands->family.name);
			if (commands->type == SH_FAMILY ||
			    (commands->type == SH_COMMAND && commands->cmd.num_args > 0))
				iprintf("...");
			iprintf("\n\r");
		}
	return (0);
}

static int
sh_print_help2(int x, int y)
{
	iprintf("x: %d, y: %d\n\r", x, y);
	return 0;
}

static int
sh_max_command_args()
{
	// TODO: make this determine either at compile or runtime
	return 10;
}

static void
sh_run_command(int argc, const char **argv)
{
	const union sh_cmd_family_un<void> *cmdp = &__sh_commands_start;
	int num_args = 0;

	// Insurance
	if (argc == 0 || argv[0] == NULL)
		return;

	for (int i = 0; i < argc; i++) {
		while (cmdp->name != NULL) {
			if (strcmp(cmdp->name, argv[i]) == 0) {
				if (cmdp->type == SH_COMMAND) {
					// Start parsing arguments
					i++;
					for (int j = 0; (j + i) < argc; j++) {
						num_args++;
					}
					cmdp->cmd.func(&cmdp->cmd, num_args, argv + i);
					return;
				} else if (cmdp->type == SH_FAMILY) {
					cmdp = cmdp->family.cmd_start;
					break;
				}
			}
			cmdp++;
		}
		if (cmdp->name == NULL)
			break;
	}

	if (cmdp->name == NULL) {
		iprintf("Invalid command name: %s\n\r", argv[0]);
		return;
	}
}

int
get_line(const char *prompt, char *buf, int len, struct shell_state *state)
{
	int used = 0;
	char c;

	setvbuf(stdin, nullptr, _IONBF, 0);
	setvbuf(stdout, nullptr, _IONBF, 0);
	fwrite(prompt, 1, strlen(prompt), stdout);
	memset(buf, 0, len);
	state->sh_parse_state = SH_INIT;
	while (c != '\n' && c != '\r') {
		c = getc(stdin);
		c = parse_escape(c, state);

		if (c == 0)
			continue;
		if (c == '\n' || c == '\r') {
			if (state->sh_histlen > 0) {
				memcpy(state->sh_history[state->sh_curhist], buf,
			        MIN(used, SHELL_CMD_LINE+1));
			    state->sh_curhist = (state->sh_curhist + 1) % state->sh_histlen;
			}
			break;
		} else {
			switch (c) {
				default:
					break;
				case '\t':
					// TODO: Tab completion
					c = 0;
					break;
				case '\b':
					// TODO: backspace
					c = 0;
					break;
			}
			if (c == 0)
				continue;
			if (used == len) {
				putc('\a', stdout);
				continue;
			}
			fwrite(&c, 1, 1, stdout);
			buf[used++] = c;
		}
	}

	fwrite("\n\r", 1, 2, stdout);

	return used;
}

int shell_init(struct shell_state *parser)
{
	memset(parser, 0, sizeof(*parser));
	parser->sh_parse_state = SH_INIT;
	return (0);
}

int shell(const char *prompt)
{
	struct shell_state parser;
	char buf[120];
	char *pbuf;
	char tok[2] = {0};
	const char *parsed_args[sh_max_command_args()];
	int argm = sh_max_command_args();
	int argc;

	while (1) {
		chaos::shell::get_line(prompt, buf, 119, &parser);
		if (buf[0] == '\0')
			continue;

		pbuf = buf;
		tok[0] = ' ';
		for (argc = 0; argc < argm; argc++) {
			parsed_args[argc] = strsep(&pbuf, tok);
			if (pbuf[0] == '"' || pbuf[0] == '\'') {
				tok[0] = pbuf[0];
				pbuf++;
			} else
				tok[0] = ' ';
			if (pbuf[0] == '\0') {
				argc--;
				continue;
			}
			if (parsed_args[argc] == NULL)
				break;
		}
		if (argc > 0) {
			sh_run_command(argc, parsed_args);
		}
	}
}

char *shell_tokenize(char **line, char *septok)
{
	return nullptr;
}
}
}

const struct sh_cmd_family<void*> __sh_commands_end __attribute__((section(".commands_end"))) = { .type = SH_FAMILY, .name = nullptr };
CMD_FAMILY();
CMD(,help,chaos::shell::sh_print_help);
CMD(,help2,chaos::shell::sh_print_help2);
