# Copyright 2014 The Cobalt Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This file is meant to be included by a GYP target that needs to copy the
# platform specific localized string files into <(PRODUCT_DIR)/content.


{
  'includes': [ '../build/contents_dir.gypi' ],

  'variables': {
    'inputs_i18n':
      '<!(find <(static_contents_source_dir)/i18n/platform/<(target_arch)/*.xlb)',
  },

  'copies': [
    {
      'destination': '<(static_contents_output_data_dir)/i18n',
      'files': ['<@(inputs_i18n)'],
    },
  ],

  'all_dependent_settings': {
    'variables': {
      'content_deploy_subdirs': [ 'i18n' ]
    }
  },
}
