#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define SET_BIT(num, bit) ((num) = (num) | 1 << (bit));
#define CLEAR_BIT(num, bit) ((num) = (num) & (1 << (bit) ^ 0xFFFFFFFF));
#define TEST_BIT(num, bit) ((num) & (1 << (bit)));
#define COMPLEMENT(num) ((num) = (num) ^ 0xFFFFFFFF);
#define TOGGLE_BIT(num, bit) ((num) = (num) ^ (1 << (bit)));

#define MAX_MASK_LENGTH 32          // Maximum Value for Subnet-mask
#define PREFIX_LENGTH 15            // Maximumum IPv4 len in A.B.C.D format

// Calculate total hosts possible in subnet (ONLY Class B and Class C), with given mask value. Mask 24 gives total of 254 hosts. This is also called "subnet-cardinality". 
// Return -1 if the given mask prefix falls outside of Class B and Class C prefix ranges. B (/16 -> /30)    C (/24 -> /30)
// Mask 24 = 11111111   11111111    11111111    00000000
//              ^           ^           ^           ^
//           Mask-bits   Mask-bit   Mask-bit    Control-bits
unsigned long get_max_hosts_in_subnet(unsigned char given_mask){

    if(given_mask > 30 || given_mask < 16){
        printf("Error: Given mask-prefix falls outside of the range for Class B and/or Class C IP-addresses. Exiting function. No Calculation performed\n");
        return -1;
    }

    unsigned long max_hosts;
    max_hosts = pow(2, (MAX_MASK_LENGTH - given_mask)) - 2;
    return max_hosts;
}

