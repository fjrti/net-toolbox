/* __LICENSE_HEADER_BEGIN__ */

/*
 * Copyright (C) 2009, 2011  Daniel Borkmann <daniel@netsniff-ng.org> and
 *                           Emmanuel Roullit <emmanuel@netsniff-ng.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 *
 */

 /* __LICENSE_HEADER_END__ */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include "bpf_compiler.h"

const char * bpf_arith_ops_to_string(const enum bpf_arith_ops op)
{
	static const char * const bpf_arith_ops_string[] = 
	{
		[EQUAL] = stringify(EQUAL),
		[NOT_EQUAL] = stringify(NOT_EQUAL),
		[LESS] = stringify(LESS),
		[GREATER] = stringify(GREATER),
		[GREATER_EQUAL] = stringify(GREATER_EQUAL),
		[LESS_EQUAL]= stringify(LESS_EQUAL)
	};

	assert(op <= sizeof(bpf_arith_ops_string));

	return (bpf_arith_ops_string[op]);
}

const char * bpf_bit_ops_to_string(const enum bpf_bit_ops op)
{
	static const char * const bpf_bit_ops_string[] = 
	{
		[NOT] = stringify(NOT),
		[AND] = stringify(AND),
		[OR] = stringify(OR),
		[XOR] = stringify(XOR)
	};

	assert(op <= sizeof(bpf_bit_ops_string));

	return (bpf_bit_ops_string[op]);
}

const char * bpf_code_to_string(const enum bpf_compiler_obj obj)
{
	static const char * const bpf_code_string[] = 
	{
		[UNKNOWN] = stringify(UNKNOWN),
		[SRC] = stringify(SRC),
		[DST] = stringify(DST),
		[IP] = stringify(IP),
		[IP6] = stringify(IP6),
		[MAC] = stringify(MAC),
		[PORT] = stringify(PORT),
		[BIT_OP] = stringify(BIT_OP),
		[POS_NUMBER] = stringify(POS_NUMBER),
		[MAC_ID] = stringify(MAC_ID)
	};
	
	assert(obj <= sizeof(bpf_code_string));

	return (bpf_code_string[obj]);
}

int bpf_strtoull(const char * const str, uint64_t * val)
{
	char * endptr = NULL;

	assert(str);
	assert(val);

	errno = 0;

	*val = strtoull(str, &endptr, 0);

	if ((errno == ERANGE && *val == ULLONG_MAX) || (errno != 0 && *val == 0)) 
	{
		return(errno);
	}

	if (endptr == str) 
	{
		return(EINVAL);
	}

	return (0);
}

void bpf_expr_init(struct bpf_expr * expr)
{
	assert(expr);

	memset(expr, 0, sizeof(*expr));

	TAILQ_INIT(&expr->head);
}

void bpf_expr_free(struct bpf_expr * expr)
{
	struct bpf_step * step;

	/* Same behaviour as free(3) */
	if (!expr)
		return;

	while((step = TAILQ_FIRST(&expr->head)) != NULL)
	{
		TAILQ_REMOVE(&expr->head, step, entry);
		free(step);
		step = NULL;
	}

	memset(expr, 0, sizeof(*expr));
}

struct bpf_step * bpf_step_alloc(void)
{
	struct bpf_step * step;

	step = malloc(sizeof(*step));

	if (!step)
		return NULL;

	memset(step, 0, sizeof(*step));

	return step;
}

static int bpf_step_add_value(struct bpf_expr * expr, const union value value)
{
	struct bpf_step * step;

	assert(expr);

	/* 
	 * As a value must go with the previous code
	 * we fetch the last added step and attach the value to it
	 */

	step = TAILQ_LAST(&expr->head, bpf_expr_head);

	assert(step);

	switch(step->obj)
	{
		case PORT:
		case LEN:
			step->value.nr = value.nr;
		break;

		case IP:
			step->value.in = value.in;
		break;

		case IP6:
			step->value.in6 = value.in6;
		break;

		case MAC:
			step->value.eth = value.eth;
		break;

		case BIT_OP:
			step->value.bit_op = value.bit_op;
		break;

		default:
			return EINVAL;
		break;
	}

	return 0;
}

