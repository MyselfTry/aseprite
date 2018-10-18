// Aseprite
// Copyright (C) 2018  Igara Studio S.A.
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/color_spaces.h"

#include "app/doc.h"
#include "app/modules/editors.h"
#include "app/ui/editor/editor.h"
#include "os/display.h"
#include "os/system.h"

namespace app {

os::ColorSpacePtr get_screen_color_space()
{
  return os::instance()->defaultDisplay()->colorSpace();
}

os::ColorSpacePtr get_current_color_space()
{
  if (current_editor)
    return current_editor->document()->osColorSpace();
  else
    return get_screen_color_space();
}

//////////////////////////////////////////////////////////////////////
// Color conversion

ConvertCS::ConvertCS()
{
  auto srcCS = get_current_color_space();
  auto dstCS = get_screen_color_space();
  if (srcCS && dstCS)
    m_conversion = os::instance()->convertBetweenColorSpace(srcCS, dstCS);
}

ConvertCS::ConvertCS(const os::ColorSpacePtr& srcCS,
                     const os::ColorSpacePtr& dstCS)
{
  m_conversion = os::instance()->convertBetweenColorSpace(srcCS, dstCS);
}

ConvertCS::ConvertCS(ConvertCS&& that)
  : m_conversion(std::move(that.m_conversion))
{
}

gfx::Color ConvertCS::operator()(const gfx::Color c)
{
  if (m_conversion) {
    gfx::Color out;
    m_conversion->convert((uint32_t*)&out, (const uint32_t*)&c, 1);
    return out;
  }
  else {
    return c;
  }
}

ConvertCS convert_from_current_to_screen_color_space()
{
  return ConvertCS();
}

ConvertCS convert_from_custom_to_srgb(const os::ColorSpacePtr& from)
{
  return ConvertCS(from,
                   os::instance()->createColorSpace(gfx::ColorSpace::MakeSRGB()));
}

} // namespace app