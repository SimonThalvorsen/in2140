#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

int stringsum(char *sum) {
    int total_sum = 0;
    int i = 0;

    while (sum[i] != '\0') { //sjekker om strengen er ferdig
        if (sum[i] == ' ') {
            i++; //itererer videre hvis mellomrom
            continue;
        }
        if ((sum[i] >= 'a' && sum[i] <= 'z') || (sum[i] >= 'A' && sum[i] <= 'Z')) {
            total_sum += tolower(sum[i]) - 'a' + 1;
            i++;
        } else {
            return -1;
        }
    }

    return total_sum;
}


int distance_between(char* s, char c){
    int i =0;
    int startpos = -1;
    int sluttpos = -1;

    while(s[i] != '\0'){
        if (s[i] == c){
            if(startpos == -1){ //sjekker om den ikke har fått verdi
            startpos = i;
            }
            sluttpos = i; //hvis startpos har fått verdi, setter vi sluttpos til denne verdien
        } 
        i++;
    }

    if(startpos == -1){
        return -1;
    } else if (startpos == 0) {
        return 0;
    }
    return sluttpos - startpos;
}

char* string_between(char* s, char c){
    int startpos = -1;
    int sluttpos = -1;
    int i = 0;
   

    while ((s[i] != '\0')){
        if(s[i] == c){
            if(startpos == -1){
                startpos = i;
            }
            sluttpos = i;
        }
        i++;
    }
   
   if(startpos == -1 ){
    return NULL;
    } else if(startpos == sluttpos){
        return strdup("");
   }

    char *delstreng = (char*)malloc(sizeof(char) * (sluttpos - startpos +1));
   
   int j = 0;
   for(j=startpos+1; j< sluttpos; j++){
    delstreng[j-startpos-1] = s[j];
   }
   delstreng[j-startpos-1] = '\0';

   return delstreng;
    
}

int stringsum2(char* s, int* res){
    int total_sum = 0;
    int i = 0;

    while ((s[i] != '\0')) { //sjekker om strengen er ferdig
        if (s[i] == ' ') {
            i++; //itererer videre hvis mellomrom
            continue;
        }
       
        if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')) {
            total_sum += tolower(s[i]) - 'a' + 1;
            i++;
            if(isdigit(s[i])){
                *res = -1;
                return -1;
            }
            
        } else if(s[i] == '\0'){
            return -1;
        }
        else {
            return -1;
        }
    }
    *res = total_sum;
    return 0;

}



