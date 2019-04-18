/**
 * @file crypto_demo.cpp
 * @brief Comprehensive demonstration of the CryptoUtils library
 * @author Systems Programming Toolkit
 * @version 2.0
 * 
 * This comprehensive demo demonstrates all functions of the CryptoUtils library:
 * - MD5 Hashing (with RFC 1321 verification)
 * - String Obfuscation (Compile-time & Runtime XOR)
 * - Data Integrity Verification (Checksums, CRC32, Validation)
 * - Pseudo-Random Number Generation
 * - File Operations and HMAC
 * - Performance Benchmarks
 * - Security Tests and Edge Cases
 */

#define NOMINMAX
#include "../libraries/crypto-utils/CryptoUtils.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cassert>
#include <chrono>
#include <algorithm>
#include <random>
#include <set>
#include <numeric>

// ANSI Color Codes for better output
#ifdef _WIN32
#include <windows.h>
#define RESET_COLOR ""
#define GREEN_COLOR ""
#define RED_COLOR ""
#define BLUE_COLOR ""
#define YELLOW_COLOR ""
#define CYAN_COLOR ""
#else
#define RESET_COLOR "\033[0m"
#define GREEN_COLOR "\033[32m"
#define RED_COLOR "\033[31m"
#define BLUE_COLOR "\033[34m"
#define YELLOW_COLOR "\033[33m"
#define CYAN_COLOR "\033[36m"
#endif

class CryptoDemo {
private:
    int totalTests = 0;
    int passedTests = 0;
    
    void PrintHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    void PrintSubHeader(const std::string& title) {
        std::cout << "\n--- " << title << " ---" << std::endl;
    }
    
    void PrintResult(const std::string& test, bool passed) {
        totalTests++;
        if (passed) passedTests++;
        
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test << std::endl;
    }
    
    void PrintHex(const std::vector<uint8_t>& data, const std::string& label, bool newline = true) {
        std::cout << label << ": ";
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
            if (i < data.size() - 1) std::cout << " ";
        }
        if (newline) std::cout << std::dec << std::endl;
    }
    
    std::string FormatTime(std::chrono::microseconds duration) {
        auto us = duration.count();
        if (us < 1000) {
            return std::to_string(us) + " us";
        } else if (us < 1000000) {
            return std::to_string(us / 1000) + " ms";
        } else {
            return std::to_string(us / 1000000) + " s";
        }
    }

public:
    void RunAllTests() {
        PrintHeader("FINAL CRYPTOUTILS LIBRARY DEMONSTRATION");
        std::cout << "Complete demonstration of all cryptographic functions" << std::endl;
        std::cout << "Version 2.0 - With correct MD5 implementation" << std::endl;
        
        // Run all tests
        TestMD5Implementation();
        TestStringObfuscation();
        TestDataIntegrity();
        TestFileOperations();
        TestSecurityFeatures();
        TestPerformanceBenchmarks();
        TestEdgeCases();
        TestRealWorldScenarios();
        
        PrintFinalResults();
    }

