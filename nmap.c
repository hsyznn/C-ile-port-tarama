#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int validate_ip(const char *ip_address) {
    
    const char *ip_regex = "^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$";
    regex_t regex;
    int ret;

    
    ret = regcomp(&regex, ip_regex, REG_EXTENDED);
    if (ret) {
        fprintf(stderr, "Could not compile regex\n");
        return 0;
    }


    ret = regexec(&regex, ip_address, 0, NULL, 0);
    regfree(&regex);

    if (!ret) {
        return 1;  
    } else {
        return 0;  
    }
}

void execute_nmap(const char *ip_address) {
    char command[256];
    char buffer[1024];
    FILE *fp;

    
    snprintf(command, sizeof(command), "nmap -T4 -p- %s --max-retries 4", ip_address);

    
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run nmap command\n");
        exit(1);
    }

    
    char open_ports[1024] = "";

   
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "open") != NULL) {
           
            char *port = strtok(buffer, "/");
            strcat(open_ports, port);
            strcat(open_ports, ",");
        }
    }
    pclose(fp);

   
    if (strlen(open_ports) > 0 && open_ports[strlen(open_ports) - 1] == ',') {
        open_ports[strlen(open_ports) - 1] = '\0';
    }

    
    snprintf(command, sizeof(command), "nmap -A -Pn -sV -sC -p %s %s", open_ports, ip_address);
    system(command);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: Give a valid IP address as an argument\n");
        return 1;
    }

    const char *ip_address = argv[1];

    if (validate_ip(ip_address)) {
        execute_nmap(ip_address);
    } else {
        printf("Error: Give a valid IP address as an argument\n");
    }

    return 0;
}

