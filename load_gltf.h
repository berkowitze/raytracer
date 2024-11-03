#ifndef LOAD_GLTF_H
#define LOAD_GLTF_H

#include <memory>
#include "vec3.h"
#include "hittables/triangle.h"
#include "hittables/hittable_list.h"

using namespace tinygltf;

// When converting from gltf, this raytracer uses gltf.(y, -x, z) for the coordinate system

vec3 read_vec3(float *buffer, int start_index)
{
  float x = buffer[start_index];
  float y = buffer[start_index + 1];
  float z = buffer[start_index + 2];
  return vec3(y, -x, z);
}

vec3 read_vec2(float *buffer, int start_index)
{
  float x = buffer[start_index];
  float y = buffer[start_index + 1];
  return vec3(x, 1 - y, 0);
}

float *get_accessor(const Model &model, const Primitive &primitive, std::string attribute_name)
{
  auto accessor = model.accessors.at(primitive.attributes.at(attribute_name));
  auto buffer_view = model.bufferViews[accessor.bufferView];
  auto buffer = model.buffers[buffer_view.buffer];
  float *positions = reinterpret_cast<float *>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
  return positions;
}

vertex read_vertex(float *positions, float *normals, float *uvs, int index)
{
  vec3 pos = read_vec3(positions, 3 * index);
  vec3 norm = read_vec3(normals, 3 * index);
  vec3 uv = read_vec2(uvs, 2 * index);
  return vertex(pos, norm, uv);
}

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

      shared_ptr<material> material;
      if (primitive.material < 0)
      {
        material = white;
      }
      else
      {
        auto pbr = model.materials[primitive.material].pbrMetallicRoughness;
        if (pbr.baseColorTexture.index >= 0)
        {
          // std::cout << "Using image texture for model named " << mesh.name << std::endl;
          int tex_index = pbr.baseColorTexture.index;
          auto image = model.images[model.textures[tex_index].source];
          material = make_shared<lambertian>(make_shared<image_texture>(image));
        }
        else
        {
          // std::cout << "Using base color factor for model named " << mesh.name << std::endl;
          material = make_shared<lambertian>(color(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]));
        }
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

      float *positions = get_accessor(model, primitive, "POSITION");
      float *normals = get_accessor(model, primitive, "NORMAL");
      float *uvs = get_accessor(model, primitive, "TEXCOORD_0");

      for (int i = 0; i < index_accessor.count / 3; i++)
      {
        int tri_index_0 = indices[i * 3];
        int tri_index_1 = indices[i * 3 + 1];
        int tri_index_2 = indices[i * 3 + 2];

        vertex v0 = read_vertex(positions, normals, uvs, tri_index_0);
        vertex v1 = read_vertex(positions, normals, uvs, tri_index_1);
        vertex v2 = read_vertex(positions, normals, uvs, tri_index_2);

        // std::cout << "Adding triangle with pos_0: " << pos_0 << " pos_1: " << pos_1 << " pos_2: " << pos_2 << " uv0: " << uv0 << " uv1: " << uv1 << " uv2: " << uv2 << std::endl;

        world.add(make_shared<tri>(v0, v1, v2, material));
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
  // cam.lookat = vec3();
  // std::cout << "Lookfrom: " << cam.lookfrom << " l" << cam.lookfrom.length() << std::endl;
  // std::cout << "Lookat: " << cam.lookat << " l" << cam.lookat.length() << std::endl;
  // exit(1);

  cam.aspect_ratio = model.cameras[0].perspective.aspectRatio;
  cam.vfov = radians_to_degrees(model.cameras[0].perspective.yfov);
  cam.vup = vec3(0, 0, 1);
  cam.defocus_angle = 0;

  cam.image_width = 400;
  cam.samples_per_pixel = 25;
  cam.max_depth = 15;
  // light blue
  cam.background = color(0.70, 0.80, 1.00);
  // exit(1);
}

#endif