// Calculate total subnets possible- (ONLY Class B and Class C), with given mask value. Mask 24 gives total of 254 subnetts. 
// Return -1 if the given mask prefix falls outside of Class B and Class C prefix ranges. B (/16 -> /30)    C (/24 -> /30)
unsigned long get_max_subnets(unsigned char given_mask){
    unsigned long max_subnets;

    if(given_mask > 30 || given_mask < 16){
        printf("Error: Given mask-prefix falls outside of the range for Class B and/or Class C IP-addresses. Exiting function. No Calculation performed\n");
        return -1;
    }
    
    if (given_mask == 16){
        max_subnets = 0;
    } else if(given_mask < 24){
        max_subnets = pow(2, (given_mask - 16));
    } else if (given_mask > 24){
        max_subnets = pow(2, (given_mask - 24));
    } else {
        max_subnets = 0;
    }

    return max_subnets;
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


// Get IP-format in A.B.C.D from integer format. Return IP in A.B.C.D-format
char* get_abcd_ip_format(unsigned long ip_address, char* output_buffer){
    unsigned char ip_bytes[4];
    ip_bytes[3] = (ip_address & 0xFF000000) >> 24;      // A
    ip_bytes[2] = (ip_address & 0xFF0000) >> 16;        // B
    ip_bytes[1] = (ip_address & 0xFF00) >> 8;           // C
    ip_bytes[0] = (ip_address & 0xFF);                  // D

    sprintf(output_buffer, "%d.%d.%d.%d", ip_bytes[3], ip_bytes[2], ip_bytes[1], ip_bytes[0]);
    return output_buffer;
}


// Get current network ID, given IP-adress in format (A.B.C.D) and mask value in integer format. Return Network ID in integer format.
unsigned long get_network_id(char* ip_address, unsigned char given_mask, char* output_buffer){

    unsigned long network_mask = get_mask_value_in_integer_format(given_mask);
    unsigned long network_ip = get_ip_integer_equivalent(ip_address);
    unsigned long network_id = network_ip & network_mask;
   
    get_abcd_ip_format(network_id, output_buffer);
    return network_id;
}

// Get current network broadcast address, given IP-address in format (A.B.C.D) and mask value in integer format. Return network broadcast in integer format
unsigned long get_network_broadcast(char* ip_address, unsigned char given_mask, char* output_buffer){

    unsigned long network_mask = get_mask_value_in_integer_format(given_mask);
    unsigned long network_ip = get_ip_integer_equivalent(ip_address);
    unsigned long network_id = network_ip & network_mask;

    unsigned long network_broadcast = network_id | COMPLEMENT(network_mask);
    get_abcd_ip_format(network_broadcast, output_buffer);
    return network_broadcast;
}

// Get subnet-mask in A.B.C.D-format, given mask-value in integer format. Return subnet-mask in integer format
unsigned long get_subnet_mask(unsigned char given_mask, char* output_buffer){
    unsigned long network_mask = get_mask_value_in_integer_format(given_mask);
    get_abcd_ip_format(network_mask, output_buffer);
    return network_mask;
}

// Check if the given IP (ip-address 2) integer value is equal the given IP (ip-address 1) Network-ID integer value. If true, Return 0, else Return -1 
// The given network-ID and given IP-addressers are in in A.B.C.D-format.
unsigned long check_ip_subnet_membership(char* network_ip1, unsigned char network_ip1_mask, char* network_ip2, unsigned char network_ip2_mask, char* output_buffer){
    unsigned long network_id1 = get_network_id(network_ip1, network_ip1_mask, output_buffer);
    unsigned long network_id2 = get_network_id(network_ip2, network_ip2_mask, output_buffer);

    if(network_ip1_mask != network_ip2_mask){
        printf("\n");
        printf("Warning: There are different masks in this configuration. Communication is theoreticlly possible as long as: \n");
        if(network_ip1_mask > network_ip2_mask){
            printf("%s tries to connect to %s. Only because the subnet-mask of %s (%d) is smaller %s (%d). But since it's bad configuration. Communication will not work both ways\n", network_ip2, network_ip1, network_ip2, network_ip2_mask, network_ip1, network_ip1_mask);
        } else {
            printf("%s tries to connect to %s. Only because the subnet-mask of %s (%d) is smaller %s (%d). But since it's bad configuration. Communication will not work both ways\n", network_ip1, network_ip2, network_ip1, network_ip1_mask, network_ip2, network_ip2_mask);
        }

        printf("\n");
    }

    if(network_id1 == network_id2){
        printf("The given IP-address %s (ID = %s/%d), is in the same subnet as %s (ID = %s/%d)\n", network_ip2, get_abcd_ip_format(network_id2, output_buffer), network_ip2_mask, network_ip1, get_abcd_ip_format(network_id1, output_buffer), network_ip1_mask);
        return 0;
    }
    else {
        printf("The given IP-address %s (ID = %s/%d), is NOT in the same subnet as %s (ID = %s/%d)\n", network_ip2, get_abcd_ip_format(network_id2, output_buffer), network_ip2_mask, network_ip1, get_abcd_ip_format(network_id1, output_buffer), network_ip1_mask);
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

    char buffer[100];                           // User input buffer
    char continue_with_program;                 // Value to check if user wants to continue with the program.
    char output_buffer[PREFIX_LENGTH+1] = {0};
    unsigned char ip_bytes[4];
    char* network_broadcast;
    unsigned char subnet_mask;
    char* ip_address;

    while (1){
        printf("\n");
        printf("Enter IP-configuration....\n");
        
        printf("IP-address in format A.B.C.D: ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%hhu.%hhu.%hhu.%hhu", &ip_bytes[3], &ip_bytes[2], &ip_bytes[1], &ip_bytes[0]);

        // Must use this to get the correct A.B.C.D-format for the functions below.
        sprintf(output_buffer, "%d.%d.%d.%d", ip_bytes[3], ip_bytes[2], ip_bytes[1], ip_bytes[0]);
        ip_address = output_buffer;

        printf("Subnet prefix (ex. /24 = 255.255.255.0): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%hhu", &subnet_mask);

        get_network_id(ip_address, subnet_mask, output_buffer);
        printf("Network ID: %s/%d\n", output_buffer, subnet_mask);
        get_network_broadcast(ip_address, subnet_mask, output_buffer);
        printf("Network Broadcast: %s\n", output_buffer);
        unsigned long total_amount_of_hosts = get_max_hosts_in_subnet(subnet_mask);
        printf("Total amount of hosts: %d\n", total_amount_of_hosts);
        unsigned long total_amount_of_subnets = get_max_subnets(subnet_mask);
        printf("Total amount of subnets: %d\n", total_amount_of_subnets);

        printf("\nDo you want to continue? (y/n): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%c", &continue_with_program);
        if(continue_with_program == 'y' || continue_with_program == 'Y'){
            continue;
        } else {
            break;
        }
    }
 


    return 0;
}