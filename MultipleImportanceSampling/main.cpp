#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "../general/Tools.h"
#include "BasicMesh.h"

//
// 课程中提到我们可以对光源采样，也可以对 BSDF采样，那么两种方法怎样
// 才能结合起来？另外结合两种方法又有什么优势？你可以探索多重重要性采样的
// 方法，当渲染表面的粗糙度较低时，更多依赖于对 BSDF的采样，而当光源较小
// 时，更多依赖于对光源的采样。注意这里要求你先了解并实现对BSDF的重要性
// 采样，再考虑多重重要性采样。
//

int main() {
  PrintInfo("Project: MIS\n");

  Assimp::Importer importer;
  const auto obj_file_name = "../assets/cornell_box/cornell_box.obj";
  const auto assimp_flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                           aiProcess_JoinIdenticalVertices |
                           aiProcess_SortByPType;
  const aiScene *scene = importer.ReadFile(obj_file_name, assimp_flag);
  if (scene == nullptr) {
    PrintErr("Failed to load scene");
    return -1;
  }

  BasicMesh basic_mesh;
  basic_mesh.InitFromScene(*scene);

  return 0;
}
