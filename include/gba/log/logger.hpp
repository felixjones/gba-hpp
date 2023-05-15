/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_LOG_LOGGER_HPP
#define GBAXX_LOG_LOGGER_HPP

#include <functional>
#include <memory>

#include <gba/type.hpp>

#if defined __has_include
#if __has_include(<posprintf.h>)
extern "C" {
#include <posprintf.h>
}

#include <cstdarg>
#endif
#endif

namespace gba {

    namespace log_detail {

        struct interface {
            virtual ~interface() noexcept = default;
            virtual void output(int level, const char* message) noexcept = 0;
#ifdef _PSPRINTF_HEADER_
            virtual void posprintf(int level, const char* fmt, std::size_t argn, ...) noexcept = 0;
#endif
        };

        struct noop : interface {
            static std::unique_ptr<interface> try_create() noexcept {
                return nullptr;
            }

            void output(int, const char*) noexcept override {}

#ifdef _PSPRINTF_HEADER_
            void posprintf(int, const char*, std::size_t, ...) noexcept override {}
#endif
        };

        struct mgba : interface {
            static constexpr auto FLAGS = registral<const_ptr<volatile u16>(0x04FFF700)>{};
            static constexpr auto ENABLE = registral<const_ptr<volatile u16>(0x04FFF780)>{};
            static constexpr auto LOG_OUT = const_ptr<volatile char[256]>{0x04FFF600};

            static std::unique_ptr<interface> try_create() noexcept {
                if ((ENABLE = 0xc0de) == 0x1dea) {
                    return std::make_unique<mgba>();
                }
                return nullptr;
            }

            void output(int level, const char* message) noexcept override {
                for (int ii = 0; message[ii] && ii < 256; ++ii) {
                    LOG_OUT[ii] = message[ii];
                }
                FLAGS = u8(level) | 0x100;
            }

#ifdef _PSPRINTF_HEADER_
            void posprintf(int level, const char* fmt, std::size_t argn, ...) noexcept override {
                // Pointer to register arguments on stack
                auto* stack = static_cast<const void**>(__builtin_apply_args());

                // Pointer to variadic arguments
                void** vargsptr;
                va_list vargs;
                va_start(vargs, argn);
                __builtin_memcpy(&vargsptr, &vargs, sizeof(vargsptr));
                va_end(vargs);

                // Set register arguments on stack
                stack[1] = static_cast<const void*>(&LOG_OUT);
                stack[2] = static_cast<const void*>(fmt);

                if (argn > 2) {
                    __builtin_memcpy(&stack[3], vargsptr, sizeof(int) * 2);
                    // Set variadics arguments on stack
                    __builtin_memcpy(vargsptr, &vargsptr[2], sizeof(int) * (argn - 2));
                } else {
                    __builtin_memcpy(&stack[3], vargsptr, sizeof(int) * argn);
                }

                auto* ret = __builtin_apply(reinterpret_cast<void(*)(...)>(::posprintf), stack, sizeof(int) * argn);

                FLAGS = u8(level) | 0x100;
                __builtin_return(ret);
            }
#endif
        };

    } // namespace log_detail

    struct logger {
    private:
        using interface_type = std::unique_ptr<log_detail::interface>;

        inline static interface_type interface{};
    public:
        static bool init() noexcept {
            static constexpr auto interfaces = std::array<interface_type(*)(void), 2>{
                log_detail::mgba::try_create,

                log_detail::noop::try_create
            };

            for (auto createInterface : interfaces) {
                interface = std::invoke(createInterface);
                if (interface) {
                    return true;
                }
            }
            return false;
        }

        static void output(int level, const char* message) noexcept {
            interface->output(level, message);
        }

#ifdef _PSPRINTF_HEADER_
        [[gnu::always_inline]]
        static void posprintf(int level, const char* fmt, ...) noexcept {
            interface->posprintf(level, fmt, __builtin_va_arg_pack_len(), __builtin_va_arg_pack());
        }
#endif
    };

} // namespace gba

#endif // define GBAXX_LOG_LOGGER_HPP
