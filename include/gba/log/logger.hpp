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

namespace gba {

    namespace log_detail {

        struct interface {
            virtual ~interface() noexcept {}
            virtual void output(int level, const char* message) noexcept = 0;
        };

        struct noop : interface {
            static std::unique_ptr<interface> try_create() noexcept {
                return nullptr;
            }

            void output(int, const char*) noexcept override {}
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
                FLAGS = u8(level - 1) | 0x100;
            }
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

            for (auto ii = 0u; ii < interfaces.size(); ++ii) {
                interface = std::invoke(interfaces[ii]);
                if (interface) {
                    return true;
                }
            }
            return false;
        }

        static void output(int level, const char* message) noexcept {
            interface->output(level, message);
        }
    };

} // namespace gba

#endif // define GBAXX_LOG_LOGGER_HPP
