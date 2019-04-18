# 🔐 Crypto Utilities Library

A comprehensive cryptographic utilities library providing educational implementations of common cryptographic algorithms and security utilities. Designed for learning, security research, and data integrity verification.

## ✨ Features

### 🔗 MD5 Hashing
- **Complete MD5 Implementation**: Full MD5 algorithm with proper state management
- **Incremental Processing**: Support for streaming large data sets
- **Multiple Input Types**: Hash strings, binary data, and files
- **Thread-Safe Operations**: Concurrent hashing operations supported
- **HMAC Support**: Message Authentication Code generation

### 🔒 String Obfuscation
- **Compile-Time Encryption**: XOR string encryption at compile time
- **Runtime Encryption**: Dynamic string encryption/decryption
- **Secure Memory Management**: Automatic memory clearing on destruction
- **Advanced XOR**: Multi-key rotating XOR encryption
- **Random Key Generation**: Cryptographically secure key generation

### 🛡️ Data Integrity
- **Checksum Algorithms**: Simple and CRC32 checksums
- **Hash Verification**: MD5-based integrity verification
- **Secure Random Generation**: Platform-specific entropy sources
- **Constant-Time Comparison**: Timing attack prevention
- **Corruption Detection**: Automated data integrity checks

## 🚀 Quick Start

### Basic Usage
```cpp
#include "libraries/crypto-utils/CryptoUtils.hpp"
#include <iostream>

int main() {
    // MD5 Hashing
    std::string message = "Hello, World!";
    std::string hash = MD5::HashString(message);
    std::cout << "MD5 Hash: " << hash << std::endl;
    
    // HMAC Generation
    std::string key = "secret_key";
    std::string hmac = MD5::HMAC(key, message);
    std::cout << "HMAC: " << hmac << std::endl;
    
    // Compile-time String Obfuscation
    const char* obfuscated = OBFUSCATED_STRING("Secret Message");
    std::cout << "Obfuscated: " << obfuscated << std::endl;
    
    // Runtime Encryption
    StringObfuscation::RuntimeXor encrypted("Sensitive Data", 0xAA);
    std::string decrypted = encrypted.Decrypt();
    std::cout << "Decrypted: " << decrypted << std::endl;
    
    // Data Integrity Verification
    uint8_t data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
    uint32_t checksum = DataIntegrity::SimpleChecksum(data, sizeof(data));
    uint32_t crc32 = DataIntegrity::CRC32(data, sizeof(data));
    
    std::cout << "Simple Checksum: 0x" << std::hex << checksum << std::endl;
    std::cout << "CRC32: 0x" << std::hex << crc32 << std::endl;
    
    // Secure Random Generation
    auto randomBytes = DataIntegrity::GenerateSecureRandomBytes(16);
    std::cout << "Random bytes generated: " << randomBytes.size() << " bytes" << std::endl;
    
    return 0;
}
```

### Advanced Usage
```cpp
// Incremental MD5 Hashing
MD5::Context ctx;
MD5::Initialize(ctx);

// Process data in chunks
std::string chunk1 = "Hello, ";
std::string chunk2 = "World!";

MD5::Update(ctx, reinterpret_cast<const uint8_t*>(chunk1.c_str()), chunk1.length());
MD5::Update(ctx, reinterpret_cast<const uint8_t*>(chunk2.c_str()), chunk2.length());

uint8_t digest[MD5::DIGEST_LENGTH];
MD5::Finalize(ctx, digest);

// Advanced XOR Encryption
std::string plaintext = "This is a secret message";
std::string key = "encryption_key_123";

std::string encrypted = StringObfuscation::AdvancedXorEncrypt(plaintext, key);
std::string decrypted = StringObfuscation::AdvancedXorDecrypt(encrypted, key);

assert(plaintext == decrypted);
```

## 📚 API Reference

### MD5 Namespace
- `HashString(const std::string& input)` - Hash a string
- `Hash(const uint8_t* data, size_t length, uint8_t digest[16])` - Hash binary data
- `PseudoRandom(uint32_t seed)` - Generate pseudo-random number

### StringObfuscation Namespace
- `XorString<N, Key>` - Compile-time string encryption template
- `RuntimeXor` - Runtime string encryption class
- `XorEncrypt/XorDecrypt` - Byte array encryption
- `GenerateRandomKey()` - Random key generation

### DataIntegrity Namespace
- `SimpleChecksum(const uint8_t* data, size_t length)` - Calculate checksum
- `VerifyMD5(const uint8_t* data, size_t length, const std::string& expectedHash)` - Verify integrity
- `GenerateRandomBytes(size_t count)` - Generate random data

## Security Notes

⚠️ **Educational Purpose Only**: This implementation is for educational and demonstration purposes. For production applications, use established cryptographic libraries like OpenSSL.

⚠️ **MD5 Deprecation**: MD5 is cryptographically broken and should not be used for security-critical applications. This implementation is provided for educational purposes only.

## Compilation

```bash
g++ -std=c++17 -I. your_program.cpp -o your_program
```

## Dependencies

- C++17 or later
- Standard library only (no external dependencies)

## Thread Safety

- MD5 operations are thread-safe when using separate contexts
- String obfuscation operations are thread-safe
- Data integrity functions are thread-safe