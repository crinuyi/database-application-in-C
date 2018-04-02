/*  Kompilacja:
      gcc -I /usr/include/postgresql -o postgres postgres.c -L /usr/include/postgresql -lpq
*/
#include "postgres.h"

int main(int argc, char const *argv[])
{
  line table;
  char temp[400];
  int howManyColumns;
  static PGconn* status;

  status = connectingDB();
  FILE* fileCSV;
  fileCSV = fopen(argv[1], "r");
  uploadFile(fileCSV);

  getTableName(table, argv[1], status);
  howManyColumns = getTableWidth(fileCSV, table);
  getFirstLine(table, howManyColumns, status);
  getLines(fileCSV, table, howManyColumns);

  closeConnection(status);
  fclose(fileCSV);
	return 0;
}
