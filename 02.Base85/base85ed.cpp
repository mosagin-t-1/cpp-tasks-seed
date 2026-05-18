#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <array>
#include <algorithm>

#include "base85ed.h"

namespace
{

    const char* BASE85_ALPHABET =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";

    std::array<int, 256> create_decode_table()
    {
        std::array<int, 256> table{};

        table.fill(-1);

        for (int i = 0; i < 85; ++i)
        {
            table[
                static_cast<unsigned char>(
                    BASE85_ALPHABET[i]
                    )
            ] = i;
        }

        return table;
    }

    const auto DECODE_TABLE = create_decode_table();

} // namespace


// TODO: implement this in C++
std::vector<uint8_t> base85::encode(
    std::vector<uint8_t> const& bytes
)
{
    std::vector<uint8_t> out;

    size_t i = 0;

    while (i < bytes.size())
    {
        uint32_t value = 0;

        size_t chunk_size =
            std::min<size_t>(
                4,
                bytes.size() - i
            );

        for (size_t j = 0; j < chunk_size; ++j)
        {
            value |=
                static_cast<uint32_t>(
                    bytes[i + j]
                    ) << (24 - j * 8);
        }

        char encoded[5];

        uint32_t tmp = value;

        for (int j = 4; j >= 0; --j)
        {
            encoded[j] =
                BASE85_ALPHABET[tmp % 85];

            tmp /= 85;
        }

        size_t out_size =
            (chunk_size == 4)
            ? 5
            : chunk_size + 1;

        for (size_t j = 0; j < out_size; ++j)
        {
            out.push_back(
                static_cast<uint8_t>(
                    encoded[j]
                    )
            );
        }

        i += chunk_size;
    }

    return out;
}


// TODO: implement this in C++
std::vector<uint8_t> base85::decode(
    std::vector<uint8_t> const& b85str
)
{
    std::vector<uint8_t> out;

    size_t i = 0;

    while (i < b85str.size())
    {
        size_t chunk_size =
            std::min<size_t>(
                5,
                b85str.size() - i
            );

        uint32_t value = 0;

        int padding = 0;

        for (size_t j = 0; j < 5; ++j)
        {
            int digit;

            if (j < chunk_size)
            {
                unsigned char ch =
                    static_cast<unsigned char>(
                        b85str[i + j]
                        );

                digit = DECODE_TABLE[ch];

                if (digit == -1)
                {
                    throw std::runtime_error(
                        "Invalid Base85 character"
                    );
                }
            }
            else
            {
                digit = 84;
                ++padding;
            }

            value = value * 85 + digit;
        }

        uint8_t decoded[4];

        decoded[0] = (value >> 24) & 0xFF;
        decoded[1] = (value >> 16) & 0xFF;
        decoded[2] = (value >> 8) & 0xFF;
        decoded[3] = value & 0xFF;

        size_t bytes_to_write = 4 - padding;

        for (size_t j = 0; j < bytes_to_write; ++j)
        {
            out.push_back(decoded[j]);
        }

        i += chunk_size;
    }

    return out;
}
