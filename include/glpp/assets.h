#ifndef GLPP_ASSETS__H
#define GLPP_ASSETS__H

#include <string>
#include <memory>

#include <glpp/glpp.h>
#include <glpp/scene.h>

#include <glpp/utils.h>

namespace glpp {
   struct path_t {
      path_t(std::string path);
      std::string path;
      bool found;
      bool is_directory;

      path_t base() const;
      std::string file_name() const; // assumes leaf is a filename
      std::string file_name_extension() const;
      std::string file_name_without_extension() const;

      friend path_t combine(path_t const & base, path_t const & suffix);
   };

   class directory_archive_t {
   public:
      directory_archive_t(path_t const & path);

      shader load_shader(std::string const & id) const;
      scene_t load_scene(std::string const & id) const;
      image_t load_image(std::string const & id) const;

   private:
      glpp::path_t id_to_file_path(std::string const & id) const;

      path_t path;
   };


   class archive_t {
   public:
      static archive_t load_from_directory(path_t const & path);

      shader load_shader(std::string const & id) const;
      scene_t load_scene(std::string const & id) const;
      image_t load_image(std::string const & id) const;
      
   private:
      template <typename T>
      archive_t(T impl) : self_(std::make_shared<model<T>>(std::move(impl))) {}

      struct concept_t {
         virtual ~concept_t() = default;
         virtual shader load_shader_(std::string const & id) const = 0;
         virtual scene_t load_scene_(std::string const & id) const = 0;
         virtual image_t load_image_(std::string const & id) const = 0;
      };

      template <typename T>
      struct model : concept_t {
         model(T data) : data_(std::move(data)) {}
         shader load_shader_(std::string const & id) const override { return data_.load_shader(id); }
         scene_t load_scene_(std::string const & id) const override { return data_.load_scene(id); }
         image_t load_image_(std::string const & id) const override { return data_.load_image(id); }

         T data_;
      };
      std::shared_ptr<const concept_t> self_;
   };
}


#endif // #ifndef GLPP_ASSETS__H
