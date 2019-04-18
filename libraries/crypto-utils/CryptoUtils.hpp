/**
 * @file CryptoUtils.hpp
 * @brief Cryptographic utilities and hash functions
 * @author Lukas Ernst
 * 
 * A collection of cryptographic utilities including MD5 hashing, string obfuscation,
 * and pseudo-random number generation. Designed for security research, data integrity
 * verification, and educational purposes.
 */

#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

// MD5 Hash Implementation
namespace MD5 {
    
    static constexpr size_t DIGEST_LENGTH = 16;
    static constexpr size_t BLOCK_SIZE = 64;

    /**
     * @brief MD5 context structure for incremental hashing
     */
    struct Context {
        uint32_t state[4];          // MD5 state
        uint64_t count;             // Number of bits processed
        uint8_t buffer[BLOCK_SIZE]; // Input buffer
    };

    // MD5 auxiliary functions
    namespace Internal {
        uint32_t F(uint32_t x, uint32_t y, uint32_t z);
        uint32_t G(uint32_t x, uint32_t y, uint32_t z);
        uint32_t H(uint32_t x, uint32_t y, uint32_t z);
        uint32_t I(uint32_t x, uint32_t y, uint32_t z);
        uint32_t RotateLeft(uint32_t value, int shift);

        /**
         * @brief Process a 64-byte block of data
         */
        void ProcessBlock(uint32_t state[4], const uint8_t block[BLOCK_SIZE]);
    }

    /**
     * @brief Initialize MD5 context
     */
    void Initialize(Context& ctx);

    /**
     * @brief Update MD5 context with new data
     */
    void Update(Context& ctx, const uint8_t* data, size_t length);

    /**
     * @brief Finalize MD5 hash and produce digest
     */
    void Finalize(Context& ctx, uint8_t digest[DIGEST_LENGTH]);

    /**
     * @brief Compute MD5 hash of data in one function call
     */
    void Hash(const uint8_t* data, size_t length, uint8_t digest[DIGEST_LENGTH]);

    /**
     * @brief Compute MD5 hash of string
     */
    std::string HashString(const std::string& input);

    /**
     * @brief Hash a file and return hex string
     */
    std::string HashFile(const std::string& filePath);

    /**
     * @brief Generate pseudo-random number from seed using MD5
     */
    uint32_t PseudoRandom(uint32_t seed);

    /**
     * @brief Compute HMAC-MD5 (Message Authentication Code)
     */
    std::string HMAC(const std::string& key, const std::string& message);
}

/**
 * @brief String obfuscation utilities using XOR encryption
 */
namespace StringObfuscation {

    /**
     * @brief Compile-time string XOR encryption template
     */
    template<size_t N, uint8_t Key>
    class XorString {
    private:
        char m_data[N];
        mutable bool m_decrypted;

    public:
        constexpr XorString(const char (&str)[N]) : m_data{}, m_decrypted(false) {
            for (size_t i = 0; i < N; ++i) {
                m_data[i] = str[i] ^ (Key + static_cast<uint8_t>(i));
            }
        }

        ~XorString() {
            // Clear decrypted data for security
            for (size_t i = 0; i < N; ++i) {
                m_data[i] = 0;
            }
        }

        const char* decrypt() const {
            if (!m_decrypted) {
                XorString* mutable_this = const_cast<XorString*>(this);
                for (size_t i = 0; i < N; ++i) {
                    mutable_this->m_data[i] ^= (Key + static_cast<uint8_t>(i));
                }
                mutable_this->m_decrypted = true;
            }
            return m_data;
        }

        void encrypt() const {
            if (m_decrypted) {
                XorString* mutable_this = const_cast<XorString*>(this);
                for (size_t i = 0; i < N; ++i) {
                    mutable_this->m_data[i] ^= (Key + static_cast<uint8_t>(i));
                }
                mutable_this->m_decrypted = false;
            }
        }

        size_t size() const { return N - 1; } // Exclude null terminator
    };

    /**
     * @brief Runtime string XOR encryption/decryption
     */
    class RuntimeXor {
    private:
        std::vector<uint8_t> m_data;
        uint8_t m_key;

    public:
        RuntimeXor(const std::string& plaintext, uint8_t key);
        std::string Decrypt() const;
        void Clear();
    };

    /**
     * @brief Simple XOR encryption for byte arrays
     */
    void XorEncrypt(uint8_t* data, size_t length, uint8_t key);

    /**
     * @brief Simple XOR decryption for byte arrays (same as encryption)
     */
    void XorDecrypt(uint8_t* data, size_t length, uint8_t key);

    /**
     * @brief Generate random key for XOR operations
     */
    uint8_t GenerateRandomKey();

    /**
     * @brief Advanced XOR with rotating key
     */
    std::string AdvancedXorEncrypt(const std::string& plaintext, const std::string& key);

    /**
     * @brief Advanced XOR decryption (same as encryption due to XOR symmetry)
     */
    std::string AdvancedXorDecrypt(const std::string& ciphertext, const std::string& key);

    /**
     * @brief Generate a random key of specified length
     */
    std::string GenerateRandomKey(size_t length);
}

// Convenience macro for compile-time string obfuscation
#define OBFUSCATED_STRING(str) \
    []() -> const char* { \
        static auto obfuscated = StringObfuscation::XorString<sizeof(str), 0xAA>{str}; \
        return obfuscated.decrypt(); \
    }()

/**
 * @brief Utility functions for data integrity and verification
 */
namespace DataIntegrity {

    /**
     * @brief Compute checksum of data using simple algorithm
     */
    uint32_t SimpleChecksum(const uint8_t* data, size_t length);

    /**
     * @brief Verify data integrity using MD5 hash
     */
    bool VerifyMD5(const uint8_t* data, size_t length, const std::string& expectedHash);

    /**
     * @brief Generate secure random bytes (platform-dependent)
     */
    std::vector<uint8_t> GenerateRandomBytes(size_t count);

    /**
     * @brief CRC32 checksum implementation
     */
    uint32_t CRC32(const uint8_t* data, size_t length);

    /**
     * @brief Enhanced secure random bytes using system entropy
     */
    std::vector<uint8_t> GenerateSecureRandomBytes(size_t count);

    /**
     * @brief Compare two byte arrays in constant time to prevent timing attacks
     */
    bool ConstantTimeCompare(const uint8_t* a, const uint8_t* b, size_t length);
}
