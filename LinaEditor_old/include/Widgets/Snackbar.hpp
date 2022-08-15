/*
Class: Snackbar



Timestamp: 12/29/2021 11:56:59 PM
*/

#pragma once

#ifndef Snackbar_HPP
#define Snackbar_HPP

// Headers here.
#include <Data/String.hpp>

namespace Lina
{
    enum class LogLevel;
}
namespace Lina::Editor
{
    class Snackbar
    {

    public:

        /// <summary>
        /// Sends in a snackbar from the side of the window informing the user with the given text & log level.
        /// </summary>
        static void PushSnackbar(LogLevel level, const String& text);

    private:
    };
} // namespace Lina::Editor

#endif