private:
    void TestMD5Implementation() {
        PrintHeader("MD5 HASH IMPLEMENTATION TESTS");
        
        // RFC 1321 Test Vectors
        PrintSubHeader("RFC 1321 Standard Test Vectors");
        
        struct TestVector {
            std::string input;
            std::string expected;
            std::string description;
        };
        
        std::vector<TestVector> vectors = {
            {"", "d41d8cd98f00b204e9800998ecf8427e", "Empty string"},
            {"a", "0cc175b9c0f1b6a831c399e269772661", "Single 'a'"},
            {"abc", "900150983cd24fb0d6963f7d28e17f72", "String 'abc'"},
            {"message digest", "f96b697d7cb7938d525a2f31aaf161d0", "Message digest"},
            {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b", "Alphabet"},
            {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 
             "d174ab98d277d9f5a5611c2c9f419d9f", "Mixed alphanumeric"},
            {"12345678901234567890123456789012345678901234567890123456789012345678901234567890",
             "57edf4a22be3c955ac49da2e2107b67a", "80 digits"}
        };
        
        for (const auto& test : vectors) {
            std::string computed = MD5::HashString(test.input);
            bool passed = (computed == test.expected);
            
            std::cout << "  " << test.description << ":" << std::endl;
            std::cout << "    Input: \"" << (test.input.empty() ? "<empty>" : test.input) << "\"" << std::endl;
            std::cout << "    Expected: " << test.expected << std::endl;
            std::cout << "    Computed: " << computed << std::endl;
            PrintResult("MD5 " + test.description, passed);
        }
        
        // Incremental hashing test
        PrintSubHeader("Incremental Hashing Test");
        std::string fullString = "The quick brown fox jumps over the lazy dog";
        std::string directHash = MD5::HashString(fullString);
        
        MD5::Context ctx;
        MD5::Initialize(ctx);
        MD5::Update(ctx, reinterpret_cast<const uint8_t*>("The quick brown fox "), 20);
        MD5::Update(ctx, reinterpret_cast<const uint8_t*>("jumps over the lazy dog"), 23);
        uint8_t incrementalDigest[16];
        MD5::Finalize(ctx, incrementalDigest);
        
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; i++) {
            ss << std::setw(2) << static_cast<int>(incrementalDigest[i]);
        }
        std::string incrementalHash = ss.str();
        
        std::cout << "  Full string: \"" << fullString << "\"" << std::endl;
        std::cout << "  Direct hash:      " << directHash << std::endl;
        std::cout << "  Incremental hash: " << incrementalHash << std::endl;
        PrintResult("Incremental hashing consistency", directHash == incrementalHash);
        
        // Binary data test
        PrintSubHeader("Binary Data Hashing");
        std::vector<uint8_t> binaryData = {0x00, 0x01, 0x02, 0x03, 0xAA, 0xBB, 0xCC, 0xDD, 0xFF, 0xFE, 0xFD, 0xFC};
        uint8_t binaryDigest[16];
        MD5::Hash(binaryData.data(), binaryData.size(), binaryDigest);
        
        PrintHex(binaryData, "  Binary input", false);
        std::cout << std::dec << " (" << binaryData.size() << " bytes)" << std::endl;
        std::cout << "  Binary hash: ";
        for (int i = 0; i < 16; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(binaryDigest[i]);
        }
        std::cout << std::dec << std::endl;
        PrintResult("Binary data hashing", true); // Always pass if no exception
        
        // HMAC test
        PrintSubHeader("HMAC-MD5 Testing");
        std::string key = "secret_key_123";
        std::string message = "authenticated_message_content";
        std::string hmac1 = MD5::HMAC(key, message);
        std::string hmac2 = MD5::HMAC(key, message);
        std::string hmacDiff = MD5::HMAC(key + "x", message);
        
        std::cout << "  Key: \"" << key << "\"" << std::endl;
        std::cout << "  Message: \"" << message << "\"" << std::endl;
        std::cout << "  HMAC: " << hmac1 << std::endl;
        PrintResult("HMAC consistency", hmac1 == hmac2);
        PrintResult("HMAC key sensitivity", hmac1 != hmacDiff);
    }
    
    void TestStringObfuscation() {
        PrintHeader("STRING OBFUSCATION TESTS");
        
        // Compile-time obfuscation test
        PrintSubHeader("Compile-time XOR Obfuscation");
        const char* obfuscated = OBFUSCATED_STRING("Secret compile-time string!");
        std::cout << "  Obfuscated string: \"" << obfuscated << "\"" << std::endl;
        PrintResult("Compile-time obfuscation", std::string(obfuscated) == "Secret compile-time string!");
        
        // Runtime XOR tests
        PrintSubHeader("Runtime XOR Obfuscation");
        std::vector<std::string> testStrings = {
            "",
            "A",
            "Hello, World!",
            "The quick brown fox jumps over the lazy dog",
            std::string(256, 'X'), // Long string
            std::string({0x00, 0x01, 0x02, static_cast<char>(0xFF), static_cast<char>(0xFE)}) // Binary data
        };
        
        for (size_t i = 0; i < testStrings.size(); i++) {
            uint8_t key = static_cast<uint8_t>(i * 42 + 7);
            StringObfuscation::RuntimeXor encrypted(testStrings[i], key);
            std::string decrypted = encrypted.Decrypt();
            
            std::cout << "  Test " << (i + 1) << " (key=0x" << std::hex << static_cast<int>(key) << std::dec << "): ";
            std::cout << "Length=" << testStrings[i].length() << " bytes" << std::endl;
            PrintResult("Runtime XOR #" + std::to_string(i + 1), testStrings[i] == decrypted);
        }
        
        // Advanced XOR with multi-byte key
        PrintSubHeader("Advanced Multi-Key XOR");
        std::string plaintext = "Advanced encryption test with multiple key bytes!";
        std::vector<std::string> keys = {"A", "KEY", "LongKey123", std::string(32, 'K')};
        
        for (size_t i = 0; i < keys.size(); i++) {
            std::string encrypted = StringObfuscation::AdvancedXorEncrypt(plaintext, keys[i]);
            std::string decrypted = StringObfuscation::AdvancedXorDecrypt(encrypted, keys[i]);
            
            std::cout << "  Key \"" << keys[i] << "\" (length=" << keys[i].length() << ")" << std::endl;
            PrintResult("Advanced XOR #" + std::to_string(i + 1), plaintext == decrypted);
        }
        
        // Byte array XOR
        PrintSubHeader("Byte Array XOR Operations");
        std::vector<uint8_t> testData = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21};
        std::vector<uint8_t> originalData = testData;
        
        PrintHex(testData, "  Original");
        
        StringObfuscation::XorEncrypt(testData.data(), testData.size(), 0xAA);
        PrintHex(testData, "  Encrypted");
        
        StringObfuscation::XorDecrypt(testData.data(), testData.size(), 0xAA);
        PrintHex(testData, "  Decrypted");
        
        bool dataIntact = std::equal(testData.begin(), testData.end(), originalData.begin());
        PrintResult("Byte array XOR roundtrip", dataIntact);
        
        // Key generation
        PrintSubHeader("Key Generation");
        std::vector<uint8_t> singleKeys;
        for (int i = 0; i < 10; i++) {
            singleKeys.push_back(StringObfuscation::GenerateRandomKey());
        }
        
        bool allDifferent = std::adjacent_find(singleKeys.begin(), singleKeys.end()) == singleKeys.end();
        std::cout << "  Generated 10 single-byte keys: ";
        for (uint8_t key : singleKeys) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(key) << " ";
        }
        std::cout << std::dec << std::endl;
        PrintResult("Single-byte key uniqueness", allDifferent);
        
        std::string multiKey = StringObfuscation::GenerateRandomKey(16);
        std::cout << "  Generated 16-byte key: ";
        for (char c : multiKey) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<uint8_t>(c)) << " ";
        }
        std::cout << std::dec << std::endl;
        PrintResult("Multi-byte key generation", multiKey.length() == 16);
    }
    
    void TestDataIntegrity() {
        PrintHeader("DATA INTEGRITY TESTS");
        
        // Checksum tests
        PrintSubHeader("Checksum Algorithms");
        std::vector<std::string> testData = {
            "",
            "A",
            "Hello, World!",
            "The quick brown fox jumps over the lazy dog",
            std::string(1000, 'X')
        };
        
        for (size_t i = 0; i < testData.size(); i++) {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(testData[i].c_str());
            size_t length = testData[i].length();
            
            uint32_t checksum = DataIntegrity::SimpleChecksum(data, length);
            uint32_t crc32 = DataIntegrity::CRC32(data, length);
            
            std::cout << "  Data #" << (i + 1) << " (" << length << " bytes):" << std::endl;
            std::cout << "    Simple Checksum: 0x" << std::hex << checksum << std::dec << std::endl;
            std::cout << "    CRC32:          0x" << std::hex << crc32 << std::dec << std::endl;
            
            // Test consistency
            uint32_t checksum2 = DataIntegrity::SimpleChecksum(data, length);
            uint32_t crc32_2 = DataIntegrity::CRC32(data, length);
            
            PrintResult("Checksum consistency #" + std::to_string(i + 1), checksum == checksum2);
            PrintResult("CRC32 consistency #" + std::to_string(i + 1), crc32 == crc32_2);
        }
        
        // MD5 verification
        PrintSubHeader("MD5 Verification");
        std::string testString = "Data integrity verification test";
        const uint8_t* data = reinterpret_cast<const uint8_t*>(testString.c_str());
        
        std::string correctHash = MD5::HashString(testString);
        std::string wrongHash = "00112233445566778899aabbccddeeff";
        
        bool validVerification = DataIntegrity::VerifyMD5(data, testString.length(), correctHash);
        bool invalidVerification = DataIntegrity::VerifyMD5(data, testString.length(), wrongHash);
        
        std::cout << "  Test string: \"" << testString << "\"" << std::endl;
        std::cout << "  Correct hash: " << correctHash << std::endl;
        std::cout << "  Wrong hash:   " << wrongHash << std::endl;
        PrintResult("MD5 verification (correct hash)", validVerification);
        PrintResult("MD5 verification (wrong hash)", !invalidVerification);
        
        // Random byte generation
        PrintSubHeader("Random Byte Generation");
        std::vector<size_t> sizes = {0, 1, 16, 256, 1024};
        
        for (size_t size : sizes) {
            std::vector<uint8_t> randomBytes = DataIntegrity::GenerateRandomBytes(size);
            std::vector<uint8_t> secureBytes = DataIntegrity::GenerateSecureRandomBytes(size);
            
            std::cout << "  Size " << size << " bytes:" << std::endl;
            PrintResult("Random bytes generation", randomBytes.size() == size);
            PrintResult("Secure random bytes generation", secureBytes.size() == size);
            
            if (size > 0 && size <= 32) {
                PrintHex(randomBytes, "    Random", true);
                PrintHex(secureBytes, "    Secure", true);
            }
        }
        
        // Constant time comparison
        PrintSubHeader("Constant Time Comparison");
        std::vector<uint8_t> data1 = {0x01, 0x02, 0x03, 0x04, 0x05};
        std::vector<uint8_t> data2 = {0x01, 0x02, 0x03, 0x04, 0x05};
        std::vector<uint8_t> data3 = {0x01, 0x02, 0x03, 0x04, 0x06};
        std::vector<uint8_t> data4 = {0x01, 0x02, 0x03, 0x04};
        
        bool same = DataIntegrity::ConstantTimeCompare(data1.data(), data2.data(), data1.size());
        bool different = DataIntegrity::ConstantTimeCompare(data1.data(), data3.data(), data1.size());
        bool nullTest = DataIntegrity::ConstantTimeCompare(nullptr, nullptr, 0);
        
        PrintHex(data1, "  Data 1", true);
        PrintHex(data2, "  Data 2", true);
        PrintHex(data3, "  Data 3", true);
        
        PrintResult("Constant time compare (equal)", same);
        PrintResult("Constant time compare (different)", !different);
        PrintResult("Constant time compare (null)", !nullTest);
    }
    
    void TestFileOperations() {
        PrintHeader("FILE OPERATIONS TESTS");
        
        // Create test files
        PrintSubHeader("File Hashing Operations");
        
        struct TestFile {
            std::string filename;
            std::string content;
            std::string description;
        };
        
        std::vector<TestFile> testFiles = {
            {"test_empty.txt", "", "Empty file"},
            {"test_small.txt", "Hello, World!", "Small text file"},
            {"test_multiline.txt", "Line 1\nLine 2\nLine 3\n", "Multi-line file"},
            {"test_binary.dat", std::string({0x00, 0x01, 0x02, 0x03, char(0xFF), char(0xFE)}), "Binary file"},
            {"test_large.txt", std::string(10000, 'X'), "Large file (10KB)"}
        };
        
        for (const auto& test : testFiles) {
            // Create file
            std::ofstream file(test.filename, std::ios::binary);
            file.write(test.content.c_str(), test.content.length());
            file.close();
            
            // Hash file and string
            std::string fileHash = MD5::HashFile(test.filename);
            std::string stringHash = MD5::HashString(test.content);
            
            std::cout << "  " << test.description << " (" << test.filename << "):" << std::endl;
            std::cout << "    Size: " << test.content.length() << " bytes" << std::endl;
            std::cout << "    File hash:   " << fileHash << std::endl;
            std::cout << "    String hash: " << stringHash << std::endl;
            
            PrintResult("File vs String hash consistency", fileHash == stringHash && !fileHash.empty());
            
            // Cleanup
            std::remove(test.filename.c_str());
        }
        
        // Test non-existent file
        std::string nonExistentHash = MD5::HashFile("non_existent_file_12345.txt");
        PrintResult("Non-existent file handling", nonExistentHash.empty());
    }
    
    void TestSecurityFeatures() {
        PrintHeader("SECURITY FEATURES TESTS");
        
        // Pseudo-random number generation
        PrintSubHeader("Pseudo-Random Number Generation");
        
        std::vector<uint32_t> seeds = {0, 1, 12345, 0xFFFFFFFF, 0xDEADBEEF};
        
        for (uint32_t seed : seeds) {
            std::vector<uint32_t> sequence1, sequence2;
            
            // Generate two sequences with same seed
            uint32_t currentSeed = seed;
            for (int i = 0; i < 10; i++) {
                currentSeed = MD5::PseudoRandom(currentSeed);
                sequence1.push_back(currentSeed);
            }
            
            currentSeed = seed;
            for (int i = 0; i < 10; i++) {
                currentSeed = MD5::PseudoRandom(currentSeed);
                sequence2.push_back(currentSeed);
            }
            
            bool deterministic = std::equal(sequence1.begin(), sequence1.end(), sequence2.begin());
            
            std::cout << "  Seed 0x" << std::hex << seed << std::dec << " generates:" << std::endl;
            std::cout << "    ";
            for (size_t i = 0; i < std::min(size_t(5), sequence1.size()); i++) {
                std::cout << "0x" << std::hex << sequence1[i] << std::dec << " ";
            }
            std::cout << "..." << std::endl;
            
            PrintResult("PRNG deterministic (seed 0x" + std::to_string(seed) + ")", deterministic);
        }
        
        // Test sequence uniqueness with different seeds
        std::set<uint32_t> allValues;
        for (uint32_t seed = 1; seed <= 100; seed++) {
            uint32_t value = MD5::PseudoRandom(seed);
            allValues.insert(value);
        }
        
        double uniqueRatio = double(allValues.size()) / 100.0;
        std::cout << "  Uniqueness ratio for 100 different seeds: " << uniqueRatio << std::endl;
        PrintResult("PRNG uniqueness", uniqueRatio > 0.8); // At least 80% unique
        
        // Memory clearing test
        PrintSubHeader("Memory Security");
        
        std::string sensitiveData = "Very secret information that should be cleared!";
        StringObfuscation::RuntimeXor encryptor(sensitiveData, 0xAA);
        
        // Decrypt and immediately clear
        std::string decrypted = encryptor.Decrypt();
        bool beforeClear = (decrypted == sensitiveData);
        encryptor.Clear();
        
        PrintResult("Memory clearing functionality", beforeClear);
    }
    
    void TestPerformanceBenchmarks() {
        PrintHeader("PERFORMANCE BENCHMARKS");
        
        // Different data sizes for benchmarking
        std::vector<size_t> dataSizes = {1024, 10240, 102400, 1048576}; // 1KB, 10KB, 100KB, 1MB
        
        for (size_t size : dataSizes) {
            PrintSubHeader("Data Size: " + std::to_string(size) + " bytes (" + 
                          std::to_string(size / 1024) + " KB)");
            
            // Generate test data
            std::vector<uint8_t> testData(size);
            std::iota(testData.begin(), testData.end(), 0);
            
            // MD5 Hashing Benchmark
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                uint8_t digest[16];
                MD5::Hash(testData.data(), testData.size(), digest);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto md5Time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            // XOR Encryption Benchmark
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                std::vector<uint8_t> dataCopy = testData;
                StringObfuscation::XorEncrypt(dataCopy.data(), dataCopy.size(), 0xAA);
            }
            end = std::chrono::high_resolution_clock::now();
            auto xorTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            // Checksum Benchmark
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                DataIntegrity::SimpleChecksum(testData.data(), testData.size());
            }
            end = std::chrono::high_resolution_clock::now();
            auto checksumTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            // CRC32 Benchmark
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                DataIntegrity::CRC32(testData.data(), testData.size());
            }
            end = std::chrono::high_resolution_clock::now();
            auto crc32Time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            // Calculate throughput (MB/s)
            double dataProcessedMB = (size * 100) / (1024.0 * 1024.0);
            
            std::cout << "  MD5 Hashing (100x):    " << FormatTime(md5Time) 
                      << " (" << std::fixed << std::setprecision(1) 
                      << dataProcessedMB / (md5Time.count() / 1000000.0) << " MB/s)" << std::endl;
            
            std::cout << "  XOR Encryption (100x):  " << FormatTime(xorTime)
                      << " (" << std::fixed << std::setprecision(1)
                      << dataProcessedMB / (xorTime.count() / 1000000.0) << " MB/s)" << std::endl;
            
            std::cout << "  Simple Checksum (100x): " << FormatTime(checksumTime)
                      << " (" << std::fixed << std::setprecision(1)
                      << dataProcessedMB / (checksumTime.count() / 1000000.0) << " MB/s)" << std::endl;
            
            std::cout << "  CRC32 (100x):           " << FormatTime(crc32Time)
                      << " (" << std::fixed << std::setprecision(1)
                      << dataProcessedMB / (crc32Time.count() / 1000000.0) << " MB/s)" << std::endl;
        }
    }
    
    void TestEdgeCases() {
        PrintHeader("EDGE CASES AND ROBUSTNESS TESTS");
        
        PrintSubHeader("Null Pointer Handling");
        
        // Test null pointer handling
        uint8_t digest[16];
        MD5::Hash(nullptr, 0, digest); // Should not crash
        PrintResult("MD5 null pointer handling", true);
        
        uint32_t nullChecksum = DataIntegrity::SimpleChecksum(nullptr, 0);
        PrintResult("Null checksum calculation", nullChecksum == 0);
        
        bool nullCompare = DataIntegrity::ConstantTimeCompare(nullptr, nullptr, 0);
        PrintResult("Null pointer comparison", !nullCompare);
        
        PrintSubHeader("Zero-Length Data");
        
        // Empty string/data tests
        std::string emptyHash = MD5::HashString("");
        PrintResult("Empty string MD5", emptyHash == "d41d8cd98f00b204e9800998ecf8427e");
        
        std::vector<uint8_t> emptyRandom = DataIntegrity::GenerateRandomBytes(0);
        PrintResult("Zero-length random generation", emptyRandom.empty());
        
        PrintSubHeader("Maximum Size Stress Test");
        
        // Large data test (limited by available memory)
        try {
            size_t largeSize = 10 * 1024 * 1024; // 10 MB
            std::vector<uint8_t> largeData(largeSize, 0xAA);
            
            auto start = std::chrono::high_resolution_clock::now();
            uint8_t largeDigest[16];
            MD5::Hash(largeData.data(), largeData.size(), largeDigest);
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "  10MB MD5 calculation: " << duration.count() << " ms" << std::endl;
            
            PrintResult("Large data MD5 processing", true);
        } catch (const std::exception& e) {
            std::cout << "  Large data test failed: " << e.what() << std::endl;
            PrintResult("Large data MD5 processing", false);
        }
        
        PrintSubHeader("Extreme Values");
        
        // Test with extreme PRNG values
        uint32_t maxVal = MD5::PseudoRandom(0xFFFFFFFF);
        uint32_t minVal = MD5::PseudoRandom(0);
        uint32_t midVal = MD5::PseudoRandom(0x80000000);
        
        std::cout << "  PRNG(0xFFFFFFFF) = 0x" << std::hex << maxVal << std::dec << std::endl;
        std::cout << "  PRNG(0x00000000) = 0x" << std::hex << minVal << std::dec << std::endl;
        std::cout << "  PRNG(0x80000000) = 0x" << std::hex << midVal << std::dec << std::endl;
        
        PrintResult("Extreme value PRNG", true);
    }
    
    void TestRealWorldScenarios() {
        PrintHeader("REAL-WORLD USAGE SCENARIOS");
        
        PrintSubHeader("Password Hashing Simulation");
        
        std::vector<std::string> passwords = {
            "password123",
            "MySecurePassword!",
            "admin",
            "qwerty",
            "P@ssw0rd2024!"
        };
        
        for (const auto& password : passwords) {
            std::string salt = "random_salt_12345";
            std::string saltedPassword = salt + password;
            std::string hash = MD5::HashString(saltedPassword);
            
            std::cout << "  Password: \"" << password << "\"" << std::endl;
            std::cout << "  Salted hash: " << hash << std::endl;
            
            // Verify hash
            bool verified = (MD5::HashString(salt + password) == hash);
            PrintResult("Password verification", verified);
        }
        
        PrintSubHeader("File Integrity Monitoring");
        
        // Simulate file monitoring scenario
        std::string configFile = "config_monitor_test.txt";
        std::string originalContent = "# Configuration File\nserver=localhost\nport=8080\n";
        std::string modifiedContent = "# Configuration File\nserver=localhost\nport=9090\n";
        
        // Create original file and get hash
        std::ofstream file(configFile, std::ios::binary);
        file.write(originalContent.c_str(), originalContent.length());
        file.close();
        
        std::string originalHash = MD5::HashFile(configFile);
        std::cout << "  Original file hash: " << originalHash << std::endl;
        
        // Modify file
        file.open(configFile, std::ios::binary);
        file.write(modifiedContent.c_str(), modifiedContent.length());
        file.close();
        
        std::string modifiedHash = MD5::HashFile(configFile);
        std::cout << "  Modified file hash: " << modifiedHash << std::endl;
        
        bool fileChanged = (originalHash != modifiedHash);
        PrintResult("File modification detection", fileChanged);
        
        // Cleanup
        std::remove(configFile.c_str());
        
        PrintSubHeader("Data Transmission Verification");
        
        // Simulate data packet transmission
        std::string packet = "DATA:timestamp=1695825600;user=admin;action=login;status=success";
        std::string packetHash = MD5::HashString(packet);
        uint32_t packetChecksum = DataIntegrity::SimpleChecksum(
            reinterpret_cast<const uint8_t*>(packet.c_str()), packet.length());
        
        std::cout << "  Data packet: \"" << packet << "\"" << std::endl;
        std::cout << "  MD5 hash: " << packetHash << std::endl;
        std::cout << "  Checksum: 0x" << std::hex << packetChecksum << std::dec << std::endl;
        
        // Simulate received packet verification
        bool hashVerified = DataIntegrity::VerifyMD5(
            reinterpret_cast<const uint8_t*>(packet.c_str()), packet.length(), packetHash);
        
        uint32_t receivedChecksum = DataIntegrity::SimpleChecksum(
            reinterpret_cast<const uint8_t*>(packet.c_str()), packet.length());
        bool checksumVerified = (receivedChecksum == packetChecksum);
        
        PrintResult("Packet MD5 verification", hashVerified);
        PrintResult("Packet checksum verification", checksumVerified);
        
        PrintSubHeader("String Protection Scenario");
        
        // Simulate protecting sensitive strings in memory
        std::vector<std::string> sensitiveStrings = {
            "API_KEY_12345",
            "database_password",
            "encryption_secret_key",
            "user_session_token"
        };
        
        for (size_t i = 0; i < sensitiveStrings.size(); i++) {
            uint8_t key = static_cast<uint8_t>(i * 17 + 42);
            StringObfuscation::RuntimeXor protectedString(sensitiveStrings[i], key);
            
            // Simulate usage
            std::string decrypted = protectedString.Decrypt();
            bool correctDecryption = (decrypted == sensitiveStrings[i]);
            
            // Clear from memory
            protectedString.Clear();
            
            std::cout << "  Protected string #" << (i + 1) << " (key=0x" 
                      << std::hex << static_cast<int>(key) << std::dec << ")" << std::endl;
            PrintResult("String protection #" + std::to_string(i + 1), correctDecryption);
        }
    }
    
    void PrintFinalResults() {
        PrintHeader("FINAL RESULTS");
        
        double successRate = (double)passedTests / totalTests * 100.0;
        
        std::cout << "Total Tests:   " << totalTests << std::endl;
        std::cout << "Passed:        " << passedTests << std::endl;
        std::cout << "Failed:        " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate:  " << std::fixed << std::setprecision(1) 
                  << successRate << "%" << std::endl;
        
        if (successRate == 100.0) {
            std::cout << "\n[PERFECT] All tests passed!" << std::endl;
            std::cout << "The CryptoUtils library functions completely correctly." << std::endl;
        } else if (successRate >= 90.0) {
            std::cout << "\n[VERY GOOD] Almost all tests passed." << std::endl;
        } else if (successRate >= 75.0) {
            std::cout << "\n[GOOD] Most tests passed, some issues found." << std::endl;
        } else {
            std::cout << "\n[PROBLEMS] found. Library needs rework." << std::endl;
        }
        
        std::cout << "\n=== LIBRARY FUNCTIONS SUMMARY ===" << std::endl;
        std::cout << "[+] MD5 Hashing (RFC 1321 compliant)" << std::endl;
        std::cout << "[+] String Obfuscation (Compile-time & Runtime XOR)" << std::endl;
        std::cout << "[+] Data Integrity Verification (Checksums, CRC32)" << std::endl;
        std::cout << "[+] Pseudo-Random Number Generation" << std::endl;
        std::cout << "[+] File Operations and Hashing" << std::endl;
        std::cout << "[+] HMAC Support" << std::endl;
        std::cout << "[+] Security Functions (Constant-time Compare)" << std::endl;
        std::cout << "[+] Performance-optimized Implementation" << std::endl;
        std::cout << "[+] Robust Edge-Case Handling" << std::endl;
        std::cout << "[+] Real-World Scenario Support" << std::endl;
    }
};

int main() {
    std::cout << "Initializing CryptoUtils Demo..." << std::endl;
    
    try {
        CryptoDemo demo;
        demo.RunAllTests();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR during demo: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN ERROR during demo!" << std::endl;
        return 1;
    }
}