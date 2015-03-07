// this file was manually generated for now, because I figured it wouldn't change much between 
// versions

#ifndef TYPES__H
#define TYPES__H

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

#include <cstddef>
#include <cstdint>
#include <type_traits>

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

namespace gl {
   // TODO: should we be using KHR for this stuff?
   using gl_proc = void(*)();
   using get_gl_proc_func = gl_proc(*)(const char *);

   using enum_t = unsigned int;
   using bitfield_t = unsigned int;
   using byte_t = std::int8_t;
   using short_t = short;
   using int_t = int;
   using int64_t = std::int64_t;
   using fixed_t = std::int32_t;
   using ubyte_t = std::uint8_t;
   using ushort_t = unsigned short;
   using uint_t = unsigned int;
   using uint64_t = std::uint64_t;
   using sizei_t = int;
   using float_t = float;
   using clampf_t = float;
   using double_t = double;
   using char_t = char;

   using sizeiptr_t = std::ptrdiff_t;
   using intptr_t = std::ptrdiff_t;

   using DEBUGPROC_t = void(APIENTRY *)(enum_t source, enum_t type, uint_t id, enum_t severity, sizei_t length, const char_t *message, const void *userParam);
   using DEBUGPROCARB_t = void(APIENTRY *)(enum_t source, enum_t type, uint_t id, enum_t severity, sizei_t length, const char_t *message, const void *userParam);
   using DEBUGPROCKHR_t = void(APIENTRY *)(enum_t source, enum_t type, uint_t id, enum_t severity, sizei_t length, const char_t *message, const void *userParam);

   using sync_t = struct __GLsync *;
   using eglImageOES_t = void *;

   // allows interfaces that take bitmasks to remain strongly typed (dont just take 'uint' or whatever)
   // i.e., allows
   //   void do_some_stuff(bitmask<myenum> mask) {
   //      if (newmask & myenum::verbose) ...
   //      if (newmask & myenum::enable_debug) ...
   //   }
   //   do_some_stuff(myenum::verbose | myenum::enable_debug);
   //   do_some_stuff(0x05); // WILL NOT COMPILE! not type safe
   template <typename FieldsT>
   struct bitmask {
      using enum_type = FieldsT;
      using value_type = typename std::underlying_type<FieldsT>::type;
      value_type value;

      bitmask(FieldsT f1) : value(static_cast<value_type>(f1)) {}
      bitmask(FieldsT f1, FieldsT f2) : value(static_cast<value_type>(f1) | static_cast<value_type>(f2)) {}
      bitmask(FieldsT f1, FieldsT f2, FieldsT f3) : value(static_cast<value_type>(f1) | static_cast<value_type>(f2) | static_cast<value_type>(f3)) {}
      bitmask(FieldsT f1, FieldsT f2, FieldsT f3, FieldsT f4) : value(static_cast<value_type>(f1) | static_cast<value_type>(f2) | static_cast<value_type>(f3) | static_cast<value_type>(f4)) {}
      bitmask(FieldsT f1, FieldsT f2, FieldsT f3, FieldsT f4, FieldsT f5) : value(static_cast<value_type>(f1) | static_cast<value_type>(f2) | static_cast<value_type>(f3) | static_cast<value_type>(f4) | static_cast<value_type>(f5)) {}
      bitmask(FieldsT f1, FieldsT f2, FieldsT f3, FieldsT f4, FieldsT f5, FieldsT f6) : value(static_cast<value_type>(f1) | static_cast<value_type>(f2) | static_cast<value_type>(f3) | static_cast<value_type>(f4) | static_cast<value_type>(f5) | static_cast<value_type>(f6)) {}

      friend bitmask<FieldsT> operator|(bitmask<FieldsT> f1, bitmask<FieldsT> f2) { return bitmask<FieldsT>::or_(f1.value, f2.value); }
      friend bitmask<FieldsT> operator&(bitmask<FieldsT> f1, bitmask<FieldsT> f2) { return bitmask<FieldsT>::and_(f1.value, f2.value); }
      friend bitmask<FieldsT> operator^(bitmask<FieldsT> f1, bitmask<FieldsT> f2) { return bitmask<FieldsT>::xor_(f1.value, f2.value); }
      friend bitmask<FieldsT> operator~(bitmask<FieldsT> f1) { return bitmask<FieldsT>::not_(f1.value); }

      static bitmask<FieldsT> from_underlying(value_type mask) { return{ mask }; }
      static bitmask<FieldsT> or_(FieldsT f1, FieldsT f2) { return{ f1.value, f2.value }; }
      static bitmask<FieldsT> and_(FieldsT f1, FieldsT f2) { return from_underlying(static_cast<value_type>(f1) & static_cast<value_type>(f2)); }
      static bitmask<FieldsT> xor_(FieldsT f1, FieldsT f2) { return from_underlying(static_cast<value_type>(f1) ^ static_cast<value_type>(f2)); }
      static bitmask<FieldsT> not_(FieldsT f1) { return from_underlying(~static_cast<value_type>(f1)); }

   private:
      bitmask(value_type mask) : value(mask) {}
   };

   // VC compiler cannot find these if they are declared as nested friend functions unfortunately
   #define CREATE_ENUM_BITMASK_TYPE(type_name, e) \
      using type_name = ::gl::bitmask<e>; \
      inline ::gl::bitmask<e> operator|(e f1, e f2) { return{ f1, f2 }; } \
      inline ::gl::bitmask<e> operator&(e f1, e f2) { return ::gl::bitmask<e>::and_(f1, f2); } \
      inline ::gl::bitmask<e> operator^(e f1, e f2) { return ::gl::bitmask<e>::xor_(f1, f2); } \
      inline ::gl::bitmask<e> operator~(e f1) { return ::gl::bitmask<e>::not_(f1); } 
}

#endif // #ifndef TYPES__H
