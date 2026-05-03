#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

int main(int argc, char **argv) {
        std::cerr << "TARGET START\n";
        char ptr[20];
        if(argc>1){
                FILE *fp = fopen(argv[1], "r");
                fgets(ptr, sizeof(ptr), fp);
        }
        else{
                fgets(ptr, sizeof(ptr), stdin);
        }
        printf("%s", ptr);
        if(ptr[0] == 'd') {
                if(ptr[1] == 'e') {
                        if(ptr[2] == 'a') {
                                if(ptr[3] == 'd') {
                                        if(ptr[4] == 'b') {
                                                if(ptr[5] == 'e') {
                                                        if(ptr[6] == 'e') {
                                                                if(ptr[7] == 'f') {
                                                                        abort();
                                                                }
                                                                else    printf("%c",ptr[7]);
                                                        }
                                                        else    printf("%c",ptr[6]);
                                                }
                                                else    printf("%c",ptr[5]);
                                        }
                                        else    printf("%c",ptr[4]);
                                }
                                else    printf("%c",ptr[3]);
                        }
                        else    printf("%c",ptr[2]);
                }
                else    printf("%c",ptr[1]);
        }
        else    printf("%c",ptr[0]);
	char secret[] = "abbacd\x0a";
	if(strcmp(ptr,secret)== 0){
			abort();
	}
	
	return 0;
}