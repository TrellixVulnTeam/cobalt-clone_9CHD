// Copyright 2016 The Cobalt Authors. All Rights Reserved.
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

#include "cobalt/debug/page_component.h"

#include "base/bind.h"
#include "base/values.h"
#include "cobalt/dom/document.h"
#include "cobalt/math/matrix3_f.h"
#include "cobalt/math/transform_2d.h"
#include "cobalt/render_tree/brush.h"
#include "cobalt/render_tree/color_rgba.h"
#include "cobalt/render_tree/composition_node.h"
#include "cobalt/render_tree/font.h"
#include "cobalt/render_tree/rect_node.h"
#include "cobalt/render_tree/text_node.h"

namespace cobalt {
namespace debug {

namespace {
// Parameter field names:
const char kFrameId[] = "result.frameTree.frame.id";
const char kLoaderId[] = "result.frameTree.frame.loaderId";
const char kMimeType[] = "result.frameTree.frame.mimeType";
const char kResources[] = "result.frameTree.resources";
const char kSecurityOrigin[] = "result.frameTree.frame.securityOrigin";
const char kUrl[] = "result.frameTree.frame.url";

// Constant parameter values:
const char kFrameIdValue[] = "Cobalt";
const char kLoaderIdValue[] = "Cobalt";
const char kMimeTypeValue[] = "text/html";
}  // namespace

PageComponent::PageComponent(DebugDispatcher* dispatcher, dom::Window* window,
                             scoped_ptr<RenderLayer> render_layer,
                             render_tree::ResourceProvider* resource_provider)
    : window_(window),
      render_layer_(render_layer.Pass()),
      resource_provider_(resource_provider),
      ALLOW_THIS_IN_INITIALIZER_LIST(commands_(this)) {
  DCHECK(dispatcher);
  DCHECK(window_);
  DCHECK(window_->document());
  DCHECK(render_layer_);
  DCHECK(resource_provider_);

  commands_["Page.disable"] = &PageComponent::Disable;
  commands_["Page.enable"] = &PageComponent::Enable;
  commands_["Page.getResourceTree"] = &PageComponent::GetResourceTree;
  commands_["Page.setOverlayMessage"] = &PageComponent::SetOverlayMessage;

  dispatcher->AddDomain("Page", commands_.Bind());
}

void PageComponent::Disable(const Command& command) {
  command.SendResponse();
}

void PageComponent::Enable(const Command& command) {
  command.SendResponse();
}

void PageComponent::GetResourceTree(const Command& command) {
  JSONObject response(new base::DictionaryValue());
  response->SetString(kFrameId, kFrameIdValue);
  response->SetString(kLoaderId, kLoaderIdValue);
  response->SetString(kMimeType, kMimeTypeValue);
  response->SetString(kSecurityOrigin, window_->document()->url());
  response->SetString(kUrl, window_->document()->url());
  response->Set(kResources, new base::ListValue());
  command.SendResponse(response);
}

void PageComponent::SetOverlayMessage(const Command& command) {
  std::string message;
  JSONObject params = JSONParse(command.GetParams());
  bool got_message = false;
  if (params) {
    got_message = params->GetString("message", &message);
  }

  if (got_message) {
    const float font_size = 16.0f;
    const float padding = 12.0f;
    render_tree::ColorRGBA bg_color(1.0f, 1.0f, 0.75f, 1.0f);
    render_tree::ColorRGBA text_color(0.0f, 0.0f, 0.0f, 1.0f);

    scoped_refptr<render_tree::Font> font =
        resource_provider_->GetLocalTypeface("monospace",
                                             render_tree::FontStyle())
            ->CreateFontWithSize(font_size);
    scoped_refptr<render_tree::GlyphBuffer> glyph_buffer(
        resource_provider_->CreateGlyphBuffer(message, font));
    const math::RectF bounds(glyph_buffer->GetBounds());

    const float bg_width = bounds.width() + 2.0f * padding;
    const float bg_height = bounds.height() + 2.0f * padding;
    const float bg_x = 0.5f * (window_->inner_width() - bg_width);
    const float bg_y = padding;
    const float text_x = bg_x - bounds.x() + padding;
    const float text_y = bg_y - bounds.y() + padding;

    render_tree::CompositionNode::Builder composition_builder;
    composition_builder.AddChild(new render_tree::RectNode(
        math::RectF(bg_x, bg_y, bg_width, bg_height),
        scoped_ptr<render_tree::Brush>(
            new render_tree::SolidColorBrush(bg_color))));
    composition_builder.AddChild(new render_tree::TextNode(
        math::Vector2dF(text_x, text_y), glyph_buffer, text_color));

    render_layer_->SetFrontLayer(
        new render_tree::CompositionNode(composition_builder.Pass()));
  } else {
    render_layer_->SetFrontLayer(scoped_refptr<render_tree::Node>());
  }

  command.SendResponse();
}

}  // namespace debug
}  // namespace cobalt
