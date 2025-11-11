#include "Model.h"

int main() {
  Model model;
  model.InitFromObj("assets/cube_tesslated1.obj");

  model.ExportToObj("assets/cube_tesslated2.obj");
  return 0;
}