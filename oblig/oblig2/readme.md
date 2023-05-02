  - Simon Halvorsen - simontha
  - Iselin Bakke    - iselimb 
  - Rohit Jang      - rohitja 
  - Embrik Bovollen - embrikbb
  - Alex Holm       - alexsho 

### - Lese fra master\_file\_table
  - For å lese data på mft må man vite hvordan informasjonen er lagret
Hvert element(mappe/fil) er lagret som en inode, som har verdiene id(int), navn(variabel lengde), is\_dir(flag), read\_only(flag), størrelse(int), antall entries(int).
Disse verdiene leses i samme rekkefølge fra mft. Første 4 byte er en tallverdi(id), neste 4 er en tallverdi som sier hvor mange bytes som skal leses inn til navn,de neste x bytene er navnet, neste 2 bytene er om inoden er et directory og/eller read-only, neste 4 byte bestemmer størrelsen(kun filer) og de siste 4 bestemmer hvor mange oppføringer i inoden. Så leses antall oppføringer ganget størrelse på en oppføring bytes. 


### - Mener på at alt fungerer etter kravene


### - Ingen avvik fra prekoden(ish)
- Måtte kopiere over block\_allocation\_table til create\_example2 og 3.
- Ekstra funksjon i inode.c for å gjøre load\_inodes() rekursivt.


### - Tror alle testene passerer
