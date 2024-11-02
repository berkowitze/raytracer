#ifndef LOAD_GLTF_H
#define LOAD_GLTF_H
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "gltf/tiny_gltf.h"

#include <memory>
#include "vec3.h"
#include "hittables/triangle.h"
#include "hittables/hittable_list.h"

using namespace tinygltf;
// When converting from gltf, this raytracer uses gltf.(y, -x, z) for the coordinate system

int add_gltf_to_world(hittable_list &world, Model model)
{
  auto white = make_shared<lambertian>(0.7);

  for (auto mesh : model.meshes)
  {
    for (auto primitive : mesh.primitives)
    {
      if (primitive.mode != 4)
      {
        printf("Primitive mode is not TRIANGLES\n");
        return -1;
      }

      auto index_accessor = model.accessors[primitive.indices];
      auto index_buffer_view = model.bufferViews[index_accessor.bufferView];
      auto index_buffer = model.buffers[index_buffer_view.buffer];
      if (index_accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
      {
        printf("Got index buffer type that is not unsigned short, got component %d\n", index_accessor.componentType);
        return -1;
      }
      uint16_t *indices = reinterpret_cast<uint16_t *>(&index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset]);

      auto position_accessor = model.accessors[primitive.attributes["POSITION"]];
      auto position_buffer_view = model.bufferViews[position_accessor.bufferView];
      auto position_buffer = model.buffers[position_buffer_view.buffer];
      float *positions = reinterpret_cast<float *>(&position_buffer.data[position_buffer_view.byteOffset + position_accessor.byteOffset]);

      auto normal_accessor = model.accessors[primitive.attributes["NORMAL"]];
      auto normal_buffer_view = model.bufferViews[normal_accessor.bufferView];
      auto normal_buffer = model.buffers[normal_buffer_view.buffer];
      float *normals = reinterpret_cast<float *>(&normal_buffer.data[normal_buffer_view.byteOffset + normal_accessor.byteOffset]);
      // auto base_color = model.materials[primitive.material].pbrMetallicRoughness.baseColorFactor;
      // auto material = make_shared<lambertian>(color(base_color[0], base_color[1], base_color[2]));
      auto material = make_shared<lambertian>(color(0.5, 0.5, 0.5));

      for (int i = 0; i < index_accessor.count / 3; i++)
      {
        int tri_index_0 = indices[i * 3];
        int tri_index_1 = indices[i * 3 + 1];
        int tri_index_2 = indices[i * 3 + 2];

        float pos_0_x = positions[3 * tri_index_0 + 0];
        float pos_0_y = positions[3 * tri_index_0 + 1];
        float pos_0_z = positions[3 * tri_index_0 + 2];

        float pos_1_x = positions[3 * tri_index_1 + 0];
        float pos_1_y = positions[3 * tri_index_1 + 1];
        float pos_1_z = positions[3 * tri_index_1 + 2];

        float pos_2_x = positions[3 * tri_index_2 + 0];
        float pos_2_y = positions[3 * tri_index_2 + 1];
        float pos_2_z = positions[3 * tri_index_2 + 2];

        float norm_x = normals[3 * tri_index_0 + 0];
        float norm_y = normals[3 * tri_index_0 + 1];
        float norm_z = normals[3 * tri_index_0 + 2];

        point3 pos_0 = point3(pos_0_y, -pos_0_x, pos_0_z);
        point3 pos_1 = point3(pos_1_y, -pos_1_x, pos_1_z);
        point3 pos_2 = point3(pos_2_y, -pos_2_x, pos_2_z);
        vec3 normal = vec3(-norm_x, norm_y, norm_z);

        world.add(make_shared<tri>(pos_0, pos_1, pos_2, normal, material));
      }
    }
  }
  return 0;
}

void set_camera_from_gltf(camera &cam, Model model)
{
  Node camera_node;
  for (auto node : model.nodes)
  {
    if (node.camera >= 0)
    {
      camera_node = node;
      break;
    }
  }
  if (camera_node.camera < 0)
  {
    std::cout << "No camera found in gltf file" << std::endl;
    exit(1);
  }

  quat rotation(vec3(camera_node.rotation[0], camera_node.rotation[1], camera_node.rotation[2]), camera_node.rotation[3]);
  //  = rotation * vec3(0, 0, -1);
  vec3 basis = rotation * vec3(0, 0, 1);
  cam.lookfrom = point3(camera_node.translation[1], -camera_node.translation[0], camera_node.translation[2]);
  cam.lookat = cam.lookfrom - vec3(basis.y(), -basis.x(), basis.z());
  // std::cout << "Lookfrom: " << cam.lookfrom << " l" << cam.lookfrom.length() << std::endl;
  // std::cout << "Lookat: " << cam.lookat << " l" << cam.lookat.length() << std::endl;
  // exit(1);

  cam.aspect_ratio = model.cameras[0].perspective.aspectRatio;
  cam.vfov = radians_to_degrees(model.cameras[0].perspective.yfov);
  cam.vup = vec3(0, 0, 1);
  cam.defocus_angle = 0;

  cam.image_width = 600;
  cam.samples_per_pixel = 100;
  cam.max_depth = 30;
  // light blue
  cam.background = color(0.70, 0.80, 1.00);
  // exit(1);
}

#endif