#include "postgres.h"

typedef struct structure {
  char* name;
  int width;
  char** data;
} line;

int main(int argc, char const *argv[])
{
  line table;
	//__________________________________________________
	//WCZYTANIE PLIKU DO PROGRAMU
	FILE* fileCSV; //plik podawany przy uruchamianiu programu
	fileCSV = fopen(argv[1], "r");
	char temp[400];
	if(fileCSV == NULL) {
		printf(" Wystapil blad przy wczytywaniu pliku!\n Nazwa pliku (z rozszerzeniem .csv) powinna wystapic po nazwie programu.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf(" Plik został wczytany.\n");


	//__________________________________________________
	//UZUPELNIANIE STRUKTURY O NAZWE TABELI
	table.name = (char*)malloc(sizeof(char)*strlen(argv[1])-3);
	strcpy(temp, argv[1]);
	for(int i=0; i<strlen(temp); i++)
		if(temp[i] == '.')
			temp[i] = '\0';
	strcpy(table.name, temp);

	//__________________________________________________
	//USTALANIE SZEROKOSCI TABLICY
	fgets(temp, sizeof(temp), fileCSV);
  int whichString = 0; //wskazuje na to, ktory aktualnie napis kopiujemy
  int stringFirstIndex = 0; //bedzie wskazywac na poczatkowe miejsce wyrazu
  int stringLastIndex = 0; //bedzie wskazywac na aktualne miejce w napisie temp
  int howManyColumns = 0; //ilosc kolumn
  int counter; //licznik ilosci znakow w wyrazie
  for(int a=0; a<strlen(temp); a++)
    if(temp[a] == ';' || temp[a] == '\n')
      howManyColumns++;
  table.data = (char**)malloc(sizeof(char*)*howManyColumns);
  for(int b=0; b<howManyColumns; b++) {
		counter = 0;
    for(int n=stringFirstIndex; temp[n] != ';' && temp[n] != '\n'; n++)
			counter++;
    stringLastIndex = stringFirstIndex+counter-1;
    int m = 0;
    table.data[whichString] = (char*)malloc(sizeof(char)*(counter+1));
    for(int x=stringFirstIndex; x<=stringLastIndex; x++) {
      table.data[whichString][m] = temp[x];
      m++;
    }
    table.data[whichString][stringLastIndex+1] = '\0';
    stringFirstIndex = stringLastIndex+2;
    whichString++;
	}

	//__________________________________________________
	//TEST NAGLOWKA
    for(int i=0; i<howManyColumns; i++)
      printf("%s ", table.data[i]);
    printf("\n");

    //zwalnianie pamieci
    for(int c=0; c<howManyColumns; c++)
      free(table.data[c]);
    free(table.data);


    //__________________________________________________
  	//RESZTA WIERSZY
    while (fgets(temp, sizeof(temp), fileCSV) != NULL) {
      int whichString = 0; //wskazuje na to, ktory aktualnie napis kopiujemy
      int stringFirstIndex = 0; //bedzie wskazywac na poczatkowe miejsce wyrazu
      int stringLastIndex = 0; //bedzie wskazywac na aktualne miejce w napisie temp
      int howManyColumns = 0; //ilosc kolumn
      int counter; //licznik ilosci znakow w wyrazie
      for(int a=0; a<strlen(temp); a++)
        if(temp[a] == ';' || temp[a] == '\n')
          howManyColumns++;
      table.data = (char**)malloc(sizeof(char*)*howManyColumns);
      for(int b=0; b<howManyColumns; b++) {
        counter = 0;
        for(int n=stringFirstIndex; temp[n] != ';' && temp[n] != '\n'; n++)
          counter++;
        stringLastIndex = stringFirstIndex+counter-1;
        int m = 0;
        table.data[whichString] = (char*)malloc(sizeof(char)*(counter+1));
        for(int x=stringFirstIndex; x<=stringLastIndex; x++) {
          table.data[whichString][m] = temp[x];
          m++;
        }
        table.data[whichString][stringLastIndex+1] = '\0';
        stringFirstIndex = stringLastIndex+2;
        whichString++;
      }

      for(int i=0; i<howManyColumns; i++)
        printf("%s ", table.data[i]);
      printf("\n");

      //zwalnianie pamieci
      for(int c=0; c<howManyColumns; c++)
        free(table.data[c]);
      free(table.data);
    }

  fclose(fileCSV);
	return 0;
}
