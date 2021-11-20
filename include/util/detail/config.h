/*
 * Copyright (c) 2021	Justin Hibbits
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

/**
 * \file
 *
 * \internal No user servicable parts.
 */
#include <algorithm>
#include <string.h>
#include <misc/crc32.h>
#include <util/config.h>

namespace chaos {
namespace util {

namespace _detail {
static size_t
get_skip(size_t size)
{
	size_t r = 1 << (ffs(size) - 1);
	if (((r - 1) & size) != 0)
		r <<= 1;
	return (r);
}
}

/*
 * Theory of operations:
 *
 * - Configuration blobs are stored sequentially in memory, power-of-two
 *   alignment for simplicity.
 *
 * - A change in format starts on a new block.
 * - Sequence numbers are block-based, not config based.  The last valid config
 *   in a block will always be the most recent of the block.  When saving a new
 *   config, if it goes onto a new block the sequence number gets updated,
 *   otherwise it remains.
 */

/**
 * \brief Load the most current config blob from flash memory.
 *
 * To load, first walk the blocks, looking at the first blob only, until we get
 * to a block with the right format.  If there is none, try to load older
 * formats until format 1.
 *
 * The config store is default constructed prior.
 */
template <typename T, typename BE, uintptr_t B, int c>
void
flash_config<T,BE,B,c>::load()
{
	using config_header = typename config_store::config_header;
	config_header *block_ptr;

	find_config();

	// Bail out early if sentinel indicates flash is empty
	if (flash_store.cooked->header.sentinel == 0xffffffff)
		return;

	block_ptr = &flash_store.cooked->header;
	if (block_ptr->format != conf.header.format) {
		// Need to be explicit with template type because it can't be
		// inferred.
		upgrade<config_store>(block_ptr);
	} else {
		memcpy(&conf, block_ptr,
		    std::min((size_t)block_ptr->size + sizeof(*block_ptr), sizeof(conf)));
	}
}

/*
 * Theory:
 * If the current save pointer is to an old format, choose the next erase block
 * to start the new format.
 */
template <typename T, typename BE, uintptr_t B, int C>
void
flash_config<T,BE,B,C>::save()
{
	// Some sanity checking.  If CRC fails it's most likely blank.
	if (flash_store.indexed == (uintptr_t)-1) {
		find_config();
	}

	config_store *flashed = flash_store.cooked;

	if (flashed->header.sentinel == SENTINEL &&
	    flashed->header.crc ==
	    crc32(&flash_store.cooked->body, flashed->header.size)) {
		flash_store.indexed += _detail::get_skip(flashed->header.size +
		    sizeof(flashed->header));
		if (flash_store.indexed > base + erase_size * block_count)
			flash_store.indexed = (uintptr_t)base;
		if ((flash_store.indexed - (uintptr_t)base) % erase_size == 0) {
			flash_erase(flash_store.raw);
			conf.header.seq = flashed->header.seq + 1;
		}
	}
	conf.header.size = sizeof(conf.body);
	conf.header.crc = crc32(&conf.body, sizeof(conf.body));
	flash_write(flash_store.raw, &conf, sizeof(conf));
}

// Locate the most recent config.
template <typename T, typename BE, uintptr_t B, int C>
void
flash_config<T,BE,B,C>::find_config()
{
	using config_header = typename config_store::config_header;

	auto next = [](config_header *h) {
		return _detail::get_skip(h->size + sizeof(config_header));
	};

	union searcher {
		config_header h;
		uint8_t	block[erase_size];
	} *search_header, *end;

	search_header = reinterpret_cast<searcher *>(base);
	end = reinterpret_cast<searcher *>(base + psize);

	while (search_header != end && search_header->h.sentinel != SENTINEL)
		search_header++;

	if (search_header == end) {
		// flash is blank, go back to the beginning.
		flash_store.indexed = base;
		return;
	}

	// Find the "latest" block in flash.
	while (&search_header[1] != end &&
	    search_header[1].h.sentinel == SENTINEL &&
	    search_header[1].h.seq > search_header->h.seq)
	    	search_header++;

	// Found the block we want to search now.
	union {
		uintptr_t index;
		config_header *h;
	} block_ptr;
	block_ptr.h = &search_header->h;
	uintptr_t block_base = block_ptr.index;
	uintptr_t block_end = block_base + erase_size;
	do {
		while (block_ptr.h->crc != crc32(block_ptr.h + 1, block_ptr.h->size)) {
			block_ptr.index += next(block_ptr.h);
		}
		config_header *cur = block_ptr.h;
		block_ptr.index += next(cur);

		if (block_ptr.h->sentinel != SENTINEL) {
		    	block_ptr.h = cur;
			break;
		}
		if (block_ptr.h->crc != crc32(block_ptr.h + 1, block_ptr.h->size)) {
		}
	} while (block_ptr.index + next(block_ptr.h) != block_end);
	flash_store.raw = block_ptr.h;
}
}
}
