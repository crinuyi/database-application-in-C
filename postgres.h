#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <unistd.h>

char tableN[255];
int howManyColumns = 0;
char** columns;

//__________________________________________________
//STRUKTURA ZAWIERAJACE DANE Z PLIKU
typedef struct structure {
  char* name;
  int width;
  char** data;
} line;

void doSQL(PGconn* conn, char* command){
  PGresult* result;
  //printf("%s\n", command);
  result = PQexec(conn, command);
  //printf("status is: %s\n", PQresStatus(PQresultStatus(result)));
  //printf("#rows affected: %s\n", PQcmdTuples(result));
  //printf("result message: %s\n\n", PQresultErrorMessage(result));
  switch(PQresultStatus(result)) {
    case PGRES_TUPLES_OK:
    {
      int n=0, m=0;
      int nrows = PQntuples(result);
      int nfields = PQnfields(result);
      printf("number of rows returned = %d\n", nrows);
      printf("number of fields returned = %d\n", nfields);
      for(m=0; m<nrows; m++){
        for(n=0; n<nfields; n++)
          printf(" %s = %s", PQfname(result, n), PQgetvalue(result, m, n));
        printf("\n");
      }
    }
  }
  PQclear(result);
}


//____________________________________________________________________________________________________
//____________________________________________________________________________________________________
//

//__________________________________________________
//LACZENIE Z BAZA DANYCH
void connectionCheck(PGconn* myConnection){
  if(PQstatus(myConnection) == CONNECTION_OK)
    printf("\n --Nawiazano polaczenie z baza.\n");
  else {
    printf("\n --Brak polaczenia. Sprobuj ponownie.\n");
    exit(EXIT_FAILURE);
  }
}
PGconn* connectingDB(){
  char userName[255];
  char dbName[255];
  char* password;

  printf("\n --Podaj nazwe bazy: " );
  scanf("%s", dbName);
  printf(" --Podaj nazwe uzytkownika: ");
  scanf("%s", userName);
  password = getpass(" --Podaj haslo: ");

  char temp[1000];
  strcat(temp, "host=localhost port=5432 dbname=");
  strcat(temp, dbName);
  strcat(temp, " user=");
  strcat(temp, userName);
  strcat(temp, " password=");
  strcat(temp, password);

  PGconn* myConnection = PQconnectdb(temp);
  connectionCheck(myConnection);
  return myConnection;
}


//
//ODLACZANIE BAZY DANYCH
void closeConnection(PGconn* myConnection){
  PQfinish(myConnection);
  printf("\n --Polaczenie z baza zostalo zakonczone.\n\n");
}

//__________________________________________________
//WCZYTANIE PLIKU DO PROGRAMU
void uploadFile(FILE* fileCSV){
  if(fileCSV == NULL) {
    printf("\n! --Wystapil blad przy wczytywaniu pliku!\n Nazwa pliku (z rozszerzeniem .csv) powinna wystapic po nazwie programu.\n");
    exit(EXIT_FAILURE);
  }
  else
    printf("\n --Plik został wczytany do programu.\n");
}


//__________________________________________________
//[sql] UZUPELNIANIE STRUKTURY O NAZWE TABELI
void getTableName(line table, const char* tableName, PGconn* status){
  char temp[1000];
  table.name = (char*)malloc(sizeof(char)*strlen(tableName)-3);
  strcpy(temp, tableName);
  for(int i=0; i<strlen(temp); i++)
    if(temp[i] == '.')
      temp[i] = '\0';
  strcpy(table.name, temp);
  strcpy(tableN, temp);
  //--- SQL ---
  char line1[200] = "DROP TABLE IF EXISTS ";
  char line2[200] = "CREATE TABLE ";

  printf("\n --Tworzenie tabeli..\n");
  strcat(line1, table.name);
  strcat(line1, ";");
  strcat(line2, table.name);
  strcat(line2, "(ID SERIAL PRIMARY KEY);");

  if(PQstatus(status) == CONNECTION_OK) {
  doSQL(status, line1);
  doSQL(status, line2);
  }
  else {
    printf("! --Wystapil blad polaczenia podczas tworzenia tabeli.\n");
    exit(EXIT_FAILURE);
  }
}


//__________________________________________________
//WYDRUK LINII
void printLine(line table){
  for(int i=0; i<howManyColumns; i++)
    printf("%10s ", table.data[i]);
  printf("\n");
}


//__________________________________________________
//ZWALNIANIE PAMIECI
void freeLine(line table){
  for(int c=0; c<howManyColumns; c++)
    free(table.data[c]);
  free(table.data);
}

