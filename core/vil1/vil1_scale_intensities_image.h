#ifndef vil_scale_intensities_image_h_
#define vil_scale_intensities_image_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vil_scale_intensities_image - Support function for vil_scale_intensities_image_impl
// .INCLUDE vil/vil_scale_intensities_image.h
// .FILE vil_scale_intensities_image.cxx

#include <vcl_compiler.h>

class vil_image;

#define vil_decl(T) \
bool vil_scale_intensities_image(vil_image const &base, double scale, double shift, \
                                 T *buf, int x0, int y0, int w, int h)

template <class T> vil_decl(T);

#ifdef VCL_KAI
template <typename T> struct vil_rgb;
template <> vil_decl(vil_rgb<unsigned char>);
template <> vil_decl(vil_rgb<float>);
template <> vil_decl(vil_rgb<double>);
#endif

#undef vil_decl

#endif // vil_scale_intensities_image_h_
