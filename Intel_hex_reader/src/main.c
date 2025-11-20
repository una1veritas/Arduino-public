/*
 * main.c
 *
 *  Created on: 2025/10/20
 *      Author: sin
 */
/*
 * AI answer on Google generated code
 * all bugs are corrected.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to convert a hex string length up to 4 to an integer
int hex_to_int(char *hex_str, int len) {
    char temp[5];
    strncpy(temp, hex_str, len);
    temp[len] = (char) 0;
    //printf("%s, ", temp);
    return (int)strtol(temp, NULL, 16);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <intel_hex_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[256]; // Assuming max line length
    uint32_t base_address = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
    	if (strlen(line) == 0) {
    		fprintf(stderr,"empty line\n");
    		continue;
    	}
        if (line[0] != ':') {
            fprintf(stderr, "Invalid Intel HEX format: Missing the line start symbol ':'\n");
            continue;
        }
        int byte_count = hex_to_int(&line[1], 2);
        uint32_t address = hex_to_int(&line[3], 4);
        int record_type = hex_to_int(&line[7], 2);
        uint8_t chksum = 0;
        uint8_t xsum = byte_count + ((uint8_t *) &address)[0] +  ((uint8_t *) &address)[1] + record_type;


        // Calculate absolute address
        uint32_t current_address = base_address + address;

        // Process based on record type
        if (record_type == 0x00) {
        	// Data
            printf("Data Record: 0x%04X + %0x = \n", current_address, byte_count);
            for (int i = 0; i < byte_count; ++i) {
                int data_byte = hex_to_int(&line[9 + (i << 1)], 2);
                printf("%02X ", data_byte);
                xsum += data_byte;
                if (i == 15) printf("\n");
                // Store data_byte at current_address + i in your memory structure
            }
            chksum = hex_to_int(&line[9+(byte_count<<1)], 2);
        } else if (record_type == 0x01) {
        	// End of File
        	chksum = hex_to_int(&line[9], 2);
            printf("End of File Record.\n");
            break;
        } else if (record_type == 0x02) {
        	// Extended Segment Address
        	continue;
        } else if (record_type == 0x03) {
        	// Start Segment Address
        	continue;
        } else if (record_type == 0x04) {
        	// Extended Linear Address
            uint32_t xaddress_msw = hex_to_int(&line[9], 4);
            xsum += ((uint8_t *) &xaddress_msw)[0] + ((uint8_t *) &xaddress_msw)[1];
            base_address |= (unsigned int) xaddress_msw << 16;
            chksum = hex_to_int(&line[13], 2);
            printf("Extended Linear Address: 0x%08X\n", base_address);
        } else if (record_type == 0x05) {
        	// Start Linear Address
        	continue;
        }
        // Add handling for other record types if needed

        // Checksum verification can be added here
        printf(": %02x, %02x\n", chksum, (xsum^0xff)+1);
    }
    fclose(fp);
    fprintf(stdout, "finished.\n");
    return 0;
}
