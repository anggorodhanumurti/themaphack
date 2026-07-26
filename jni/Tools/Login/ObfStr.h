#pragma once
/*
 * Compile-time string obfuscation.
 *
 * OBF("literal") embeds the string as ciphertext (XOR with a per-string key
 * stream derived at compile time) and decrypts it into a std::string on the
 * stack at runtime. The plaintext never appears in the binary, so
 * `strings libAkSoundEngine2.so` will not reveal wrapped strings (endpoint
 * URL, auth pepper, detection needles, ...).
 *
 * C++17, header-only, no external deps.
 */

#include <cstddef>
#include <string>

namespace obf {

// FNV-1a hash of a compile-time C-string (used to derive a per-file seed).
constexpr unsigned int fnv1a(const char *s) {
    unsigned int h = 2166136261u;
    for (std::size_t i = 0; s[i] != '\0'; ++i)
        h = (h ^ static_cast<unsigned char>(s[i])) * 16777619u;
    return h;
}

// xorshift-based key stream: one pseudo-random byte per character index.
constexpr unsigned char keyByte(unsigned int seed, std::size_t i) {
    unsigned int x = seed + static_cast<unsigned int>(i) * 2654435761u + 0x9E3779B9u;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return static_cast<unsigned char>(x & 0xFFu);
}

template <std::size_t N, unsigned int SEED>
class Encrypted {
public:
    constexpr explicit Encrypted(const char (&in)[N]) : buf_{} {
        for (std::size_t i = 0; i < N; ++i)
            buf_[i] = static_cast<char>(static_cast<unsigned char>(in[i]) ^ keyByte(SEED, i));
    }

    // Decrypt at runtime; the plaintext lives only in the returned std::string.
    std::string decrypt() const {
        char tmp[N];
        for (std::size_t i = 0; i < N; ++i)
            tmp[i] = static_cast<char>(static_cast<unsigned char>(buf_[i]) ^ keyByte(SEED, i));
        std::string out(tmp, N - 1);
        for (std::size_t i = 0; i < N; ++i) tmp[i] = 0; // wipe stack copy
        return out;
    }

private:
    char buf_[N];
};

} // namespace obf

// Produces a std::string; the literal is stored encrypted in .rodata.
#define OBF(str)                                                                \
    ([]() -> std::string {                                                      \
        constexpr unsigned int _obf_seed =                                      \
            ::obf::fnv1a(__FILE__) ^                                            \
            (static_cast<unsigned int>(__LINE__) * 0x01000193u) ^              \
            (static_cast<unsigned int>(__COUNTER__) + 1u);                      \
        constexpr ::obf::Encrypted<sizeof(str), _obf_seed> _obf_val(str);       \
        return _obf_val.decrypt();                                              \
    }())
