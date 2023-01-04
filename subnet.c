#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SET_BIT(num, bit) ((num) = (num) | 1 << (bit));
#define CLEAR_BIT(num, bit) ((num) = (num) & (1 << (bit) ^ 0xFFFFFFFF));
#define TEST_BIT(num, bit) ((num) & (1 << (bit)));
#define COMPLEMENT(num) ((num) = (num) ^ 0xFFFFFFFF);
#define TOGGLE_BIT(num, bit) ((num) = (num) ^ (1 << (bit)));

#define MAX_MASK_LENGTH 32          // Maximum Value for Subnet-mask
#define PREFIX_LENGTH 15            // Maximumum IPv4 len in A.B.C.D format

// Calculate total hosts possible in subnet, with given mask value. Mask 24 gives total of 254 hosts. This is also called "subnet-cardinality"
// Mask 24 = 11111111   11111111    11111111    00000000
//              ^           ^           ^           ^
//           Mask-bits   Mask-bit   Mask-bit    Control-bits
unsigned long get_max_hosts_in_subnet(unsigned char given_mask){
    unsigned long max_hosts;
    max_hosts = pow(2, (MAX_MASK_LENGTH - given_mask)) - 2;
    return max_hosts;
}

// Caluclate the mask value in integer form. If mask value = 24, the function should return an unsigned integer equivalent to 
// 11111111 11111111 11111111 00000000 (4294967040)
unsigned long get_mask_value_in_integer_format(unsigned char given_mask){

        unsigned long mask_value = 0xFFFFFFFF;
        unsigned char n_trail_bits = MAX_MASK_LENGTH - given_mask;
        for(int i = 0; i < n_trail_bits; i++){
            CLEAR_BIT(mask_value, i);
        }
        return mask_value;
}

// Get integer format of given IP-address from A.B.C.D format. Return Integer value of IP-address
unsigned long get_ip_integer_equivalent(char *ip_address){
    
    unsigned char ip_bytes[4];
    sscanf(ip_address, "%hhu.%hhu.%hhu.%hhu", &ip_bytes[3], &ip_bytes[2], &ip_bytes[1], &ip_bytes[0]);
    return ip_bytes[0] | ip_bytes[1] << 8 | ip_bytes[2] << 16 | ip_bytes[3] << 24;
}


// Get IP-format in A.B.C.D from integer format
char* get_abcd_ip_format(unsigned long ip_address, char* output_buffer){
    
    unsigned char ip_bytes[4];

    ip_bytes[3] = (ip_address & 0xFF000000) >> 24;      // A
    ip_bytes[2] = (ip_address & 0xFF0000) >> 16;        // B
    ip_bytes[1] = (ip_address & 0xFF00) >> 8;           // C
    ip_bytes[0] = (ip_address & 0xFF);                  // D

    sprintf(output_buffer, "%d.%d.%d.%d", ip_bytes[3], ip_bytes[2], ip_bytes[1], ip_bytes[0]);
    return output_buffer;
}


// Get current network ID, given IP-adress in format (A.B.C.D) and mask value in integer format. Return a string in A.B.C.D format
char* get_network_id(char* ip_address, unsigned char given_mask, char* output_buffer){

    unsigned long network_mask = get_mask_value_in_integer_format(given_mask);
    unsigned long network_ip = get_ip_integer_equivalent(ip_address);
    unsigned long network_id = network_ip & network_mask;
   
    return get_abcd_ip_format(network_id, output_buffer);
}


// Get current network broadcast address, given IP-address in format (A.B.C.D) and mask value in integer format. Return a string in A.B.C.D format
char* get_network_broadcast(char* ip_address, unsigned char given_mask, char* output_buffer){

    unsigned long network_mask = get_mask_value_in_integer_format(given_mask);
    unsigned long network_ip = get_ip_integer_equivalent(ip_address);
    unsigned long network_id = network_ip & network_mask;

    unsigned long network_broadcast = network_id | COMPLEMENT(network_mask);

    return get_abcd_ip_format(network_broadcast, output_buffer);
}

// Get subnet-mask in A.B.C.D-format, given mask-value in integer format. Return string in A.B.C.D format
char* get_subnet_mask(unsigned char given_mask, char* output_buffer){
    unsigned long network_mask = get_mask_value_in_integer_format(given_mask);
    return get_abcd_ip_format(network_mask, output_buffer);
}

