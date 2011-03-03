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

#ifndef	__BPF_COMPILER_H__
#define	__BPF_COMPILER_H__

#include <stdint.h>
#include <sys/queue.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define	stringify(x)	#x

enum bpf_arith_ops
{
	EQUAL = 1,
	NOT_EQUAL,
	GREATER,
	LESS,
	GREATER_EQUAL,
	LESS_EQUAL
};

enum bpf_bit_ops
{
	NOT = 1,
	AND,
	OR,
	XOR
};

enum bpf_compiler_obj
{
	UNKNOWN = 0,
	SRC,
	DST,
	IP,
	IP6,
	MAC,
	LEN,
	PORT,
	BIT_OP,
	MAC_ID,
	POS_NUMBER,
	IPv4_ID
};

struct bpf_step
{
	union value
	{
		uint64_t nr;
		struct in_addr in;
		struct in6_addr in6;
		struct ether_addr eth;
		enum bpf_bit_ops bit_op;
	} value;

	enum bpf_arith_ops arith_op;
	enum bpf_compiler_obj obj;
	size_t nr;
	TAILQ_ENTRY(bpf_step) entry;
};

struct bpf_expr
{
	size_t					len;
	TAILQ_HEAD(bpf_expr_head, bpf_step)	head;
};

void lex_init(const char * const buf);
void lex_cleanup();

int bpf_expr_parse(struct bpf_expr * expr);
int bpf_strtoull(const char * const str, uint64_t * val);

void bpf_expr_init(struct bpf_expr * expr);
int bpf_step_add_arith_op(struct bpf_expr * expr, const enum bpf_arith_ops op);
int bpf_step_add_bit_op(struct bpf_expr * expr, const enum bpf_bit_ops op);
int bpf_step_add_obj(struct bpf_expr * expr, const enum bpf_compiler_obj obj);
int bpf_step_add_number(struct bpf_expr * expr, const uint64_t nr);
int bpf_step_add_eth(struct bpf_expr * expr, const struct ether_addr eth);
int bpf_step_add_in(struct bpf_expr * expr, const struct in_addr in);
int bpf_step_add_in6(struct bpf_expr * expr, const struct in6_addr in6);

#endif	/* __BPF_COMPILER_H__ */
