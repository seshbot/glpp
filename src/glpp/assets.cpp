#ifdef _MSC_VER
#  include <windows.h>
#else
// #  include <dirent.h> // to access ::opendir() ::readdir() and ::closedir()
#  include <errno.h>
#  include <sys/stat.h>
#endif

#include <glpp/assets.h>

#include <cctype>

using namespace std::string_literals;

namespace glpp {

   namespace {
#ifdef _MSC_VER
      std::string get_last_error_message() {
         DWORD dw = GetLastError();
         if (ERROR_SUCCESS == dw) return "SUCCESS";

         LPVOID lpMsgBuf;
         FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&lpMsgBuf,
            0, NULL);

         auto result = std::string{ (LPCTSTR)lpMsgBuf };

         LocalFree(lpMsgBuf);

         return result;
      }

      bool is_path_valid(std::string const & path) {
         auto attr = GetFileAttributesA(path.c_str());
         return attr != INVALID_FILE_ATTRIBUTES;

         //WIN32_FIND_DATA FindFileData;
         //HANDLE handle = FindFirstFile(file, &FindFileData);
         //int found = handle != INVALID_HANDLE_VALUE;
         //if (found)
         //{
         //   FindClose(handle);
         //}
         //return found;
      }

      bool is_path_a_directory(std::string const & path) {
         auto path_copy = path;

         auto attr = GetFileAttributesA(path_copy.c_str());
         if (attr == INVALID_FILE_ATTRIBUTES) {
            throw std::runtime_error(get_last_error_message().c_str());
         }
         return 0 != (attr & FILE_ATTRIBUTE_DIRECTORY);
      }
#else
      std::string get_last_error_message() {
         switch (errno) {
         case EACCES: return "Search permission is denied for a component of the path prefix.";
         case EIO: return "An error occurred while reading from the file system.";
         case ELOOP: return "A loop exists in symbolic links encountered during resolution of the path argument.";
         case ENAMETOOLONG: return "The length of the path argument exceeds{ PATH_MAX } or a pathname component is longer than{ NAME_MAX }.";
         case ENOENT: return "A component of path does not name an existing file or path is an empty string.";
         case ENOTDIR: return "A component of the path prefix is not a directory.";
         case EOVERFLOW: return "The file size in bytes or the number of blocks allocated to the file or the file serial number cannot be represented correctly in the structure pointed to by buf.";
         default: return "unrecognised error";
         }
      }

      bool is_path_valid(std::string const & path) {
         struct ::stat st_buf;

         int status = ::stat(path.c_str(), &st_buf);
         if (status == 0)
            return true;
         if (errno == ENOENT)
            return false;
         throw std::runtime_error(get_last_error_message().c_str());
      }

      bool is_path_a_directory(std::string const & path) {
         struct ::stat st_buf;

         int status = ::stat(path.c_str(), &st_buf);
         if (status != 0) {
            throw std::runtime_error(get_last_error_message().c_str());
         }

         // also S_ISREG() and S_ISLNK()
         return S_ISDIR(st_buf.st_mode);
      }
#endif

      bool path_name_ends_in_separator(std::string const & path) {
         if (path.size() == 0) return false;
         auto final_char = path.back();
         return final_char == '/' || final_char == '\\';
      }

      std::string trim_separator_from_path(std::string const & path) {
         if (!path_name_ends_in_separator(path)) return path;
         return path.substr(0, path.size() - 1);
      }
   }
   
   
   //
   // path_t implementation
   //

   path_t::path_t(std::string path_in)
   : path(trim_separator_from_path(path_in))
   , found(is_path_valid(path))
   , is_directory(found && is_path_a_directory(path)) {
   }

   path_t path_t::base() const {
      return{ path.substr(0, path.size() - file_name().size()) };
   }

   std::string path_t::file_name() const {
      if (path.size() == 0) throw std::runtime_error("cannot get filename from empty path");

      auto found = path.find_last_of("/\\");
      if (found == std::string::npos) return path;
      return{ path.substr(found + 1) };
   }

   std::string path_t::file_name_extension() const {
      auto filename = file_name();
      auto found = filename.find_last_of(".");
      if (found == std::string::npos) return "";
      return{ filename.substr(found + 1) };
   }

   std::string path_t::file_name_without_extension() const {
      return file_name().substr(0, file_name().size() - file_name_extension().size());
   }

   path_t combine(path_t const & base, path_t const & suffix) {
      auto should_add_separator = base.path.size() != 0 && !path_name_ends_in_separator(base.path);
      if (should_add_separator) return{ base.path + '/' + suffix.path };
      return{ base.path + suffix.path };
   }

   std::string to_lower(std::string in) {
      for (auto & c : in) {
         c = std::tolower(c);
      }
      return in;
   }

   bool string_ends_with(std::string base, std::string suffix, bool case_sensitive = true) {
      if (base.size() < suffix.size()) return false;
      auto base_end = base.substr(base.size() - suffix.size());
      if (!case_sensitive) {
         base_end = to_lower(base_end);
         suffix = to_lower(suffix);
      }
      return base_end == suffix;
   }

   bool string_contains(std::string base, std::string test, bool case_sensitive = true) {
      if (base.size() < test.size()) return false;
      if (!case_sensitive) {
         base = to_lower(base);
         test = to_lower(test);
      }
      return base.find(test) != std::string::npos;
   }

   //
   // directory_archive_t implementation
   //

   directory_archive_t::directory_archive_t(path_t const & path_in)
   : path(path_in) {
      if (!path.found || !path.is_directory) throw std::runtime_error(("cannot create archive from invalid directory "s + path.path).c_str());
   }

   glpp::path_t directory_archive_t::id_to_file_path(std::string const & id) const {
      auto file_path = combine(path, { id });
      if (file_path.is_directory) throw std::runtime_error("cannot create scene from directory path");
      return file_path;
   }

   shader directory_archive_t::load_shader(std::string const & id) const {
      auto file_path = id_to_file_path(id);

      auto type = [&] {
         auto ext = file_path.file_name_extension();
         if (ext.size() != 0) {
            if (ext == "vs") return shader::type_t::vertex;
            if (ext == "vert") return shader::type_t::vertex;
            if (ext == "fs") return shader::type_t::fragment;
            if (ext == "frag") return shader::type_t::fragment;
         }

         auto base = file_path.file_name_without_extension();
         if (string_contains(base, "vert", false)) return shader::type_t::vertex;
         if (string_contains(base, "frag", false)) return shader::type_t::fragment;

         throw std::runtime_error(("could not deduce shader type from filename '"s + id).c_str());
      }();

      return shader::create_from_file(file_path.path, type);
   }
   
   scene_t directory_archive_t::load_scene(std::string const & id) const {
      auto file_path = id_to_file_path(id);

      return scene_t::create_from_file(file_path.path);
   }

   image_t directory_archive_t::load_image(std::string const & id) const {
      auto file_path = id_to_file_path(id);

      return image_t{ file_path.path };
   }


   //
   // archive_t implementation
   //

   archive_t archive_t::load_from_directory(path_t const & path) {
      return{ directory_archive_t {path} };
   }

   shader archive_t::load_shader(std::string const & id) const {
      return self_->load_shader_(id);
   }

   scene_t archive_t::load_scene(std::string const & id) const {
      return self_->load_scene_(id);
   }

   image_t archive_t::load_image(std::string const & id) const {
      return self_->load_image_(id);
   }
}

