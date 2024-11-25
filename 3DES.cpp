#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <random>
#include <fstream>

// Permutation tables (simplified versions for demonstration)
const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7};

const int IP_INV[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25};

const int E[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};

const int P[32] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25};

// DES keys
const std::string key1 = "12345678";
const std::string key2 = "23456789";
const std::string key3 = "34567890";

// Converts string to hex for better readability
std::string toHex(const std::string &str)
{
    std::ostringstream oss;
    for (unsigned char c : str)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return oss.str();
}

// Converts hex back to string
std::string fromHex(const std::string &hex)
{
    std::string result;
    for (size_t i = 0; i < hex.size(); i += 2)
    {
        int byte;
        std::istringstream(hex.substr(i, 2)) >> std::hex >> byte;
        result += static_cast<char>(byte);
    }
    return result;
}

// Applies permutation based on a table
std::string applyPermutation(const std::string &data, const int *table, int tableSize)
{
    if (data.size() * 8 < tableSize)
    {
        throw std::runtime_error("Input data too short for permutation.");
    }
    std::string result(tableSize / 8, '\0'); // Result initialized to 0s
    for (int i = 0; i < tableSize; ++i)
    {
        int srcIndex = (table[i] - 1) / 8;
        int srcBit = 7 - (table[i] - 1) % 8;
        int destIndex = i / 8;
        int destBit = 7 - i % 8;

        if (data[srcIndex] & (1 << srcBit))
        {
            result[destIndex] |= (1 << destBit);
        }
    }
    return result;
}

// XOR two strings
std::string xorStrings(const std::string &a, const std::string &b)
{
    if (a.size() != b.size())
    {
        throw std::runtime_error("Strings must be of equal length for XOR.");
    }
    std::string result;
    for (size_t i = 0; i < a.size(); ++i)
    {
        result += a[i] ^ b[i];
    }
    return result;
}

// Generates a random 8-byte IV
std::string generateRandomIV()
{
    std::string iv(8, '\0');
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 255);

    for (char &c : iv)
    {
        c = static_cast<char>(distribution(generator));
    }
    return iv;
}

// Single DES encryption function (simplified)
std::string DES_encrypt(const std::string &data, const std::string &key)
{
    if (data.size() != 8 || key.size() != 8)
    {
        throw std::runtime_error("DES requires 8-byte data and key.");
    }
    std::string permutedData = applyPermutation(data, IP, 64);
    // Simulated encryption logic; real DES requires more steps
    std::string result = applyPermutation(permutedData, IP_INV, 64);
    return result;
}

// Single DES decryption function (simplified)
std::string DES_decrypt(const std::string &data, const std::string &key)
{
    if (data.size() != 8 || key.size() != 8)
    {
        throw std::runtime_error("DES requires 8-byte data and key.");
    }
    std::string permutedData = applyPermutation(data, IP_INV, 64);
    // Simulated decryption logic; real DES requires more steps
    std::string result = applyPermutation(permutedData, IP, 64);
    return result;
}

// Triple DES encryption with a random IV
std::string tripleDES_encrypt(const std::string &data, std::string &iv)
{
    iv = generateRandomIV(); // Generate a random IV
    std::string previousBlock = iv;
    std::string encryptedData;

    for (size_t i = 0; i < data.size(); i += 8)
    {
        std::string block = data.substr(i, 8);
        if (block.size() < 8)
        {
            block.append(8 - block.size(), '\0'); // Pad with zeros
        }
        block = xorStrings(block, previousBlock);
        block = DES_encrypt(block, key1);
        block = DES_encrypt(block, key2); // Use the second key
        block = DES_encrypt(block, key3);
        encryptedData += block;
        previousBlock = block;
    }
    return iv + encryptedData; // Prepend IV to the ciphertext
}

// Triple DES decryption with IV extraction
std::string tripleDES_decrypt(const std::string &data)
{
    if (data.size() < 8)
    {
        throw std::runtime_error("Data too short for decryption.");
    }
    std::string iv = data.substr(0, 8); // Extract IV
    std::string encryptedData = data.substr(8);

    std::string previousBlock = iv;
    std::string decryptedData;

    for (size_t i = 0; i < encryptedData.size(); i += 8)
    {
        std::string block = encryptedData.substr(i, 8);
        std::string tempBlock = block;
        block = DES_decrypt(block, key3);
        block = DES_decrypt(block, key2);
        block = DES_decrypt(block, key1);
        block = xorStrings(block, previousBlock);
        decryptedData += block;
        previousBlock = tempBlock;
    }
    return decryptedData;
}

// Input processing function
void processText(bool encrypt)
{
    std::string input;
    std::cout << "Enter text: ";
    std::cin.ignore();
    std::getline(std::cin, input);

    try
    {
        if (encrypt)
        {
            std::string iv;
            std::string encrypted = tripleDES_encrypt(input, iv);
            std::cout << "Encrypted text (hex): " << toHex(encrypted) << std::endl;
        }
        else
        {
            std::string encrypted = fromHex(input);

            std::string decrypted = tripleDES_decrypt(encrypted);
            std::cout << "Decrypted text: " << decrypted << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// File encryption function
void processFile(bool encrypt) {
    std::string inputFile, outputFile;
    std::cout << "Enter the input file path: ";
    std::cin >> inputFile;
    std::cout << "Enter the output file path: ";
    std::cin >> outputFile;

    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening input file!" << std::endl;
        return;
    }

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        return;
    }

    std::string fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    
    try {
        std::string iv;
        std::string result;
        if (encrypt) {
            result = tripleDES_encrypt(fileData, iv);
        } else {
            result = tripleDES_decrypt(fileData);
        }
        outFile.write(result.c_str(), result.size());
        std::cout << "File " << (encrypt ? "encrypted" : "decrypted") << " successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Main function
int main() {
    int choice;
    std::cout << "1. Encrypt Text\n2. Decrypt Text\n3. Encrypt File\n4. Decrypt File\n5. Exit\nEnter choice: ";
    std::cin >> choice;

    switch (choice) {
    case 1:
        processText(true);
        break;
    case 2:
        processText(false);
        break;
    case 3:
        processFile(true);
        break;
    case 4:
        processFile(false);
        break;
    default:
        std::cout << "Exiting." << std::endl;
    }
    return 0;
}
