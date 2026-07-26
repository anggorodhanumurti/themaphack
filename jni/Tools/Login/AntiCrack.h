#pragma once
/*
 * Lightweight anti-debug / anti-tamper checks for the key-system client.
 *
 * These are intentionally non-destructive: on detection they simply report
 * "compromised" so the caller can fail the license check, rather than crashing
 * the host process. All telltale strings are wrapped with OBF() so they do not
 * appear in `strings` output.
 */

#include <string>
#include <fstream>
#include "ObfStr.h"

namespace anticrack {

// A debugger (or another tracer/injector) is attached if TracerPid != 0.
inline bool debuggerAttached() {
    std::ifstream status("/proc/self/status");
    if (!status.is_open()) return false;
    const std::string needle = OBF("TracerPid:");
    std::string line;
    while (std::getline(status, line)) {
        if (line.rfind(needle, 0) == 0) {
            for (std::size_t i = needle.size(); i < line.size(); ++i) {
                char c = line[i];
                if (c >= '1' && c <= '9') return true;
                if (c != ' ' && c != '\t' && c != '0') break;
            }
            return false;
        }
    }
    return false;
}

// Frida / dynamic-instrumentation frameworks leave recognizable regions in
// the process memory map.
inline bool instrumentationPresent() {
    std::string needles[] = {
        OBF("frida"), OBF("gum-js-loop"), OBF("gmain"),
        OBF("linjector"), OBF("gadget"), OBF("frida-agent"),
    };
    std::ifstream maps("/proc/self/maps");
    if (!maps.is_open()) return false;
    std::string line;
    while (std::getline(maps, line)) {
        for (const auto &n : needles) {
            if (line.find(n) != std::string::npos) return true;
        }
    }
    return false;
}

// Aggregate verdict used to gate the license check.
inline bool isCompromised() {
    return debuggerAttached() || instrumentationPresent();
}

} // namespace anticrack
