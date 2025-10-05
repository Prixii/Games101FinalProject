#include "Model.h"

int main() {
  Model model;
  model.InitFromObj("assets/cube_tesslated2.obj");
  model.DoCatmullClarkSubdivision();

  model.ExportToObj("assets/cube_tesslated3.obj");
  return 0;
}