#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#include "components/header.c"

#define MAX_PRODUCTS 250        // Define o número máximo de produtos que podem ser armazenados no inventário.
#define ALERT_QUANTITY 20       // Define o limite para a quantidade de produtos no inventário, que se for abaixo desse limite vai emitir um alerta.
#define EXPIRY_ALERT_DAYS 20    // Define o número de dias antes da data de validade acabar de um produto para que um alerta seja emitido.
#define FILENAME "products.txt" // Define o nome do ficheiro onde os dados dos produtos serão armazenados.

typedef struct
{
  char name[100];
  int quantity;
  struct tm expiry_date;
} Product;

Product products[MAX_PRODUCTS];
int product_count = 0;

void save_products_to_file()
{
  FILE *file = fopen(FILENAME, "w");
  if (file == NULL)
  {
    printf("Erro ao abrir o ficheiro .\n");
    return;
  }

  for (int i = 0; i < product_count; i++)
  {

    fprintf(file, " _______      ___   .___________. __    __   .______          ___      \n");
    fprintf(file, "|   ____|    /   \\  |           ||  |  |  |  |   _  \\        /   \\     \n");
    fprintf(file, "|  |__      /  ^  \\ `---|  |----`|  |  |  |  |  |_)  |      /  ^  \\    \n");
    fprintf(file, "|   __|    /  /_\\  \\    |  |     |  |  |  |  |      /      /  /_\\  \\   \n");
    fprintf(file, "|  |      /  _____  \\   |  |     |  `--'  |  |  |\\  \\----./  _____  \\  \n");
    fprintf(file, "|__|     /__/     \\__\\  |__|      \\______/   |_| `._____/__/     \\__\\ \n");
    fprintf(file, "                                                                      \n");

    fprintf(file, "produto: %s \n", products[i].name);
    fprintf(file, "quantidade disponivel: %d\n", products[i].quantity);
    fprintf(file, "data de validade: %02d.%02d.%d\n", products[i].expiry_date.tm_mday, products[i].expiry_date.tm_mon + 1, products[i].expiry_date.tm_year + 1900);
  }

  fclose(file);
}

void load_products_from_file()
{
  FILE *file = fopen(FILENAME, "r");
  if (file == NULL)
  {
    printf("Erro ao abrir o ficheiro\n");
    return;
  }

  product_count = 0;
  while (fscanf(file, "%s %d %d.%d.%d", products[product_count].name, &products[product_count].quantity, &products[product_count].expiry_date.tm_mday, &products[product_count].expiry_date.tm_mon, &products[product_count].expiry_date.tm_year) != EOF)
  {
    products[product_count].expiry_date.tm_mon -= 1;
    products[product_count].expiry_date.tm_year -= 1900;
    product_count++;
  }

  fclose(file);
}

void add_product()
{

  if (product_count >= MAX_PRODUCTS)
  {
    printf("O seu armazenamento está cheio, não será preciso encomendar mais produtos\n");
    return;
  }

  printf("Nome do produto: ");
  scanf("%s", products[product_count].name);

  printf("Quantidade: ");
  scanf("%d", &products[product_count].quantity);

  printf("Validade (dd.mm.yyyy): ");
  int day, month, year;
  scanf("%d.%d.%d", &day, &month, &year);

  products[product_count].expiry_date.tm_mday = day;
  products[product_count].expiry_date.tm_mon = month - 1;    // Months are 0-11 in struct tm
  products[product_count].expiry_date.tm_year = year - 1900; // Years since 1900 in struct tm

  product_count++;
  save_products_to_file();
}

void check_inventory()
{
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);

  for (int i = 0; i < product_count; i++)
  {
    if (products[i].quantity <= ALERT_QUANTITY)
    {
      printf("ALERTA: Só há %d unidades de %s. Encomende mais!\n", products[i].quantity, products[i].name);
    }

    double seconds = difftime(mktime(&products[i].expiry_date), now);
    int days = seconds / (60 * 60 * 24);

    if (days <= EXPIRY_ALERT_DAYS)
    {
      printf("ALERTA: O prazo de validade de %s está próximo (%02d.%02d.%d). Recomenda-se aplicar desconto!\n",
             products[i].name,
             products[i].expiry_date.tm_mday,
             products[i].expiry_date.tm_mon + 1,
             products[i].expiry_date.tm_year + 1900);
    }

    int days_until_expiry = (int)(difftime(mktime(&products[i].expiry_date), now) / (60 * 60 * 24));
    if (days_until_expiry == 15)
    {
      printf("ALERTA: O produto %s tem 15 dias restantes até o vencimento.\n", products[i].name);
    }
  }
}

void sell_product()
{
  char name[100];
  int quantity;

  printf("Nome do produto vendido: ");
  scanf("%s", name);

  printf("Quantidade vendida: ");
  scanf("%d", &quantity);

  for (int i = 0; i < product_count; i++)
  {
    if (strcmp(products[i].name, name) == 0)
    {
      if (products[i].quantity >= quantity)
      {
        products[i].quantity -= quantity;
        printf("%d unidades de %s vendidas. Restam %d unidades.\n", quantity, name, products[i].quantity);
        save_products_to_file();
        return;
      }
      else
      {
        printf("Quantidade insuficiente de %s. Disponível: %d unidades.\n", name, products[i].quantity);
        return;
      }
    }
  }

  printf("Produto %s não encontrado.\n", name);
}

int main()
{
  system("chcp 65001");
  system("cls");

  load_products_from_file();
  int choice;

  while (1)
  {
    header();
    printf("\nMenu:\n");
    printf("1. Adicionar produto\n");
    printf("2. Vender produto\n");
    printf("3. Verificar inventário\n");
    printf("4. Sair\n");
    printf("Escolha uma opcao: \n\n\n\n\n");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
      add_product();
      break;
    case 2:
      sell_product();
      break;
    case 3:
      check_inventory();
      break;
    case 4:
      printf("Saindo...\n");
      return 0;
    default:
      printf("Opção inválida! Tente novamente.\n");
    }
  }

  return 0;
}