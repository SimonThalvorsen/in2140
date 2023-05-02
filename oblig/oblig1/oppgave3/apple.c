#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int locateTheWorm(char *apple) {
    int i = 0;
    while (apple[i]){
        if(apple[i] == 'w'){
            return i;
        } i++;
    } return 0;
}


int removeWorm(char *apple) {
    int i = 0;
    int j = 0;
    while(apple[i]) {
        if(
        (apple[i] == 'w') || 
        (apple[i] == 'o') || 
        (apple[i] == 'r') || 
        (apple[i] == 'm')
        ) {
           apple[i] = ' ';
           j+= 1; }
        if (apple[i] == 'm') {return j;}
        i++;
    }

    return j;
}


int main() {

char apple[] = "                     a\n"
               "                    ppl\n"
               "                  eappl\n"
               "                 eapple\n"
               "                 apple\n"
               "                 appl\n"
               "         eappleappleappleappl\n"
               "    eappleappleappleappleapple\n"
               "   appleappleappleappleappleapple\n"
               "  appleappleappleappleappleappleappl\n"
               " eappleappleappleappleappleappleapple\n"
               "appleappleappleappleappleappleappleapp\n"
               "leappleappleappleappleappleappleapplea\n"
               "ppleappleappleappleappleappleappleappl\n"
               "eappleappleappleappleappleappleappleap\n"
               "pleappleappleappleappleappleappleapple\n"
               "appleappleappleappleappleappleappleapp\n"
               "leappleappleappleappwormpleappleapplea\n"
               " ppleappleappleappleappleappleappleap\n"
               "  pleappleappleappleappleappleapplea\n"
               "   ppleappleappleappleappleappleapp\n"
               "   leappleappleappleappleappleap\n"
               "     pleappleappleappleappleapp\n"
               "       leappleappleappleapple\n"
               "         appleappleappleapp\n";

   
   
   printf("%d\n", locateTheWorm(apple));
   printf("%d\n", removeWorm(apple));
   printf("apple: %s\n", apple);
   
    return 0;
}

