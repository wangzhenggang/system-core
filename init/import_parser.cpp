/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "import_parser.h"

#include <android-base/logging.h>

#include "util.h"

bool ImportParser::ParseSection(std::vector<std::string>&& args, const std::string& filename,
                                int line, std::string* err) {
    if (args.size() != 2) {
        *err = "single argument needed for import\n";
        return false;
    }

    std::string conf_file;
    bool ret = expand_props(args[1], &conf_file);
    if (!ret) {
        *err = "error while expanding import";
        return false;
    }

    LOG(INFO) << "Added '" << conf_file << "' to import list";
    if (filename_.empty()) filename_ = filename;
    imports_.emplace_back(std::move(conf_file), line);
    return true;
}

void ImportParser::EndFile() {
    auto current_imports = std::move(imports_);
    imports_.clear();
    for (const auto& [import, line_num] : current_imports) {
        if (!parser_->ParseConfig(import)) {
            PLOG(ERROR) << filename_ << ": " << line_num << ": Could not import file '" << import
                        << "'";
        }
    }
}
