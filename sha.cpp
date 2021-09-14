#include <iostream>
#include <bitset>
#include <cstring>
#include <math.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "constants.h"

using namespace std;
using namespace boost::multiprecision;

uint32_t rotate_right(uint32_t chain, int rotate_amount) {
    return (chain >> rotate_amount) | (chain << (32 - rotate_amount));
}

uint32_t mod_add(uint32_t chain1, uint32_t chain2) {
    return (chain1 + chain2);// % 4294967296; // O overflow resolve o modulo sozinho
}

uint32_t l_sigma0(uint32_t chain) {
    uint32_t rotr7 = rotate_right(chain, 7);
    uint32_t rotr18 = rotate_right(chain, 18);
    uint32_t shiftr3 = chain >> 3;
    return rotr7 ^ rotr18 ^ shiftr3;
}

uint32_t l_sigma1(uint32_t chain) {
    uint32_t rotr17 = rotate_right(chain, 17);
    uint32_t rotr19 = rotate_right(chain, 19);
    uint32_t shiftr10 = chain >> 10;
    return rotr17 ^ rotr19 ^ shiftr10;
}

uint32_t u_sigma0(uint32_t chain) {
    uint32_t rotr2 = rotate_right(chain, 2);
    uint32_t rotr13 = rotate_right(chain, 13);
    uint32_t rotr22 = rotate_right(chain, 22);
    return rotr2 ^ rotr13 ^ rotr22;
}

uint32_t u_sigma1(uint32_t chain) {
    uint32_t rotr6 = rotate_right(chain, 6);
    uint32_t rotr11 = rotate_right(chain, 11);
    uint32_t rotr25 = rotate_right(chain, 25);
    return rotr6 ^ rotr11 ^ rotr25;
}

uint32_t choice(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) | (~x & z);
}

uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
    return (b & c) | (a & c) | (a & b);
}

int get_n_blocks(int msg_bit_size) {
    return ceil((msg_bit_size + 65) / 512.0); // 1 bit + 64 bits no final
}

void print512bit(uint512_t r) {
    std::bitset<512> b;
    size_t idx = 0;
    while(r) {                   // loop for as long as "num" isn't zero
        b[idx++] = (r & 1) != 0; // check the least significant bit
        r >>= 1;                 // shift down by one
    }

    // print the result
    std::cout << b << '\n';
}

void print_hex(u_int32_t i) {
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(u_int32_t)*2) 
         << std::hex << i;
  cout << stream.str();
}

int main(int argc, char** argv) {
    uint32_t x = 0b10011;
    uint32_t y = 0b11001;
    uint32_t z = 0b10100;
    
    char* msg = argv[1];
    int msg_size = strlen(argv[1]);
    int msg_bit_size = msg_size * 8;
    int n_blocks = get_n_blocks(msg_bit_size);

    uint256_t digest;
    for (int i = 0; i < n_blocks; i++) {
        uint512_t block = 0;
        for (int j = 0; j < msg_size; j++) { // TODO: tratar para casos com mais de um bloco
            // cout << std::bitset<8>(msg[j]) << endl;
            block = block << 8;
            block += msg[j];
        }
        block = block << 1;
        block += 1;
        block = block << (512 - msg_bit_size - 1); // TODO: confirmar o -1
        block += msg_bit_size;

        //print512bit(block);

        uint32_t message_schedule[64];
        for (int w = 15; w >= 0; w--) {
            message_schedule[w] = (uint32_t)(block & 0xffffffff);
            block = block >> 32;
        }
        for (int w = 16; w < 64; w++) {
            message_schedule[w] = l_sigma1(message_schedule[w-2]) + message_schedule[w-7] + l_sigma0(message_schedule[w-15]) + message_schedule[w-16];
            // cout << std::bitset<32>(message_schedule[w]) << endl;
        }
        // cout << std::bitset<512>(a << 500) << endl;

        uint32_t h_a = 0x6a09e667;
        uint32_t h_b = 0xbb67ae85;
        uint32_t h_c = 0x3c6ef372;
        uint32_t h_d = 0xa54ff53a;
        uint32_t h_e = 0x510e527f;
        uint32_t h_f = 0x9b05688c;
        uint32_t h_g = 0x1f83d9ab;
        uint32_t h_h = 0x5be0cd19;

        for (int w = 0; w < 64; w++) {
            uint32_t t1 = u_sigma1(h_e) + choice(h_e, h_f, h_g) + h_h + CONSTS[w] + message_schedule[w];
            uint32_t t2 = u_sigma0(h_a) + majority(h_a, h_b, h_c);
            cout << "t1:  " << std::bitset<32>(t1) << endl;
            cout << "t2:  " << std::bitset<32>(t2) << endl;
            h_h = h_g;
            h_g = h_f;
            h_f = h_e;
            h_e = h_d;
            h_d = h_c;
            h_c = h_b;
            h_b = h_a;

            h_a = t1 + t2;
            h_e = t1;
            // cout << std::bitset<32>(message_schedule[w]) << endl;
        }

        // cout << std::bitset<32>(h_a) << endl;
        // h_a += H_a;
        // h_b += H_b;
        // h_c += H_c;
        // h_d += H_d;
        // h_e += H_e;
        // h_f += H_f;
        // h_g += H_g;
        // h_h += H_h;

        // print_hex(h_a);
        // print_hex(h_b);
        // print_hex(h_c);
        // print_hex(h_d);
        // print_hex(h_e);
        // print_hex(h_f);
        // print_hex(h_g);
        // print_hex(h_h);
    }

    return 0;
}