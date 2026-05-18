#include <gtest/gtest.h>

#include <vector>
#include <cstdint>
#include <string>
#include <random>
#include <stdexcept>
#include "base85ed.h"
const std::vector<std::pair<const char*, const char* >> short_cases =
{
    { "",     ""     },
    { "F#",   "1"    },
    { "F){",  "12"   },
    { "F)}j", "123"  },
    { "F)}kW","1234" }
};

static std::vector<uint8_t> cstr2v(const char* s)
{
    return std::vector<uint8_t>(
        s,
        s + std::string(s).size()
    );
}


static std::vector<uint8_t> str2vec(const std::string& s)
{
    return std::vector<uint8_t>(
        s.begin(),
        s.end()
    );
}

static std::string repeat_string(
    const std::string& s,
    int times
)
{
    std::string result;

    for (int i = 0; i < times; ++i)
    {
        result += s;
    }

    return result;
}

TEST(Base85ShortsEncode, TrivialShortEncodes)
{
    for (const auto& p : short_cases)
    {
        EXPECT_EQ(base85::encode(cstr2v(p.second)), cstr2v(p.first));
    }
}

TEST(Base85ShortsDecode, TrivialShortDecodes)
{
    for (const auto& p : short_cases)
    {
        EXPECT_EQ(base85::decode(cstr2v(p.first)), cstr2v(p.second));
    }
}


TEST(Base85EncodeDecode, EmptyString)
{
    std::vector<uint8_t> input;

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, OneCharacter)
{
    auto input = str2vec("A");

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, TwoCharacters)
{
    auto input = str2vec("AB");

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, ThreeCharacters)
{
    auto input = str2vec("ABC");

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, FourCharacters)
{
    auto input = str2vec("ABCD");

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, FiveCharacters)
{
    auto input = str2vec("ABCDE");

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, SimpleSentence)
{
    auto input = str2vec("Hello world!");

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, LongEnglishText)
{
    std::string text =
        repeat_string(
            "The quick brown fox jumps over the lazy dog. ",
            1000
        );

    auto input = str2vec(text);

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, HugeText)
{
    std::string text =
        repeat_string(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
            5000
        );

    auto input = str2vec(text);

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, BinaryDataAllBytes)
{
    std::vector<uint8_t> input;

    for (int i = 0; i < 256; ++i)
    {
        input.push_back(
            static_cast<uint8_t>(i)
        );
    }

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, LargeBinaryBlock)
{
    std::vector<uint8_t> input;

    for (int i = 0; i < 100000; ++i)
    {
        input.push_back(
            static_cast<uint8_t>(i % 256)
        );
    }

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85EncodeDecode, RandomBinaryDataSmall)
{
    std::mt19937 rng(12345);

    for (int size = 1; size <= 1000; ++size)
    {
        std::vector<uint8_t> input(size);

        for (int i = 0; i < size; ++i)
        {
            input[i] = rng() % 256;
        }

        auto encoded = base85::encode(input);
        auto decoded = base85::decode(encoded);

        EXPECT_EQ(decoded, input);
    }
}

TEST(Base85EncodeDecode, RandomBinaryDataMedium)
{
    std::mt19937 rng(77777);

    for (int size = 1000; size <= 20000; size += 997)
    {
        std::vector<uint8_t> input(size);

        for (int i = 0; i < size; ++i)
        {
            input[i] = rng() % 256;
        }

        auto encoded = base85::encode(input);
        auto decoded = base85::decode(encoded);

        EXPECT_EQ(decoded, input);
    }
}

TEST(Base85EncodeDecode, RandomBinaryDataHuge)
{
    std::mt19937 rng(99999);

    std::vector<uint8_t> input(500000);

    for (size_t i = 0; i < input.size(); ++i)
    {
        input[i] = rng() % 256;
    }

    auto encoded = base85::encode(input);
    auto decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(Base85Decode, InvalidCharacterSpace)
{
    std::vector<uint8_t> invalid =
    {
        ' ',
        'A',
        'B'
    };

    EXPECT_THROW(
        base85::decode(invalid),
        std::runtime_error
    );
}

TEST(Base85Decode, InvalidCharacterNewline)
{
    std::vector<uint8_t> invalid =
    {
        '\n',
        'A',
        'B'
    };

    EXPECT_THROW(
        base85::decode(invalid),
        std::runtime_error
    );
}

TEST(Base85Decode, InvalidCharacterTab)
{
    std::vector<uint8_t> invalid =
    {
        '\t',
        'A',
        'B'
    };

    EXPECT_THROW(
        base85::decode(invalid),
        std::runtime_error
    );
}

TEST(Base85Decode, InvalidBinaryData)
{
    std::vector<uint8_t> invalid =
    {
        0,
        1,
        2,
        3,
        255
    };

    EXPECT_THROW(
        base85::decode(invalid),
        std::runtime_error
    );
}

TEST(Base85StressTest, MultipleSequentialOperations)
{
    std::mt19937 rng(55555);

    for (int iteration = 0; iteration < 100; ++iteration)
    {
        std::vector<uint8_t> input(10000);

        for (size_t i = 0; i < input.size(); ++i)
        {
            input[i] = rng() % 256;
        }

        auto encoded = base85::encode(input);
        auto decoded = base85::decode(encoded);

        EXPECT_EQ(decoded, input);
    }
}