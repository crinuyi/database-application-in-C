/*  Kompilacja:
      gcc -I /usr/include/postgresql -o postgres postgres.c -L /usr/include/postgresql -lpq
*/
#include "postgres.h"

int main(int argc, char const *argv[])
{
  line table;
  char temp[400];
  char yn[3];
  static PGconn* status;

  status = connectingDB();
  FILE* fileCSV;
  fileCSV = fopen(argv[1], "r");
  uploadFile(fileCSV);

  getTableName(table, argv[1], status);
  getFirstLine(fileCSV, table, status);
	getLines(fileCSV, table, status);

  printf("\n --Czy chcesz wydrukowac tabele (T/N)? \n");
  fflush(stdin);
  scanf("%s", yn);
  if(yn[0] == 84)
    printTable(status);
  else printf("\n --Drukowanie zostało anulowane.\n\n");

  printf("\n --Czy chcesz utworzyc plik HTML (T/N)? ");
  fflush(stdin);
  scanf("%s", yn);
  if(yn[0] == 84)
    creatingHTMLfile(status);
  else printf("\n --Tworzenie pliku zostało anulowane.\n");

  closeConnection(status);
  fclose(fileCSV);
	return 0;
}
