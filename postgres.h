#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <libpq-fe.h>
#include<unistd.h>


//__________________________________________________
//STRUKTURA ZAWIERAJACE DANE Z PLIKU
typedef struct structure {
  char* name;
  int width;
  char** data;
} line;

//__________________________________________________
//LACZENIE Z BAZA DANYCH
void connectingDB(){
typedef struct structure {
  char* name;
  int width;
  char** data;
} line;
  char userName[255];
  char dbName[255];
  char* password;
  char temp[1000];

  printf("\n --Podaj nazwe bazy: " );
  scanf("%s", dbName);
  printf(" --Podaj nazwe uzytkownika: ");
  scanf("%s", userName);
  password = getpass(" --Podaj haslo: ");

  strcat(temp, "host=localhost port=5432 dbname=");
  strcat(temp, dbName);
  strcat(temp, "  user=");
  strcat(temp, userName);
  strcat(temp, " password=");
  strcat(temp, password);

  PGconn* myConnection = PQconnectdb(temp);
  if(PQstatus(myConnection) == CONNECTION_OK)
    printf("\n --Nawiazano polaczenie.\n");
  else {
    printf("\n --Brak polaczenia. Sprobuj ponownie.\n");
    exit(EXIT_FAILURE);
  }
}


//__________________________________________________
//WCZYTANIE PLIKU DO PROGRAMU
void uploadFile(FILE* fileCSV){
  if(fileCSV == NULL) {
    printf("\n --Wystapil blad przy wczytywaniu pliku!\n Nazwa pliku (z rozszerzeniem .csv) powinna wystapic po nazwie programu.\n");
    exit(EXIT_FAILURE);
  }
  else
    printf("\n --Plik został wczytany.\n");
}


//__________________________________________________
//UZUPELNIANIE STRUKTURY O NAZWE TABELI
void getTableName(line table, const char* tableName){
  char temp[1000];
  table.name = (char*)malloc(sizeof(char)*strlen(tableName)-3);
  strcpy(temp, tableName);
  for(int i=0; i<strlen(temp); i++)
    if(temp[i] == '.')
      temp[i] = '\0';
  strcpy(table.name, temp);
}


//__________________________________________________
//WYDRUK LINII
void printLine(line table, int howManyColumns){
  for(int i=0; i<howManyColumns; i++)
    printf("%10s ", table.data[i]);
  printf("\n");
}


//__________________________________________________
//ZWALNIANIE PAMIECI
void freeLine(line table, int howManyColumns){
  for(int c=0; c<howManyColumns; c++)
    free(table.data[c]);
  free(table.data);
}


//__________________________________________________
//USTALANIE SZEROKOSCI TABLICY
int getTableWidth(FILE* fileCSV, line table) {
  char temp[1000];
  fgets(temp, sizeof(temp), fileCSV);
  int howManyColumns = 0; //ilosc kolumn
  for(int a=0; a<strlen(temp); a++)
    if(temp[a] == ';' || temp[a] == '\n')
      howManyColumns++;
  return howManyColumns;
}

//__________________________________________________
//USTALANIE NAZW KOLUMN
void getFirstLine(line table, int howManyColumns) {
  char temp[1000];
  int whichString = 0; //wskazuje na to, ktory aktualnie napis kopiujemy
  int stringFirstIndex = 0; //bedzie wskazywac na poczatkowe miejsce wyrazu
  int stringLastIndex = 0; //bedzie wskazywac na aktualne miejce w napisie temp
  int counter; //licznik ilosci znakow w wyrazie
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
    table.data[whichString][counter] = '\0';
    stringFirstIndex = stringLastIndex+2;
    whichString++;
  }
  printLine(table, howManyColumns);
  freeLine(table, howManyColumns);
}


//__________________________________________________
//RESZTA WIERSZY
void getLines(FILE* fileCSV, line table, int howManyColumns){
  char temp[1000];
  while (fgets(temp, sizeof(temp), fileCSV) != NULL) {
    int whichString = 0; //wskazuje na to, ktory aktualnie napis kopiujemy
    int stringFirstIndex = 0; //bedzie wskazywac na poczatkowe miejsce wyrazu
    int stringLastIndex = 0; //bedzie wskazywac na aktualne miejce w napisie temp
    int counter; //licznik ilosci znakow w wyrazie
    table.data = (char**)malloc(sizeof(char*)*howManyColumns);
    for (int a=0; a<howManyColumns; a++) {
      counter = 0;
      for(int b=stringFirstIndex; temp[b]!=';' && temp[b]!='\n'; b++)
          counter++;
      stringLastIndex = stringFirstIndex+counter-1;
      int x=0;
      if(counter == 0) {
        table.data[whichString] = (char*)malloc(sizeof(char)*5);
        strcpy(table.data[whichString], "NULL\0");
        stringFirstIndex = stringLastIndex+2;
        whichString++;
        continue;
      }
      table.data[whichString] = (char*)malloc(sizeof(char)*(counter+1));
      for(int d=stringFirstIndex; d<=stringLastIndex; d++) {
          table.data[whichString][x] = temp[d];
        x++;
      }
      table.data[whichString][counter]='\0';
      stringFirstIndex = stringLastIndex+2;
      whichString++;
    }
    printLine(table, howManyColumns);
    freeLine(table, howManyColumns);
  }
}
