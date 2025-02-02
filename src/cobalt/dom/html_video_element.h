// Copyright 2015 The Cobalt Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COBALT_DOM_HTML_VIDEO_ELEMENT_H_
#define COBALT_DOM_HTML_VIDEO_ELEMENT_H_

#include <string>

#include "cobalt/dom/html_media_element.h"
#include "cobalt/dom/video_playback_quality.h"
#include "cobalt/math/rect.h"
#include "cobalt/math/size_f.h"
#include "cobalt/script/environment_settings.h"
#if !defined(COBALT_MEDIA_SOURCE_2016)
#include "media/base/shell_video_frame_provider.h"
#endif  // !defined(COBALT_MEDIA_SOURCE_2016)

namespace cobalt {
namespace dom {

// The HTMLVideoElement is used to play videos.
//   https://www.w3.org/TR/html5/embedded-content-0.html#the-video-element
class HTMLVideoElement : public HTMLMediaElement {
 public:
#if defined(COBALT_MEDIA_SOURCE_2016)
  typedef media::VideoFrameProvider VideoFrameProvider;
#else   // defined(COBALT_MEDIA_SOURCE_2016)
  typedef ::media::ShellVideoFrameProvider VideoFrameProvider;
#endif  // defined(WebMediaPlayerDelegate)

  static const char kTagName[];

  explicit HTMLVideoElement(Document* document);

  // Web API: HTMLVideoElement
  //
  uint32 width() const;
  uint32 height() const;
  void set_width(uint32 width);
  void set_height(uint32 height);
  uint32 video_width() const;
  uint32 video_height() const;
  scoped_refptr<VideoPlaybackQuality> GetVideoPlaybackQuality(
      script::EnvironmentSettings* environment_settings) const;

  // Custom, not in any spec
  //
  // From HTMLElement
  scoped_refptr<HTMLVideoElement> AsHTMLVideoElement() override { return this; }

  scoped_refptr<VideoFrameProvider> GetVideoFrameProvider();

  WebMediaPlayer::SetBoundsCB GetSetBoundsCB();

  math::SizeF GetVideoSize() const;

  DEFINE_WRAPPABLE_TYPE(HTMLVideoElement);

 private:
  // Thread checker ensures that HTMLMediaElement::GetVideoFrameProvider() is
  // only called from the thread that the HTMLMediaElement is created in.
  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(HTMLVideoElement);
};

}  // namespace dom
}  // namespace cobalt

#endif  // COBALT_DOM_HTML_VIDEO_ELEMENT_H_
