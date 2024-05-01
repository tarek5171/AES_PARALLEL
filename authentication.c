#include <stdio.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <time.h>

uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;  // Perform modulus operation on the base initially
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

int main() {
    // Prime number and generator
    uint64_t prime = 100003;
    uint64_t generator = 10453;

    // Alice's private random number
    uint64_t alice_private = 151111;
    // Bob's private random number
    uint64_t bob_private = 131111;

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    // Alice's calculation
    uint64_t alice_public = mod_exp(generator, alice_private, prime);
    // Bob's calculation
    uint64_t bob_public = mod_exp(generator, bob_private, prime);

    // Alice computes the shared secret key
    uint64_t alice_shared_secret = mod_exp(bob_public, alice_private, prime);
    // Bob computes the shared secret key
    uint64_t bob_shared_secret = mod_exp(alice_public, bob_private, prime);

    // Print shared keys
    printf("Alice's shared secret key: %llu\n", (unsigned long long)alice_shared_secret);
printf("Bob's shared secret key: %llu\n", (unsigned long long)bob_shared_secret);

    // Convert shared secrets to byte arrays
    uint8_t alice_shared_secret_bytes[8];
    uint8_t bob_shared_secret_bytes[8];
    for (int i = 0; i < 8; i++) {
        alice_shared_secret_bytes[i] = (alice_shared_secret >> (i * 8)) & 0xFF;
        bob_shared_secret_bytes[i] = (bob_shared_secret >> (i * 8)) & 0xFF;
    }

    // Concatenate Alice's and Bob's shared secrets
    uint8_t combined_secret[16];
    for (int i = 0; i < 8; i++) {
        combined_secret[i] = alice_shared_secret_bytes[i];
        combined_secret[i + 8] = bob_shared_secret_bytes[i];
    }

    // Calculate SHA-256 hash of combined secret to get 128-bit key
    uint8_t aes_key[32];
    SHA256(combined_secret, sizeof(combined_secret), aes_key);

    // Output AES key
    printf("AES Key: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", aes_key[i]);
    }
    printf("\n");

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", cpu_time_used);

    return 0;
}