int bpf_step_add_arith_op(struct bpf_expr * expr, const enum bpf_arith_ops op)
{
	struct bpf_step * step;

	assert(expr);

	/* 
	 * As a value must go with the previous code
	 * we fetch the last added step and attach the value to it
	 */

	step = TAILQ_LAST(&expr->head, bpf_expr_head);

	assert(step);

	step->arith_op = op;

	return 0;
}

int bpf_step_add_bit_op(struct bpf_expr * expr, const enum bpf_bit_ops op)
{
	struct bpf_step * step;

	assert(expr);

	bpf_step_add_obj(expr, BIT_OP);

	step = TAILQ_LAST(&expr->head, bpf_expr_head);

	assert(step);

	step->value.bit_op = op;

	return 0;
}

int bpf_step_add_obj(struct bpf_expr * expr, const enum bpf_compiler_obj obj)
{
	struct bpf_step * step;
 
        assert(expr);
 
	step = bpf_step_alloc();

	if (!step)
		return ENOMEM;

	step->obj = obj;
	step->arith_op = EQUAL;
	step->nr = expr->len;

	TAILQ_INSERT_TAIL(&expr->head, step, entry);
	expr->len++;

	return 0;
}

int bpf_step_add_number(struct bpf_expr * expr, const uint64_t nr)
{
	union value value = {0};

	value.nr = nr;

	return bpf_step_add_value(expr, value);
}

int bpf_step_add_eth(struct bpf_expr * expr, const struct ether_addr eth)
{
	union value value = {0};

	value.eth = eth;

	return bpf_step_add_value(expr, value);
}

int bpf_step_add_in(struct bpf_expr * expr, const struct in_addr in)
{
	union value value = {0};

	value.in = in;

	return bpf_step_add_value(expr, value);
}

int bpf_step_add_in6(struct bpf_expr * expr, const struct in6_addr in6)
{
	union value value = {0};

	value.in6 = in6;

	return bpf_step_add_value(expr, value);
}

int bpf_print_expr(const struct bpf_expr * const expr)
{
	union
	{
		char in_str[INET_ADDRSTRLEN];
		char in6_str[INET6_ADDRSTRLEN];
	} addr;

	struct bpf_step * step;

	assert(expr);

	TAILQ_FOREACH(step, &expr->head, entry)
	{
		printf("%s %s ", bpf_code_to_string(step->obj), bpf_arith_ops_to_string(step->arith_op));

		switch(step->obj)
		{
			case IP:
				inet_ntop(AF_INET, &step->value.in, addr.in_str, sizeof(addr.in_str));
				printf("%s\n", addr.in_str);
			break;

			case IP6:
				inet_ntop(AF_INET6, &step->value.in6, addr.in6_str, sizeof(addr.in6_str));
				printf("%s\n", addr.in6_str);
			break;
	
			case MAC:
				printf("%s\n", ether_ntoa(&step->value.eth));
			break;
	
			case LEN:
				printf("%"PRIu64"\n", step->value.nr);
			break;
	
			case PORT:
				printf("%"PRIu64"\n", step->value.nr);
			break;

			case BIT_OP:
				printf("%s\n", bpf_bit_ops_to_string(step->value.bit_op));
			break;
			
			default:
				return EINVAL;
			break;
		}
	}

	return 0;
}

int main (int argc, char ** argv)
{
	struct bpf_expr expr;

	if (argc != 2)
	{
		printf("Please write your BPF expression into double quotes\n");
		return (EXIT_FAILURE);
	}

	lex_init((argc == 2 && argv[1]) ? argv[1] : "");

	bpf_expr_parse(&expr);

	bpf_print_expr(&expr);

	bpf_expr_free(&expr);

	lex_cleanup();

	return (0);
}