// Check if the given IP integer value is equal the given network-id integer value. If true, Return 0, else Return -1 
// The given network-ID and given IP-addressers are in in A.B.C.D-format.
int check_ip_subnet_membership(char* network_id, unsigned char mask, char* ip_address){
    unsigned long network_id_address = get_ip_integer_equivalent(network_id);
    unsigned long check_ip_address = get_ip_integer_equivalent(ip_address);
    unsigned long network_mask = get_mask_value_in_integer_format(mask);
    unsigned long check_address = check_ip_address & network_mask;

    if(check_address == network_id_address){
        printf("The given IP-address %s, is in the same subnet as given Network ID %s/%d\n", ip_address, network_id, mask);
        return 0;
    }
    else {
        printf("The given IP-address %s, is NOT in the same subnet as given Network ID %s/%d\n", ip_address, network_id, mask);
        return -1;
    }
    
}

// Print out the bits set in the integer
void print_set_bits(unsigned int u_integer){
    int temp = 0;
    int is_set;
    for(int i = 0; i < MAX_MASK_LENGTH; i++ ){
        temp = TEST_BIT(u_integer, i);
        is_set = 0;
        if(i % 8 == 0){
            printf("\n");
        }
        if(temp > 0){
            is_set = 1;
        }

        printf("bit_%-3d: %-3d", i, is_set);
    }    
    printf("\n\n");
}


int main(int argc, char* argv[]){

    int mask = 24;
    unsigned long mask_value;
    char output_buffer[PREFIX_LENGTH+1] = {0};
    char* given_ip = "192.168.2.10";
    char* given_network_id = "192.168.2.0";

    /* Test Conditions */
    /* Q1. Get broadcast address */
    /* Q3. Get A.B.C.D IP Format */
    /* Q4. Get Network ID */
    /* Q5. Get Subnet-cardinality / Max number of hosts */

    printf("------------- Q1, Q3, Q4, Q5 -------------\n");
    printf("\n");
    printf("Test case 1: \n");
    printf("Given mask: %d, Max number of hosts: %lu\n", mask, get_max_hosts_in_subnet(mask));
    printf("Given ip-address: %s\n", given_ip);
    given_network_id = get_network_id(given_ip, mask, output_buffer);
    printf("Network ID: %s/%d\n", given_network_id, mask);
    printf("Network Broadcast: %s\n", get_network_broadcast(given_network_id, mask, output_buffer));
    printf("Subnet-mask: %s\n", get_subnet_mask(mask, output_buffer));
    printf("\n");

    given_ip = "10.1.23.10";
    mask = 20;

    printf("\n");
    printf("Test case 2: \n");
    printf("Given mask: %d, Max number of hosts: %lu\n", mask, get_max_hosts_in_subnet(mask));
    printf("Given ip-address: %s\n", given_ip);
    given_network_id = get_network_id(given_ip, mask, output_buffer);
    printf("Network ID: %s/%d\n", given_network_id, mask);
    printf("Network Broadcast: %s\n", get_network_broadcast(given_network_id, mask, output_buffer));
    printf("Subnet-mask: %s\n", get_subnet_mask(mask, output_buffer));
    printf("\n");

    /* Q2. Get IP integer equivalent */
    given_ip = "192.168.2.10";

    printf("------------- Q2 -------------\n");
    printf("\n");
    printf("Test case 1: \n");
    printf("Given ip-address: %s\n", given_ip);
    printf("Integer Equivalent: %lu\n", get_ip_integer_equivalent(given_ip));
    printf("\n");

    given_ip = "10.1.23.10";

    printf("\n");
    printf("Test case 2: \n");
    printf("Given ip-address: %s\n", given_ip);
    printf("Integer Equivalent: %lu\n", get_ip_integer_equivalent(given_ip));

    printf("\n");

    /* Q6. Get IP subnet membership */
    printf("------------- Q6 -------------\n");
    given_network_id = "192.168.2.0";
    given_ip = "192.168.2.13";
    mask = 24;
    printf("\n");
    printf("Test case 1: \n");
    printf("Given network-id: %s/%d\n", given_network_id, mask);
    printf("Given ip-address: %s\n", given_ip);
    printf("Network Broadcast: %s\n", get_network_broadcast(given_network_id, mask, output_buffer));
    printf("Subnet-mask: %s\n", get_subnet_mask(mask, output_buffer));
    check_ip_subnet_membership(given_network_id, mask, given_ip);
    printf("\n");

    return 0;
}