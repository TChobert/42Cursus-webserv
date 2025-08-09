#pragma once

#include "PostExecutor.hpp"
#include "MultipartParser.hpp"
#include <string>

void saveUploadedFile(const MultipartPart& part, Conversation& conv);
void storeFormField(const MultipartPart& part, Conversation& conv);
void saveFormSummary(const Conversation& conv);
