#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_SIZE 256
#define BATCH_SIZE 100 // Number of bytes to sample per print iteration

// Structure to help us sort the urandom bytes by their actual frequency
typedef struct {
    unsigned char byte_value;
    unsigned long frequency;
} ByteFreq;

// Comparison function for qsort (descending order)
int compare_frequencies(const void *a, const void *b) {
    long freq_a = ((ByteFreq*)a)->frequency;
    long freq_b = ((ByteFreq*)b)->frequency;
    return (freq_b - freq_a); 
}

int main() {
    // Your pre-sorted ASCII table (most common target symbols first)
    const char frequency_sorted_ascii[ASCII_SIZE] = 
        " etaoinsrhldcumfpgwybvkxjqz"
        "ETAOINSRHLDCUMFPGWYBVKXJQZ"
        "0123456789_.,!?-()\"';:/\n\t"
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x0b\x0c\x0d\x0e\x0f"
        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
        "\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
        "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
        "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
        "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
        "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
        "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
        "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
        "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    FILE *urandom = fopen("/dev/urandom", "r");
    if (urandom == NULL) {
        perror("Failed to open /dev/urandom");
        return EXIT_FAILURE;
    }

    // Array to track the live frequencies of all 256 possible bytes
    ByteFreq tracked_bytes[ASCII_SIZE];

    // Infinite loop
    while (1) {
        // Reset our frequency tracker for this batch
        for (int i = 0; i < ASCII_SIZE; i++) {
            tracked_bytes[i].byte_value = (unsigned char)i;
            tracked_bytes[i].frequency = 0;
        }

        // Read a distinct batch of bytes to find a local "winner"
        for (int i = 0; i < BATCH_SIZE; i++) {
            int input_byte = fgetc(urandom);
            if (input_byte == EOF) break;
            tracked_bytes[input_byte].frequency++;
        }

        // Sort the urandom bytes based on how often they just appeared
        qsort(tracked_bytes, ASCII_SIZE, sizeof(ByteFreq), compare_frequencies);

        // tracked_bytes[0] is now the absolute most common byte from this /dev/urandom sample.
        // We look up its value, and use that index to print from your pre-sorted ASCII table.
        unsigned char most_common_idx = tracked_bytes[0].byte_value;
        char mapped_char = frequency_sorted_ascii[most_common_idx];

        // Print only the character, spaced by a space
        printf("%c ", mapped_char);
        fflush(stdout); 
    }

    fclose(urandom);
    return EXIT_SUCCESS;
}