//__________________________________________________
//[sql] USTALANIE NAZW KOLUMN I SZEROKOSCI TABLICY
void getFirstLine(FILE* fileCSV, line table, PGconn* status) {
  printf(" --Dodawanie kolumn..\n");
  char temp[1000];
  fgets(temp, sizeof(temp), fileCSV);
  for(int a=0; a<strlen(temp); a++)
    if(temp[a] == ';' || temp[a] == '\n')
      howManyColumns++;

  columns = (char**)malloc(sizeof(char*)*howManyColumns);

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
    columns[whichString] = (char*)malloc(sizeof(char)*(counter+1));
    strcpy(columns[whichString], table.data[whichString]);
    columns[whichString][counter] = '\0';

    //--- SQL ---
    char line[1000] = "ALTER TABLE ";
    strcat(line, tableN);
    strcat(line, " ADD COLUMN ");
    strcat(line, table.data[whichString]);
    strcat(line, " VARCHAR(20);");
    if(PQstatus(status) == CONNECTION_OK) {
    doSQL(status, line);
    }
    else {
      printf("! --Wystapil blad polaczenia podczas dodawania kolumn.\n");
      exit(EXIT_FAILURE);
    }

    whichString++;
  }
  freeLine(table);
}


//__________________________________________________
//RESZTA WIERSZY
void getLines(FILE* fileCSV, line table, PGconn* status){
  printf(" --Wczytywanie wierszy..\n");

  int varcharValue[howManyColumns];
  for(int x=0; x<howManyColumns; x++)
    varcharValue[x] = 20;

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

      // - SQL -
      if(counter>varcharValue[whichString]) {
        varcharValue[whichString] = counter;
        char lineX[1000] = "ALTER TABLE ";
        strcat(lineX, tableN);
        strcat(lineX, " ALTER COLUMN ");
        strcat(lineX, columns[whichString]);
        strcat(lineX, " TYPE VARCHAR(");
        char tempo[10];
        sprintf(tempo, "%d", varcharValue[whichString]);
        strcat(lineX, tempo);
        strcat(lineX, ");");
        if(PQstatus(status) == CONNECTION_OK) {
        doSQL(status, lineX);
        }
        else {
          printf("! --Wystapil blad polaczenia podczas dodawania kolumn.\n");
          exit(EXIT_FAILURE);
        }
      }

      whichString++;
    }
    //--- SQL ---
    char line[1000] = "INSERT INTO ";
    strcat(line, tableN);
    strcat(line, " (");
    for(int q=0; q<howManyColumns; q++) {
      strcat(line, columns[q]);
      if(q==(howManyColumns-1))
        strcat(line, ") ");
      else
        strcat(line, ", ");
    }
    strcat(line, "VALUES (");
    for(int q=0; q<howManyColumns; q++) {
      strcat(line, "'");
      strcat(line, table.data[q]);
      if(q==(howManyColumns-1))
        strcat(line, "');");
      else
        strcat(line, "', ");
    }
    if(PQstatus(status) == CONNECTION_OK) {
    doSQL(status, line);
    }
    else {
      printf("! --Wystapil blad polaczenia podczas dodawania kolumn.\n");
      exit(EXIT_FAILURE);
    }
    freeLine(table);
  }
}

void printTable(PGconn* status) {
  printf("\n --Utworzona tabela:\n");
  char line[500] = "SELECT * FROM ";
  strcat(line, tableN);

  if(PQstatus(status) == CONNECTION_OK)
    doSQL(status, line);
  else {
    printf("! --Wystapil blad polaczenia podczas drukowania.\n");
    exit(EXIT_FAILURE);
    }
}

void creatingHTMLfile(PGconn* status){
  PGresult* result;
  FILE* HTMLfile = fopen("HTMLfile.html", "w");
  char line[500] = "SELECT * FROM ";
  strcat(line, tableN);

  if(PQstatus(status)==CONNECTION_OK) {
    result = PQexec(status, line);

    if(PQresultStatus(result)==PGRES_TUPLES_OK) {
      PQprintOpt pqp;
      pqp.header = 1;
      pqp.align = 1;
      pqp.html3 = 1;
      pqp.expanded = 0;
      pqp.pager = 0;
      pqp.fieldSep = "";
      pqp.tableOpt = "align=center";
      pqp.caption = "";
      pqp.fieldName = NULL;
      printf("<!DOCTYPE html><HTML><HEAD><STYLE> body {background-color: powderblue;} </STYLE></HEAD><BODY>\n");
      PQprint(HTMLfile, result, &pqp);
      printf("</BODY></HTML>\n");
    }
  }
  fclose(HTMLfile);
}
