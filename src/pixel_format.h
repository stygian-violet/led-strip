#ifndef PIXEL_FORMAT_H_INCLUDED
#define PIXEL_FORMAT_H_INCLUDED

#include <FAB_LED.h>

template<FAB_TDEF>
constexpr pixelFormat extract_pixel_format(const avrBitbangLedStrip<FAB_TVAR>* strip) {
  return colors;
}

template<pixelFormat pf> struct _PixelFormat {};
template<> struct _PixelFormat<pixelFormat::NONE> { typedef rgb_t type; };
template<> struct _PixelFormat<pixelFormat::RGB> { typedef rgb_t type; };
template<> struct _PixelFormat<pixelFormat::GRB> { typedef grb_t type; };
template<> struct _PixelFormat<pixelFormat::BGR> { typedef bgr_t type; };
template<> struct _PixelFormat<pixelFormat::RGBW> { typedef rgbw_t type; };
template<> struct _PixelFormat<pixelFormat::GRBW> { typedef grbw_t type; };
template<> struct _PixelFormat<pixelFormat::HBGR> { typedef hbgr_t type; };

template<pixelFormat pf> using PixelFormat = _PixelFormat<pf>::type;

#endif // PIXEL_FORMAT_H_INCLUDED
