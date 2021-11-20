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
 * \brief ChaOS runtime configuration system
 *
 * An abstract interface for creating runtime configurations that can be saved
 * and loaded at startup.  Configurations have two version identifiers: Format
 * version, and configuration version.  The format version is used by binary
 * format, particularly the Flash memory backend, to offer backward
 * compatibility with older formats, and to make sure to load the right version,
 * as size and format will be affected.  The configuration version is a sequence
 * number set at store time, so the most recent version can be loaded in a
 * multi-page storage system, like Flash memory.
 *
 * All types have a data checksum, and the Flash backend includes the size in
 * the format as well for easily skipping through older versions.
 *
 * Backends to be implemented:
 * - Flash memory, utilizing two or more pages for redundancy.  This is a binary
 *   format.  To maintain redundancy there must be enough pages to hold at least
 *   two full copies of the configuration on different pages.
 * - File storage backend.  This can be binary or text.  It utilizes two or more
 *   files for redundancy.
 */

#ifndef _UTIL_CONFIG_H
#define	_UTIL_CONFIG_H

#include <sys/types.h>
#include <algorithm>
#include <type_traits>

/**
 * \namespace chaos
 */
namespace chaos {
	namespace util {
		// Abstract base class.
		class config {
		protected:
			template <typename T>
			class item {
			};
		public:
			virtual void load(void) = 0;
			virtual void save(void) = 0;
			virtual void reset(void) = 0;
			static config *global_config;
		};

		/**
		 * Flash memory backend.
		 *
		 * \brief Uses a region of Flash memory, directly addressable,
		 * as the backing storage.
		 *
		 * Template parameters:
		 * \tparam T Type of the configuration block.  Should be a class
		 * or struct.
		 *
		 * \tparam BE Backend class.  Must implement the methods:
		 * \c flash_load , \c flash_write , and \c flash_erase
		 *
		 * \tparam B Base address of the config parameter region
		 *
		 * \tparam EB Erase block size
		 *
		 * \tparam c Number of erase blocks (sectors) to use.
		 *
		 * The \T type should have one of the following static members:
		 * \li \c consistent static member to denote new formats only
		 * add to the end of the older format.
		 * \li \c upgrade() static method to upgrade older format to
		 * new.
		 */
		template <typename T, typename BE, uintptr_t B, int C>
		class flash_config : public config {
			static constexpr uintptr_t base = B;
			static constexpr size_t erase_size = BE::erase_size;
			static constexpr int block_count = C;
			static constexpr size_t psize = block_count * erase_size;
			typedef BE backend;
			static constexpr uint16_t SENTINEL = 0xc0de;

			static_assert(C > 1,
			    "Flash config needs two or more erase blocks");
			static_assert(C < BE::block_count,
			    "Using too many blocks for flash backend");
			static_assert(base + psize - 1 <= backend::flash_end,
			    "Config region is outside flash backend region.");

			struct config_store {
				struct config_header {
					uint16_t sentinel = SENTINEL;
					uint16_t format = T::format;
					uint32_t seq;
					uint32_t size = sizeof(T);
					uint32_t crc;
				} header __packed;
				T body;
			} conf __packed;

			static_assert(sizeof(conf) < erase_size,
			    "Config size is too large");
			union {
				void *raw;
				uintptr_t indexed = -1;
				config_store *cooked;
			} flash_store;
			static constexpr size_t body_off =
			    offsetof(config_store, body);
			static constexpr size_t data_size =
			    howmany(sizeof(conf), sizeof(uintptr_t)) *
			    sizeof(uintptr_t); 
			template <typename U> struct has_consistent_property;
			template <typename U> struct has_upgrade_method;
			template <typename U> typename
			    std::enable_if_t<has_upgrade_method<decltype(U::body)>::value>
			upgrade(typename config_store::config_header *h)
			{
				//U &c2 = conf;
				conf.body.upgrade(h + 1, h->format);
			}
			template <typename U> typename
			    std::enable_if_t<has_consistent_property<decltype(U::body)>::value>
			upgrade(typename config_store::config_header *input)
			{
				U &temp = conf;
				memcpy(&temp, input,
				    std::min(static_cast<size_t>(input->size), sizeof(conf)));
			}

			/**
			 * Default upgrade method.  Does nothing.
			 *
			 * This default requires that the format version be 1 or
			 * lower.
			 */
			template <typename U> typename
			    std::enable_if_t<!has_consistent_property<decltype(U::body)>::value && !has_upgrade_method<decltype(U::body)>::value>
			upgrade(typename config_store::config_header *input)
			{
				// This intentionally left blank.
				static_assert(T::format <= 1);
			}

		protected:
			void flash_erase(void *erase_base) {
				backend::erase((uintptr_t)erase_base);
			}
			void flash_write(void *write_base,
				void *src, size_t size) {
				backend::write((uintptr_t)write_base,
				    (uintptr_t)src, size);
			}

			void find_config();
		public:
			flash_config() : config()
			{
				config::global_config = this;
			}

			void load(void) override;
			void save(void) override;

			/**
			 * \brief Reset configuration to defaults.
			 *
			 * Reset the configuration to defaults, erase all
			 * saved configuration.
			 */
			void reset(void) override {
				new (&conf) config_store;
				for (auto i = 0; i < block_count; i++)
					backend::erase(base + i * erase_size);
				flash_store.indexed = base;
			}

			T * operator->() {
				return &conf.body;
			}

		private:
			/**
			 * \internal
			 * Support template for enable_if, to enable the config
			 * type's "upgrade" method.
			 */
			template <typename U>
			struct has_upgrade_method {
			private:
				template<typename V> static auto test(int) ->
					decltype(std::declval<V>().upgrade(0),
							std::true_type());
				template<typename> static std::false_type
					test(...);
			public:
				static constexpr bool value =
				    std::is_same<decltype(test<U>(0)),
					std::true_type>::value;
			};
			/**
			 * \internal
			 *
			 * Support template for enable_if, to check if the
			 * config structure has a "consistent_format" property
			 * (static constant).  This denotes that new members are
			 * only tacked onto the end, and format doesn't change.
			 */
			template <typename U>
			struct has_consistent_property {
			private:
				template<typename V, typename A = int>
					struct has_consistent :
						std::false_type {};
				template <typename V>
					struct has_consistent <V,
					    decltype((void) V::consistent_format,
					    0)> : std::true_type {};
			public:
				static constexpr bool value =
				    std::is_base_of<std::true_type,
				    typename has_consistent<U>::type>::value;
			};
		};
	}
}

#include <util/detail/config.h>
#endif
